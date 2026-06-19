// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Execution/ExecutionThornFeature.h"

#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Chaos/Utilities.h"
#include "Character/PlayerClass.h"
#include "Core/EntityClass.h"
#include "Core/SkillActor.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "Kismet/KismetMathLibrary.h"

void UExecutionThornFeature::CleanNiagara()
{
	Super::CleanNiagara();
}

void UExecutionThornFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UExecutionThornFeature::Execute(FSkillContext& InSkillContext)
{
	Super::Execute(InSkillContext);
	
	if (!InSkillContext.bActivated) return;
	if (InSkillContext.StartLocation.IsZero() || InSkillContext.EndLocation.IsZero()) return;
	
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionThornFeature::Execute - SkillInstance invalido."));
		return;
	}
	
	
	this->SpawnThorn(InSkillContext);
	
	SkillInstance->FinishSkill();
	SkillInstance->GoOnCooldown();
}

void UExecutionThornFeature::SpawnThorn(FSkillContext& InSkillContext)
{
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionThornFeature::SpawnThorn - SkillInstance invalido."));
		return;
	}
	
	const USkillDataAsset* SkillDataAsset = SkillInstance->GetDataAsset();
	if (!SkillDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionThornFeature::SpawnThorn - SkillDataAsset invalido."));
		return;
	}
	
	UNiagaraSystem* VFX = this->ExecutionEffect.Get();
	if (!VFX)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionThornFeature::SpawnThorn - VFX ExecutionEffect invalido para ExecutionFeature '%s'."), *GetNameSafe(this));
		return;
	}
	
	AEntityClass* EntityOwner = InSkillContext.EntityOwner.Get();
	if (!EntityOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionThornFeature::SpawnThorn - EntityOwner invalido."));
		return;
	}
	
	TArray<ESurfaceType> ValidSurfaces;
	ValidSurfaces.Add(ESurfaceType::Wall);
	ValidSurfaces.Add(ESurfaceType::Floor);
	
	TArray<FHitResult> ValidHits = CheckSurfaceInAim(InSkillContext, ValidSurfaces);

	if (ValidHits.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionThornFeature::SpawnThorn - Superfície inválida."));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("ExecutionThorn Feature OK"));
	
	const FVector SpawnLocation = InSkillContext.StartLocation;
	if (SpawnLocation.IsZero()) {UE_LOG(LogTemp, Error, TEXT("UExecutionThornFeature::SpawnThorn - SpawnLocation inválido.")); return;};

	const FVector SurfaceNormal = InSkillContext.StartSurfaceNormal;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("OPAAA %s"), *SurfaceNormal.ToString()));

	const FRotator SpawnRotation = !SurfaceNormal.IsZero()
		? UKismetMathLibrary::MakeRotFromXZ(InSkillContext.Direction, SurfaceNormal)
		: FRotator::ZeroRotator;
	
	FTransform SpawnTransform(SpawnRotation, SpawnLocation);
	
	ASkillActor* SkillActor = SpawnSkillActor(EntityOwner, SpawnTransform);
	
	SkillActor->NiagaraComponent->SetAsset(VFX);
	SkillActor->Initialize(SkillInstance, EntityOwner, InSkillContext);
	
	if (IsValid(SkillActor->NiagaraComponent) && SkillActor->NiagaraComponent->GetAsset() != nullptr)
	{
		TArray<FVector> ThornLocations;
		TArray<FQuat> ThornDirections;
		for (FHitResult Hit : ValidHits)
		{
			if (InSkillContext.EntityOnEndLocation.Get())
			{
				ThornDirections.Add(FRotationMatrix::MakeFromZ((InSkillContext.EntityOnEndLocation->GetActorLocation() - Hit.ImpactPoint).GetSafeNormal()).ToQuat());
			} else
			{
				FVector ThornDirection;
				FVector FinalEndLocation = InSkillContext.EndLocation;
				
				if (Hit.ImpactNormal.Z > 0.0f)
				{
					FinalEndLocation.Z = Hit.ImpactPoint.Z + 280.0f;
					
					ThornDirection = (FinalEndLocation - Hit.ImpactPoint).GetSafeNormal();
				} else if (Hit.ImpactNormal.Z < 0.0f)
				{
					FinalEndLocation.Z = Hit.ImpactPoint.Z - 280.0f;
					
					ThornDirection = (FinalEndLocation - Hit.ImpactPoint).GetSafeNormal();
				} else
				{
					ThornDirection = (FinalEndLocation - Hit.ImpactPoint).GetSafeNormal();
					
					Hit.ImpactNormal.Normalize();
					
					float Dot = FMath::Abs(FVector::DotProduct(ThornDirection, Hit.ImpactNormal));

					if (Dot <= 0.3f)
					{
						FVector DirecaoLocal = FVector(0.0f, 0.0f, 1.0f); // Z puro
						FRotator Rotator = UKismetMathLibrary::MakeRotFromZ(Hit.ImpactNormal);
						ThornDirection = Rotator.RotateVector(DirecaoLocal);
					}
				}
				
				ThornDirections.Add(FRotationMatrix::MakeFromZ(ThornDirection).ToQuat());
			}
			ThornLocations.Add(Hit.ImpactPoint);
		}
		
		SkillActor->NiagaraComponent->SetFloatParameter(FName("MinLifeSpan"), this->MinLifeSpan);
		SkillActor->NiagaraComponent->SetFloatParameter(FName("MaxLifeSpan"), this->MaxLifeSpan);
		SkillActor->NiagaraComponent->SetVectorParameter(FName("MaxScale"), FMath::Clamp(InSkillContext.ChargeRatio, 0.5, 1.0) * this->MaxScale);
		SkillActor->NiagaraComponent->SetVectorParameter(FName("MinScale"), this->MinScale);
		SkillActor->NiagaraComponent->SetFloatParameter(FName("ThornAmount"), FMath::Clamp(InSkillContext.ChargeRatio, 0.3, 1.0) * MaxThornsAmount);
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayQuat(SkillActor->NiagaraComponent, FName(TEXT("User.ThornDirection")), ThornDirections);
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(SkillActor->NiagaraComponent, FName(TEXT("User.ThornLocation")), ThornLocations);
		

		if (!SkillActor->NiagaraComponent->OnSystemFinished.IsAlreadyBound(this, &UExecutionThornFeature::OnNiagaraSystemFinished))
		{
			SkillActor->NiagaraComponent->OnSystemFinished.AddDynamic(this, &UExecutionThornFeature::OnNiagaraSystemFinished);
		}
		

	}
}

