// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Activation/ActivationInstantFeature.h"

#include "TimerManager.h"
#include "Core/EntityClass.h"
#include "Core/SkillInstance.h"
#include "Data/SkillDataAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
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

	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UActivationCastWithHoldFeature::StartActivation - SkillInstance invalido."));
		return;
	}
	
	if (!SkillInstance->bIsCasting)
	{
		const FHitResult HitCursor = GetAimTarget(InSkillContext, AimRadius);
		
		InSkillContext.StartSurfaceNormal = HitCursor.ImpactNormal;
		InSkillContext.EndSurfaceNormal = HitCursor.ImpactNormal;
		InSkillContext.StartLocation = HitCursor.ImpactPoint;
		InSkillContext.EndLocation = HitCursor.ImpactPoint;
		
		UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
		if (!AssetManager)
		{
			UE_LOG(LogTemp, Error, TEXT("USkillInstance::CastSkill - AssetManager invalido."));
			return;
		}

		const USkillDataAsset* SkillDataAsset = SkillInstance->DataAsset;
		
		InSkillContext.SkillInstance->OnSkillActivateDelegate.Broadcast(InSkillContext);		
	}
}