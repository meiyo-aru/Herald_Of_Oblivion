// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Activation/ActivationInstantFeature.h"

#include "Core/SkillInstance.h"
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
	
	InSkillContext.bActivated = true;
	InSkillContext.SkillInstance->OnSkillActivateDelegate.Broadcast(InSkillContext);
}

void UActivationInstantFeature::OnNiagaraSystemFinished(class UNiagaraComponent* FinishedComponent)
{
	Super::OnNiagaraSystemFinished(FinishedComponent);
}
