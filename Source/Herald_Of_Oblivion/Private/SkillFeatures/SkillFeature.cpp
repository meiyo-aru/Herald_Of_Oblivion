#include "SkillFeatures/SkillFeature.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Core/EntityClass.h"
#include "Core/SkillActor.h"
#include "Structs/SkillStructs.h"
#include "Subsystems/PoolingManager.h"

void USkillFeature::WarmupNiagara(UNiagaraSystem* Niagara)
{
	if (IsValid(Niagara))
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
		{
			
			if (UNiagaraComponent* NC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World,
				Niagara, 
				FVector::ZeroVector,
				FRotator::ZeroRotator, 
				FVector(1), 
				false,
				false,
				ENCPoolMethod::None))
			{
				NC->SetVisibility(false);
				NC->Activate(true);
				NC->AdvanceSimulation(1.0f/60.0f, 2);
				NC->DeactivateImmediate();
				if (UPoolingManager* PoolingManager = World->GetGameInstance()->GetSubsystem<UPoolingManager>())
					PoolingManager->SaveNiagaraInPool(NC);
			}
		}
	}
}

UNiagaraComponent* USkillFeature::SpawnAuraVFX(UNiagaraSystem* VFX, AEntityClass* InEntityOwner, FSkillContext InSkillContext, EAuraType AuraType, USceneComponent* Target, FName AttachSocketName)
{	
	if (!InEntityOwner || !VFX || !Target ) return nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UPoolingManager* PoolingManager = World->GetGameInstance()->GetSubsystem<UPoolingManager>())
		{
			if (UNiagaraComponent* Niagara = PoolingManager->GetNiagaraComponentFromPool(VFX))
			{
				Niagara->SetAbsolute(false, false, false); 
				Niagara->AttachToComponent(Target, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocketName);

				Niagara->Activate(true);
				Niagara->SetVisibility(true);

				Niagara->OnSystemFinished.AddDynamic(this, &USkillFeature::OnNiagaraSystemFinished);
				
				switch (AuraType)
				{
				case EAuraType::SkeletalMesh:
					{
						if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(InEntityOwner->GetMesh()))
						{
							UNiagaraFunctionLibrary::OverrideSystemUserVariableSkeletalMeshComponent(Niagara,
								TEXT("Skeletal Mesh"),
								SkeletalMeshComponent);
						}
						break;
					}
				case EAuraType::StaticMesh:
					{
						if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(Target))
						{
							StaticMeshComp->GetStaticMesh()->bAllowCPUAccess = true;
							UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(Niagara,
								TEXT("Static Mesh"),
								Cast<UStaticMeshComponent>(StaticMeshComp));
						}
						break;
					}
				default:
					break;
				}
				return Niagara;
			}
		}
	}
	return nullptr;
}
ASkillActor* USkillFeature::SpawnSkillActor(AEntityClass* InEntityOwner, FTransform SpawnTransform)
{
	if (!IsValid(InEntityOwner)) return nullptr;
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = InEntityOwner;
		SpawnParams.Instigator = Cast<APawn>(InEntityOwner);
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Spawn imediato evita estados intermediários de SpawnDeferred + FinishSpawning.
		return World->SpawnActor<ASkillActor>(
			ASkillActor::StaticClass(),
			SpawnTransform,
			SpawnParams
		);
	}
	return nullptr;
}

UNiagaraComponent* USkillFeature::SpawnVFXAtLocation(UNiagaraSystem* NiagaraSystem, FRotator Rotation, FVector Location, FSkillContext& InSkillContext)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UPoolingManager* PoolingManager = World->GetGameInstance()->GetSubsystem<UPoolingManager>())
			if (UNiagaraComponent* Niagara = PoolingManager->GetNiagaraComponentFromPool(NiagaraSystem))
			{
				Niagara->SetAbsolute(false, false, false); 

				Niagara->SetWorldLocation(Location);
				Niagara->SetWorldRotation(Rotation);
				Niagara->Activate(true);
				Niagara->SetVisibility(true);
				
				if (!InSkillContext.Direction.IsZero())
					Niagara->SetVectorParameter(FName("Direction"), InSkillContext.Direction);
			
				Niagara->OnSystemFinished.AddDynamic(this, &USkillFeature::OnNiagaraSystemFinished);
				
				return Niagara;
			}
	}
	return nullptr;
}

void USkillFeature::OnNiagaraSystemFinished(UNiagaraComponent* NiagaraComponent)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
		if (UPoolingManager* PoolingManager = World->GetGameInstance()->GetSubsystem<UPoolingManager>())
		{
			NiagaraComponent->OnSystemFinished.RemoveAll(this);
			PoolingManager->SaveNiagaraInPool(NiagaraComponent);
		}
}

void USkillFeature::CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents)
{
	for (TWeakObjectPtr NC : SpawnedNiagaraComponents)
	{
		if (UNiagaraComponent* Niagara = NC.Get())
		{
			UE_LOG(LogTemp, Log, TEXT("Niagara System Finished: %s"), *Niagara->GetName());
			Niagara->Deactivate();
		}	
	}
	SpawnedNiagaraComponents.Empty();
}

TArray<FOverlapResult>  USkillFeature::MakeOverlapBox(FVector Size, FQuat Rotation, FSkillContext& InSkillContext, FVector Center)
{	
	// 1. Definir a forma da colisão
	FCollisionShape MyOverlapBox = FCollisionShape::MakeBox(Size);
	// 2. Lista para guardar quem batemos
	TArray<FOverlapResult> OutOverlaps;
	FCollisionQueryParams QueryParams;
	
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		
		TArray<AActor*> IgnoredActors;
		
		if (AEntityClass* EntityOwner = Cast<AEntityClass>(InSkillContext.EntityOwner.Get()))
			IgnoredActors.Add(EntityOwner);	
		
		if (ASkillActor* SkillActor = InSkillContext.SkillActor.Get())
			IgnoredActors.Add(SkillActor);
		
		QueryParams.AddIgnoredActors(IgnoredActors);
		
		// 3. Executar o teste de colisão
		World->OverlapMultiByChannel(
			OutOverlaps,
			Center, // Centro da caixa
			Rotation,
			ECC_Pawn,
			MyOverlapBox,
			QueryParams
		);	
		
	}
	return OutOverlaps;
}

TArray<FOverlapResult> USkillFeature::MakeOverlapSphere(float Radius, FSkillContext& InSkillContext, FVector Center)
{
	// 1. Definir a forma da colisão
	FCollisionShape MyOverlapSphere = FCollisionShape::MakeSphere(Radius);
	// 2. Lista para guardar quem batemos
	TArray<FOverlapResult> OutOverlaps;
	FCollisionQueryParams QueryParams;
	
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		ASkillActor* SkillActor = InSkillContext.SkillActor.Get();
		AEntityClass* EntityOwner = Cast<AEntityClass>(InSkillContext.EntityOwner.Get());
		
		TArray<AActor*> IgnoredActors;
		
		if (IsValid(EntityOwner))
			IgnoredActors.Add(EntityOwner);	
		
		if (IsValid(SkillActor))
			IgnoredActors.Add(SkillActor);
		
		QueryParams.AddIgnoredActors(IgnoredActors);
		// DrawDebugSphere(GetWorld(), Location, Radius, 30.0f, FColor::Red, false, 5.0f);
		
		// 3. Executar o teste de colisão
		World->OverlapMultiByChannel(
			OutOverlaps,
			Center, // Centro da caixa
			FQuat::Identity,
			ECC_Pawn,
			MyOverlapSphere,
			QueryParams
		);	
		
	}
	return OutOverlaps;
}
