// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/OnHit/OnHitSurface.h"

#include "NiagaraSystem.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

void UOnHitSurface::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
	if (Owner) 
	{
		// Delegate disparado ao feature de target terminar
		Owner->OnHitSurface.AddUObject(this, &UOnHitSurface::Execute);
	}; 
}

void UOnHitSurface::Execute(FSkillContext& InSkillContext, FVector TargetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("UOnHitActor::Execute"));
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UOnHitActor::Execute - SkillInstance invalido."));
		return;
	}
	
	const USkillDataAsset* SkillDataAsset = SkillInstance->GetDataAsset();
	if (!SkillDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UOnHitActor::Execute - SkillDataAsset invalido."));
		return;
	}
	
	UNiagaraSystem* VFX = SkillDataAsset->ExecutionEffect.LoadSynchronous();
	if (!IsValid(VFX))
	{
		UE_LOG(LogTemp, Error, TEXT("UOnHitActor::Execute - VFX ExecutionEffect invalido para SkillDataAsset '%s'."), *GetNameSafe(SkillDataAsset));
		return;
	}

	if (InSkillContext.SkillStage == ESkillStage::InExecution)
	{
		InSkillContext.SkillStage = ESkillStage::Hitted;
	} else
	{
		InSkillContext.SkillStage = ESkillStage::Persistent;
	}
	
	if (InSkillContext.HitOverlapResultType == EHitOverlapResultType::Hit)
	{
		const FHitResult& Hit = InSkillContext.HitResult;
		SpawnVFXAtLocation(VFX, UKismetMathLibrary::MakeRotFromZ(Hit.ImpactNormal), Hit.ImpactPoint);
		return;
	}

	
	// if (InSkillContext.HitOverlapResultType == EHitOverlapResultType::Overlap)
	// {		
	// 	const FOverlapResult& Overlap = InSkillContext.OverlapResult;
	// 	if (AActor* HitActor = Overlap.GetActor())
	// 	{
	// 		SpawnVFXAtLocation(VFX, FRotator::ZeroRotator, HitActor->GetActorLocation());
	// 	}
	// }
}