void UExecutionThornFeature::OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	Super::OnNiagaraSystemFinished(FinishedComponent);
	AActor* Actor = FinishedComponent->GetOwner();
	if (IsValid(Actor))
		Actor->Destroy();
}

void UExecutionThornFeature::OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	Super::OnAuraNiagaraSystemFinished(FinishedComponent);
}


void UExecutionThornFeature::ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext)
{
	// if (this->ParticlesProcessed >= Data.Num()) return;
	FVector StartLocation = SkillContext.StartLocation; // ou seu Start Location real
	FVector TargetLocation = SkillContext.EndLocation;    // ou seu Target real
    TargetLocation.Z += 0.0f;
	
	// O vetor que aponta do Start para o Target
	FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();
	
	for (const FBasicParticleData& Particle : Data)
	{
		this->ParticlesProcessed++;
		
		// Extraindo os dados básicos
		FVector ParticleScale = Particle.Velocity;
		float BaseMeshHeight = 99.0f;
		float ScaledLength = BaseMeshHeight * ParticleScale.Z;
		FVector TipPosition = Particle.Position + (Direction * ScaledLength);
		TipPosition.Z -= 0.0f;
		
		// 1. Definir a forma da colisão (usando a metade do tamanho, como na Box)
		FCollisionShape MyHitSphere = FCollisionShape::MakeSphere(2.0f);
		// 2. Lista para guardar quem batemos
		TArray<FOverlapResult> OutOverlaps;
		FCollisionQueryParams QueryParams;
		
		ASkillActor* SkillActor = SkillContext.SkillActor.Get();
		AEntityClass* EntityOwner = SkillContext.EntityOwner.Get();
		
		if (!IsValid(EntityOwner) || !IsValid(SkillActor)) return;
		
		QueryParams.AddIgnoredActor(SkillActor); // Ignora o próprio projétil/espinho
		if (EntityOwner) QueryParams.AddIgnoredActor(Cast<AActor>(EntityOwner)); // Ignora quem lançou
		
		// 3. Executar o teste de colisão
		bool bHit = GetWorld()->OverlapMultiByChannel(
			OutOverlaps,
			TipPosition, // Centro da caixa
			FQuat::Identity,
			ECC_Pawn,
			MyHitSphere,
			QueryParams
		);		

		if (bHit)
		{
			for (FOverlapResult OutOverlap : OutOverlaps)
			{
				AActor* HitActor = OutOverlap.GetActor();
				if (HitActor && HitActor->IsA(AEntityClass::StaticClass()))
				{
					// 1. Converter o ID (que está no Size) para INT para evitar bugs de float no Map
					int32 PId = FMath::RoundToInt(Particle.Size);
					FName ActorName = HitActor->GetFName();

					// 2. Usar FindOrAdd para garantir que o par exista antes de acessar
					FEntityArrayWrapper& Wrapper = this->ParticlesIDCollided.FindOrAdd(PId);

					// 3. Verificar se este Actor específico já foi atingido por ESTA partícula
					if (!Wrapper.Entities.Contains(ActorName))
					{
						UE_LOG(LogTemp, Warning, TEXT("ID %d Machucou: %s"), PId, *ActorName.ToString());
                    
						Wrapper.Entities.Add(ActorName);
						// AQUI: Chame sua função de aplicar dano real
					}
				}
			}
		}
		
	}
}

