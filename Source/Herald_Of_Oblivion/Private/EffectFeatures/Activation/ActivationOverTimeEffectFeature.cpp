// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectFeatures/Activation/ActivationOverTimeEffectFeature.h"
#include "Structs/SkillStructs.h"
#include "Core/EntityClass.h"
#include "Data/EffectDataAsset.h"
#include "EffectFeatures/Execution/ExecutionEffectFeature.h"

void UActivationOverTimeEffectFeature::Init(FHitOverlapResult& HitOverlapResult)
{
	Super::Init(HitOverlapResult);
}

void UActivationOverTimeEffectFeature::Execute(FHitOverlapResult& HitOverlapResult)
{
	Super::Execute(HitOverlapResult);
	
	if (UEffectInstance* Instance = Cast<UEffectInstance>(GetOuter()))
	{
		if (UWorld* World = Instance->OriginatingActor->GetWorld())
		{
			TWeakObjectPtr WeakInstance(Instance);
			TWeakObjectPtr WeakThis(this);
			FHitOverlapResult HitOverlapResultCopy = HitOverlapResult;
			
			World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([WeakThis, WeakInstance, HitOverlapResultCopy]
			{
				if (UActivationOverTimeEffectFeature* StrongThis = WeakThis.Get())
				{
					if (UWorld* World = GEngine->GetWorldFromContextObject(StrongThis, EGetWorldErrorMode::LogAndReturnNull))
					{
						if (StrongThis->TicksCount >= StrongThis->Duration * StrongThis->ActivationsPerSecond)
						{
							if (UEffectInstance* StrongInstance = WeakInstance.Get())
							{
								StrongThis->TicksCount = 0;	
								if (AEntityClass* TargetEnt = StrongInstance->TargetEntity.Get())
								{
									TargetEnt->RemoveEffect(StrongInstance->DataAsset->GetPrimaryAssetId());
								}
								StrongInstance->SaveToPool();
								World->GetTimerManager().ClearTimer(StrongThis->TimerHandle);
							}
							return;
						}
						StrongThis->TicksCount++;
						
						if (UEffectInstance* StrongInstance = WeakInstance.Get())
						{
							if (IsValid(StrongInstance->ExecutionFeature))
								StrongInstance->ExecutionFeature->Execute(HitOverlapResultCopy);
							
							if (StrongInstance->Reset)
							{
								StrongThis->TicksCount = 0;
								StrongInstance->Reset = false;
							}
						}
					}
				}
			}), (1.0f / ActivationsPerSecond), true);
		}
	}
}
