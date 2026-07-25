// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "NiagaraSystem.h"
#include "Sound/SoundCue.h"

#include "Core/SkillInstance.h"

void UExecutionFeature::LoadFXSync()
{
	UNiagaraSystem* CachedEffect = ExecutionEffect.LoadSynchronous();
	CachedEffects.Add(FName("ExecutionEffect"), CachedEffect);
	WarmupNiagara(CachedEffect);
	
	CachedSounds.Add(FName("ExecutionSound"), ExecutionSound.LoadSynchronous());
}

void UExecutionFeature::Initialize(USkillInstance* Owner)
{
	if (Owner) 
	{
		// Delegate disparado ao feature de target terminar
		Owner->OnSkillActivateDelegate.AddUObject(this, &UExecutionFeature::Execute);
	}; 
}

void UExecutionFeature::Execute(FSkillContext& InSkillContext)
{
	InSkillContext.SkillStage = ESkillStage::InExecution;
}

void UExecutionFeature::CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents)
{
	Super::CleanNiagara(SpawnedNiagaraComponents);
}

void UExecutionFeature::ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext)
{
}

