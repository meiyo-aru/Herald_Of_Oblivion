#include "SkillFeatures/SkillFeature.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Core/EntityClass.h"
#include "Core/SkillActor.h"
#include "Structs/SkillStructs.h"



UNiagaraComponent* USkillFeature::SpawnAuraVFX(UNiagaraSystem* VFX, AEntityClass* InEntityOwner, FSkillContext InSkillContext, EAuraType AuraType, USceneComponent* Target, FName AttachSocketName)
{	
	if (!InEntityOwner || !VFX || !Target ) return nullptr;

	UNiagaraComponent* AttachedNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(
		VFX,
		Target,
		AttachSocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true
	);
	
	switch (AuraType)
	{
		case EAuraType::SkeletalMesh:
			{
				if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(InEntityOwner->GetMesh()))
				{
					UNiagaraFunctionLibrary::OverrideSystemUserVariableSkeletalMeshComponent(AttachedNiagara,
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
					UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(AttachedNiagara,
						TEXT("Static Mesh"),
						Cast<UStaticMeshComponent>(StaticMeshComp));
				}
				break;
			}
		default:
			break;
	}
	return AttachedNiagara;
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

UNiagaraComponent* USkillFeature::SpawnVFXAtLocation(UNiagaraSystem* VFX, FRotator Rotation, FVector Location, FSkillContext& InSkillContext)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (IsValid(VFX))
		{
			UNiagaraComponent* DetachedNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				World,
				VFX,
				Location,
				Rotation,
				FVector(1.0f),
				true, 
				true,
				ENCPoolMethod::AutoRelease
			);

	
			if (!DetachedNiagara)
			{
				UE_LOG(LogTemp, Error, TEXT("SpawnVFXAtLocation falhou: O asset do Niagara não está carregado na RAM!"));
				return nullptr; // Aborta para não crashar a engine
			}
			DetachedNiagara->SetFloatParameter(FName("ChargeRatio"), InSkillContext.ChargeRatio);
			if (!InSkillContext.Direction.IsZero())
				DetachedNiagara->SetVectorParameter(FName("Direction"), InSkillContext.Direction);
		
			return DetachedNiagara;
		}
	}
	return nullptr;
}

void USkillFeature::CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents)
{
	for (TWeakObjectPtr NC : SpawnedNiagaraComponents)
	{
		if (UNiagaraComponent* Niagara = NC.Get())
		{
			Niagara->SetAutoDestroy(true);
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
