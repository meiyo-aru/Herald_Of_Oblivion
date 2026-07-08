// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Activation/ActivationInstantFeature.h"

#include "Core/EntityClass.h"
#include "Core/SkillInstance.h"
#include "Data/SkillDataAsset.h"
#include "Engine/AssetManager.h"
#include "Structs/SkillStructs.h"

void UActivationInstantFeature::LoadFXSync()
{
	Super::LoadFXSync();
}

void UActivationInstantFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UActivationInstantFeature::StartActivation(FSkillContext& InSkillContext)
{
	Super::StartActivation(InSkillContext);

	const FHitResult HitCursor = GetAimTarget(InSkillContext, AimRadius);
	
	InSkillContext.StartSurfaceNormal = HitCursor.ImpactNormal;
	InSkillContext.EndSurfaceNormal = HitCursor.ImpactNormal;
	InSkillContext.StartLocation = HitCursor.ImpactPoint;
	InSkillContext.EndLocation = HitCursor.ImpactPoint;
	
	if (USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get())
	{
		UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
		if (!AssetManager)
		{
			UE_LOG(LogTemp, Error, TEXT("USkillInstance::CastSkill - AssetManager invalido."));
			return;
		}

		const USkillDataAsset* SkillDataAsset = AssetManager->Get().GetPrimaryAssetObject<USkillDataAsset>(SkillInstance->GetAssetId());
		
		if (AEntityClass* EntityOwner = Cast<AEntityClass>(InSkillContext.EntityOwner.Get()))
		{
			if (FAttribute* AttributePtr = EntityOwner->GetSimbolicAttribute(SkillDataAsset->ReduceCastTimeAttribute))
			{
				FAttribute Attribute = *AttributePtr;
				
				float Cast = SkillDataAsset->CastTime * ((SkillDataAsset->MultiplierReduceCastTime * Attribute.GetAttributeValue())/100);
				
				UE_LOG(LogTemp, Log, TEXT("CastTime: %f"), Cast);
				
				if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
				{
					World->GetTimerManager().SetTimer(CastTimerHandle, [&InSkillContext]
					{
						InSkillContext.bActivated = true;
						InSkillContext.SkillInstance->OnSkillActivateDelegate.Broadcast(InSkillContext);
					}, Cast, false);
				};
			}			
		}
	}
	
}

