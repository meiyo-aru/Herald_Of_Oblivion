// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Execution/ExecutionThornFeature.h"

#include "NiagaraComponent.h"
#include "Core/EntityClass.h"
#include "Core/SkillActor.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "Kismet/KismetMathLibrary.h"

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
	
	if (!SkillActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionThornFeature::SpawnThorn - Falha ao spawnar Skill Actor."));
		return;
	}
	
	FVector TargetLocation = InSkillContext.EndLocation - InSkillContext.StartLocation;
	TargetLocation.Z += this->ModifierTarget;
	
	FVector DirectionWithoutZ = (InSkillContext.EndLocation - InSkillContext.StartLocation).GetSafeNormal();
	DirectionWithoutZ.Z = 0.0f;
	
	if (this->MinLifeSpan > 0.0f)
	{
		SkillActor->SetLifeSpan(this->MinLifeSpan);
	}

	// Inicializa o Actor
	SkillActor->Initialize(SkillInstance, EntityOwner, InSkillContext);
	SkillActor->SkillContext = InSkillContext;

	if (IsValid(SkillActor->NiagaraComponent))
	{
		SkillActor->NiagaraComponent->SetFloatParameter(FName("MinLifeSpan"), this->MinLifeSpan);
		SkillActor->NiagaraComponent->SetFloatParameter(FName("MaxLifeSpan"), this->MaxLifeSpan);
		SkillActor->NiagaraComponent->SetFloatParameter(FName("ModifierOffsetThorns"), this->ModifierOffsetThorns);
		SkillActor->NiagaraComponent->SetVectorParameter(FName("MaxScale"), this->MaxScale);
		SkillActor->NiagaraComponent->SetVectorParameter(FName("MinScale"), this->MinScale);
		SkillActor->NiagaraComponent->SetVectorParameter(FName("TargetLocation"), TargetLocation);
		SkillActor->NiagaraComponent->SetVectorParameter(FName("DirectionWithoutZ"), DirectionWithoutZ);

		if (!SkillActor->NiagaraComponent->OnSystemFinished.IsAlreadyBound(this, &UExecutionThornFeature::OnNiagaraSystemFinished))
		{
			SkillActor->NiagaraComponent->OnSystemFinished.AddDynamic(this, &UExecutionThornFeature::OnNiagaraSystemFinished);
		}
		SkillActor->NiagaraComponent->Activate();
	}
}

void UExecutionThornFeature::OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	Super::OnNiagaraSystemFinished(FinishedComponent);
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
    TargetLocation.Z += this->ModifierTarget;
	
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
		TipPosition.Z -= this->ModifierOffsetThorns;
		
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
