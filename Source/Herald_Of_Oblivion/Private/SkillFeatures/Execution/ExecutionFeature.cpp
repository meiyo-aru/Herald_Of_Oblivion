// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "NiagaraSystem.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Character/PlayerClass.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/EntityAnimInstance.h"
#include "Sound/SoundCue.h"

#include "Core/SkillInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UExecutionFeature::LoadFXSync()
{
	UNiagaraSystem* CachedEffect = ExecutionEffect.LoadSynchronous();
	CachedEffects.Add(FName("ExecutionEffect"), CachedEffect);
	WarmupNiagara(CachedEffect);
	
	CachedSounds.Add(FName("ExecutionSound"), ExecutionSound.LoadSynchronous());
	CachedAnimations.Add(FName("ExecutionAnimation"), ExecutionAnimation.LoadSynchronous());
}

void UExecutionFeature::Initialize(USkillInstance* Owner)
{
	if (Owner) 
	{
		// Delegate disparado ao feature de target terminar
		Owner->OnSkillActivateDelegate.AddUObject(this, &UExecutionFeature::PrimaryExecute);
	}; 
}

void UExecutionFeature::PrimaryExecute(FSkillContext& InSkillContext)
{
	InSkillContext.SkillStage = ESkillStage::InExecution;
	

	
	if (AEntityClass* EntityOwner = Cast<AEntityClass>(InSkillContext.EntityOwner.Get()))
	{
		if (bRotateManually)
		{
			if (APlayerClass* Player = Cast<APlayerClass>(EntityOwner))
			{
				Player->bIsManuallyRotating = true;
				
				FVector Direction = InSkillContext.EndLocation - EntityOwner->GetActorLocation();
				Direction.Z = 0.f;
				Direction.Normalize();
			
				Player->TargetRotatorToManuallyRotating = Direction.Rotation();
			}
		}
		
		if (UEntityAnimInstance* AnimInstance = Cast<UEntityAnimInstance>(EntityOwner->GetMesh()->GetAnimInstance()))
		{
			if (TObjectPtr<UAnimSequenceBase>* Anim = CachedAnimations.Find(FName("ExecutionAnimation")))
			{
				SkillContext = &InSkillContext;
				
				if (PrimaryExecutionAnimSlotName == FName("FullBodySlot"))
					AnimInstance->AnimationMontageBlendWeight = 0.0f;	
				else if (PrimaryExecutionAnimSlotName == FName("UpperBodySlot"))
					AnimInstance->AnimationMontageBlendWeight = 1.0f;	
				
				AnimInstance->PlaySlotAnimationAsDynamicMontage(*Anim, PrimaryExecutionAnimSlotName, PrimaryExecutionAnimBlendInTime, PrimaryExecutionAnimBlendOutTime, 1, 1);
				
				EntityOwner->OnAnimationNotify.AddUObject(this, &UExecutionFeature::OnPlayMontageNotifyBegin);
			}	
		}
	}
}

void UExecutionFeature::FinallyExecute(FSkillContext& InSkillContext)
{
	if (AEntityClass* EntityOwner = Cast<AEntityClass>(InSkillContext.EntityOwner.Get()))
		EntityOwner->OnAnimationNotify.RemoveAll(this);
}

void UExecutionFeature::OnPlayMontageNotifyBegin(FName NotifyName)
{
	if (NotifyName == FName("Finally Execute"))
	{
		if (SkillContext)
			this->FinallyExecute(*SkillContext);
	} 
}

void UExecutionFeature::CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents)
{
	Super::CleanNiagara(SpawnedNiagaraComponents);
}

void UExecutionFeature::ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& InSkillContext)
{
}

