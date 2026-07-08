// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillFeatures/OnHit/OnHitApplyEffect.h"
#include "Structs/SkillStructs.h"

#include "Core/SkillInstance.h"
#include "Data/EffectDataAsset.h"

void UOnHitApplyEffect::LoadFXSync()
{
	Super::LoadFXSync();
}

void UOnHitApplyEffect::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UOnHitApplyEffect::Execute(FSkillContext& InSkillContext)
{
	Super::Execute(InSkillContext);
	
	if (OnHitEffects.IsEmpty()) return;
	
	if (!InSkillContext.HitOverlapResult.OverlapResults.IsEmpty())
	{		
		for (FOverlapResult OverlapResult : InSkillContext.HitOverlapResult.OverlapResults)
		{
			if (OverlapResult.bBlockingHit)
			{
				if (AEntityClass* EntityOverlapped = Cast<AEntityClass>(OverlapResult.GetActor()))
				{
					AActor* Actor = InSkillContext.EntityOwner.Get();
					if (!Actor)
					{
						UE_LOG(LogTemp, Warning, TEXT("UOnHitApplyEffect::Execute Invalid EntityOwner"));
						return;
					}
					
					InSkillContext.HitOverlapResult.DamageCauser = Actor;
					InSkillContext.HitOverlapResult.ControllerInstigator = Actor->GetInstigatorController();
					
					for (UEffectDataAsset* EffectDataAsset : OnHitEffects)
					{
						int8 StacksOfEffect = EntityOverlapped->GetAmountActiveEffects(EffectDataAsset->GetPrimaryAssetId());
			
						if (StacksOfEffect > 0 && StacksOfEffect < EffectDataAsset->MaxEffectStacks && EffectDataAsset->bAcumulateEffect)
						{
							EntityOverlapped->ApplyEffect(EffectDataAsset->GetPrimaryAssetId());
						} else if (StacksOfEffect == 0)
						{
							UEffectInstance* Instance = EffectDataAsset->GetInstance(Actor, EntityOverlapped);
							EntityOverlapped->ApplyEffect(Instance, InSkillContext.HitOverlapResult);
						}
					}
				}
			}
		}
	} else if (InSkillContext.HitOverlapResult.HitResult.bBlockingHit)
	{
		if (AEntityClass* EntityHitted = Cast<AEntityClass>(InSkillContext.HitOverlapResult.HitResult.GetActor()))
		{
			AActor* Actor = InSkillContext.EntityOwner.Get();
			if (!Actor)
			{
				UE_LOG(LogTemp, Warning, TEXT("UOnHitApplyEffect::Execute Invalid EntityOwner"));
				return;
			}
			
			InSkillContext.HitOverlapResult.DamageCauser = Actor;
			InSkillContext.HitOverlapResult.ControllerInstigator = Actor->GetInstigatorController();
			
			for (UEffectDataAsset* EffectDataAsset : OnHitEffects)
			{
				int8 StacksOfEffect = EntityHitted->GetAmountActiveEffects(EffectDataAsset->GetPrimaryAssetId());
			
				if (StacksOfEffect > 0 && StacksOfEffect < EffectDataAsset->MaxEffectStacks && EffectDataAsset->bAcumulateEffect)
				{
					EntityHitted->ApplyEffect(EffectDataAsset->GetPrimaryAssetId());
				} else if (StacksOfEffect == 0)
				{
					UEffectInstance* Instance = EffectDataAsset->GetInstance(Actor, EntityHitted);
					EntityHitted->ApplyEffect(Instance, InSkillContext.HitOverlapResult);
				}
			}
		}
	}
}
