#include "SkillFeatures/SkillFeature.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Core/EntityClass.h"
#include "Core/SkillActor.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"

#include "Structs/SkillStructs.h"


UNiagaraComponent* USkillFeature::SpawnAuraVFX(UNiagaraSystem* VFX, const USkillDataAsset* InSkillDataAsset, AEntityClass* InEntityOwner, FSkillContext InSkillContext, EAuraType AuraType, USceneComponent* Target, FName AttachSocketName)
{	
	if (!InEntityOwner || !InSkillDataAsset || !VFX || !Target ) return nullptr;
	
	if (InSkillDataAsset->bAuraInStaticMesh || InSkillDataAsset->bAuraInSkeletalMesh)
	{
		
		UNiagaraComponent* DetachedNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(
			VFX,
			Target,
			AttachSocketName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);

		DetachedNiagara->SetFloatParameter(FName("MinCastTime"), InSkillDataAsset->MaxCastTime);
		DetachedNiagara->SetFloatParameter(FName("MaxCastTime"), InSkillDataAsset->MaxCastTime);
		DetachedNiagara->SetFloatParameter(FName("NormalOffsetAura"), InSkillDataAsset->NormalOffsetAura);
		DetachedNiagara->SetFloatParameter(FName("SpawnRateAura"), InSkillDataAsset->SpawnRateAura);
		DetachedNiagara->SetFloatParameter(FName("MaxLifeTimeAura"), InSkillDataAsset->MaxLifeTimeAura);
		DetachedNiagara->SetFloatParameter(FName("MinLifeTimeAura"), InSkillDataAsset->MinLifeTimeAura);
		
		switch (AuraType)
		{
			case EAuraType::SkeletalMesh:
				{
					UNiagaraFunctionLibrary::OverrideSystemUserVariableSkeletalMeshComponent(DetachedNiagara,
						TEXT("SkeletalMeshAura"),
						Cast<USkeletalMeshComponent>(InEntityOwner->GetMesh()));
					break;
				}
			case EAuraType::StaticMesh:
				{
					UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(Target);
					StaticMeshComp->GetStaticMesh()->bAllowCPUAccess = true;
					UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(DetachedNiagara,
						TEXT("StaticMeshAura"),
						Cast<UStaticMeshComponent>(StaticMeshComp));
					break;
				}
			default:
				break;
		}
		this->Context =  InSkillContext;
		DetachedNiagara->OnSystemFinished.AddDynamic(this, &USkillFeature::OnAuraNiagaraSystemFinished);
		return DetachedNiagara;
	}
	return nullptr;
}
ASkillActor* USkillFeature::SpawnSkillActor(AEntityClass* InEntityOwner, FTransform SpawnTransform)
{
	if (!IsValid(InEntityOwner) || !GetWorld()) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InEntityOwner;
	SpawnParams.Instigator = Cast<APawn>(InEntityOwner);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn imediato evita estados intermediários de SpawnDeferred + FinishSpawning.
	return GetWorld()->SpawnActor<ASkillActor>(
		ASkillActor::StaticClass(),
		SpawnTransform,
		SpawnParams
	);
}

UNiagaraComponent* USkillFeature::SpawnVFXAtLocation(UNiagaraSystem* VFX, FRotator Rotation, FVector Location)
{
	// if (!InEntityOwner || !InSkillDataAsset) return nullptr;
	
	UE_LOG(LogTemp, Warning, TEXT("VFX At Location"));
	
	if (IsValid(VFX))
	{
		UE_LOG(LogTemp, Warning, TEXT("CastEffect Load"));
		
		UNiagaraComponent* DetachedNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			VFX,
			Location,
			Rotation,
			FVector(1.0f),
			true, 
			true,
			ENCPoolMethod::AutoRelease
		);
		
		DetachedNiagara->OnSystemFinished.AddDynamic(this, &USkillFeature::OnNiagaraSystemFinished);
		return DetachedNiagara;
	}
	return nullptr;
}

UNiagaraComponent* USkillFeature::SpawnCastVfx(const USkillDataAsset* InSkillDataAsset, AEntityClass* InEntityOwner, FSkillContext InSkillContext, FVector TargetLocation)
{
	if (!InEntityOwner || !InSkillDataAsset) return nullptr;
	
	if (UNiagaraSystem* CastVFX = InSkillDataAsset->CastEffect.LoadSynchronous())
	{
		UE_LOG(LogTemp, Warning, TEXT("CastEffect Load"));
		
		UNiagaraComponent* DetachedNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			CastVFX,
			TargetLocation,
			InEntityOwner->GetActorRotation(),
			FVector(1.0f),
			true, 
			true,
			ENCPoolMethod::AutoRelease
		);
	
		if (!InSkillContext.Direction.IsZero())
			DetachedNiagara->SetVectorParameter(FName("Direction"), InSkillContext.Direction);
		
		DetachedNiagara->SetFloatParameter(FName("MinCastTime"), InSkillDataAsset->MinCastTime);
		DetachedNiagara->SetFloatParameter(FName("MaxCastTime"), InSkillDataAsset->MaxCastTime);
			
		this->Context = InSkillContext;
		
		DetachedNiagara->OnSystemFinished.AddDynamic(this, &USkillFeature::OnNiagaraSystemFinished);
		return DetachedNiagara;
	}
	return nullptr;
}

void USkillFeature::OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	FinishedComponent->OnSystemFinished.RemoveAll(this);
	
	USkillInstance* SkillInstance = this->Context.SkillInstance.Get();
	
	if (SkillInstance)
		SkillInstance->ReleaseAuraHandle();
}

void USkillFeature::OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	FinishedComponent->OnSystemFinished.RemoveAll(this);
}

TArray<FOverlapResult> USkillFeature::MakeHitSphere(float Radius, FSkillContext& InSkillContext, FVector Location)
{
	// 1. Definir a forma da colisão
	FCollisionShape MyHitSphere = FCollisionShape::MakeSphere(Radius);
	// 2. Lista para guardar quem batemos
	TArray<FOverlapResult> OutOverlaps;
	FCollisionQueryParams QueryParams;
	
	ASkillActor* SkillActor = InSkillContext.SkillActor.Get();
	AEntityClass* EntityOwner = InSkillContext.EntityOwner.Get();
	
	TArray<AActor*> IgnoredActors;
	
	if (IsValid(EntityOwner))
		IgnoredActors.Add(EntityOwner);	
	
	if (IsValid(SkillActor))
		IgnoredActors.Add(SkillActor);
	
	UE_LOG(LogTemp, Warning, TEXT("MakeHit"));
	
	QueryParams.AddIgnoredActors(IgnoredActors);
	// DrawDebugSphere(GetWorld(), Location, Radius, 30.0f, FColor::Red, false, 5.0f);
	
	// 3. Executar o teste de colisão
	GetWorld()->OverlapMultiByChannel(
		OutOverlaps,
		Location, // Centro da caixa
		FQuat::Identity,
		ECC_Pawn,
		MyHitSphere,
		QueryParams
	);	
	
	return OutOverlaps;
}

void USkillFeature::OnHitEntity(FSkillContext& InSkillContext)
{
	
}