TArray<FHitResult> UExecutionThornFeature::CheckSurfaceInAim(FSkillContext& InSkillContext, TArray<ESurfaceType>& InValidSurfaces)
{
	APlayerClass* Char = Cast<APlayerClass>(InSkillContext.EntityOwner);
	
	FVector Start = Char->GetCameraComponent()->GetComponentLocation();
	FVector End = Start + (InSkillContext.StartLocation - Start) * MaxRange;
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Char);
	
	return LineTraceAroundLocation(Start, End, CollisionParams, InValidSurfaces);
}

TArray<FHitResult> UExecutionThornFeature::LineTraceAroundLocation(FVector StartLocation, FVector EndLocation,
                                                          FCollisionQueryParams CollisionParams,
                                                          TArray<ESurfaceType>& ValidSurfaces)
{
	FVector Forward = (EndLocation - StartLocation).GetSafeNormal();
    
	// Calcula os eixos relativos à direção do tiro
	// O vetor 'Right' é sempre perpendicular ao Forward e ao Z Global
	FVector Right = FVector::CrossProduct(FVector::UpVector, Forward).GetSafeNormal();
    
	// O vetor 'Up' é perpendicular ao Forward e ao Right (ajusta para inclinações da câmera)
	FVector Up = FVector::CrossProduct(Forward, Right).GetSafeNormal();
	
	FVector RightUp = (Right + Up).GetSafeNormal();
	
	TArray<FVector> Offsets;
	
	TArray<FVector> Vectors = {Right, Up, RightUp}
	;
	for (int i = 0; i < MaxThornsAmount-1; i++)
	{
		float SpacingBetweenThorns = FMath::RandRange(12.0f, MaxSpacingBetweenThorns);
		
		switch (FMath::RandRange(0,1))
		{
			case 0:
				Offsets.Add(Vectors[FMath::RandRange(0, 2)] * -SpacingBetweenThorns);
				break;
			case 1:
				Offsets.Add(Vectors[FMath::RandRange(0, 2)] * SpacingBetweenThorns);
				break;
			default:
				break;
		}
	}
	

	TArray<FHitResult> ExternTraces;
	
	// Executa as traces ao redor
	for (const FVector& Offset : Offsets)
	{
		FVector NewStart = StartLocation + Offset;
		FVector NewEnd = EndLocation + Offset;

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, NewStart, NewEnd, ECC_Visibility, CollisionParams);
		DrawDebugLine(GetWorld(), NewStart, NewEnd, FColor::Red, false, 5.0f);
		
		if (Hit.bBlockingHit)
			ExternTraces.Add(Hit);
	}

	// Trace central original
	FHitResult CentralHit;
	GetWorld()->LineTraceSingleByChannel(CentralHit, StartLocation, EndLocation, ECC_Visibility, CollisionParams);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 5.0f);
	
	TArray<FHitResult> OutValidHits;
	OutValidHits.Add(CentralHit);
	
	for (FHitResult ExternHit : ExternTraces)
	{
		float DistanceOfCentralHit = FVector::Dist(CentralHit.ImpactPoint, ExternHit.ImpactPoint);
		if (DistanceOfCentralHit < 200.0f)
		{
			OutValidHits.Add(ExternHit);
		}
	}
	
	return OutValidHits;
}
