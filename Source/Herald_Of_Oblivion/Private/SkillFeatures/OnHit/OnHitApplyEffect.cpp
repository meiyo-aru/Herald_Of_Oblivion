// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/OnHit/OnHitApplyEffect.h"

#include "Core/SkillInstance.h"

void UOnHitApplyEffect::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
	if (Owner) 
	{
		// Delegate disparado ao feature de target terminar
		Owner->OnSkillHitDelegate.AddUObject(this, &UOnHitFeature::Execute);
	}; 
}

void UOnHitApplyEffect::Execute(FSkillContext& InSkillContext)
{
	Super::Execute(InSkillContext);
}
