// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/OnHit/OnHitFeature.h"

#include "Data/SkillDataAsset.h"

void UOnHitFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
	if (Owner) 
	{
		// Delegate disparado ao feature de target terminar
		Owner->OnSkillHitDelegate.AddUObject(this, &UOnHitFeature::Execute);
	}; 
}

void UOnHitFeature::Execute(FSkillContext& InSkillContext, FVector TargetLocation)
{
}
