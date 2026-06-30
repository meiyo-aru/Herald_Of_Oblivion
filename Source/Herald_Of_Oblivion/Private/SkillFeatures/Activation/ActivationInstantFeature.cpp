// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Activation/ActivationInstantFeature.h"

#include "Core/EntityClass.h"
#include "Core/SkillInstance.h"
#include "Data/SkillDataAsset.h"
#include "Engine/AssetManager.h"
#include "Structs/SkillStructs.h"

void UActivationInstantFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UActivationInstantFeature::StartActivation(FSkillContext& InSkillContext)
{
	Super::StartActivation(InSkillContext);

	const FHitResult HitCursor = GetCursorLocation(InSkillContext);
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
		
		if (AEntityClass* EntityOwner = InSkillContext.EntityOwner.Get())
		{
			float Cast = SkillDataAsset->CastTime * ((SkillDataAsset->MultiplierReduceCastTime * EntityOwner->GetSimbolicAttribute(SkillDataAsset->ReduceCastTimeAttribute).GetAttributeValue())/100);
			
			UE_LOG(LogTemp, Log, TEXT("CastTime: %f"), Cast);
			
			GetWorld()->GetTimerManager().SetTimer(CastTimerHandle, [&InSkillContext]
			{
				InSkillContext.bActivated = true;
				InSkillContext.SkillInstance->OnSkillActivateDelegate.Broadcast(InSkillContext);
			}, Cast, false);
		}
	}
	
}

