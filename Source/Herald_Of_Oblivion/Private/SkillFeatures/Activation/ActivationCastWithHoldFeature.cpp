// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Activation/ActivationCastWithHoldFeature.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Core/EntityClass.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "Kismet/KismetMathLibrary.h"

void UActivationCastWithHoldFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UActivationCastWithHoldFeature::StartActivation(FSkillContext& InSkillContext)
{
	Super::StartActivation(InSkillContext);
	
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UActivationCastWithHoldFeature::StartActivation - SkillInstance invalido."));
		return;
	}
	
	const USkillDataAsset* SkillDataAsset = SkillInstance->GetDataAsset();
	
	AEntityClass* EntityOwner = InSkillContext.EntityOwner.Get();
	
	if (!SkillDataAsset || !EntityOwner) return;
	
	if (this->bAura)
	{
		
		if (this->bAura)
		{
			if (UNiagaraSystem* AuraEffect = this->SkeletalMeshAuraEffect.Get())
			{
				UNiagaraComponent* NC = SpawnAuraVFX(AuraEffect, SkillDataAsset, EntityOwner, InSkillContext, EAuraType::SkeletalMesh, EntityOwner->GetMesh(), NAME_None);
				NC->SetFloatParameter(FName("NormalOffset"), this->NormalOffsetAura);
				NC->SetFloatParameter(FName("SpawnRate"), this->SpawnRateAura);
				NC->SetFloatParameter(FName("MaxLifeTime"), this->MaxLifeTimeAura);
				NC->SetFloatParameter(FName("MinLifeTime"), this->MinLifeTimeAura);

				SpawnedNiagaraComponents.Add(NC);
			}
		}
	}
	
	UNiagaraComponent* NiagaraComp;

	if (UNiagaraSystem* CastVFX = CastEffect.Get())
	{
		this->Context = InSkillContext;
		if (bCastOnTarget && !InSkillContext.StartSurfaceNormal.IsZero())
		{
			NiagaraComp = SpawnVFXAtLocation(CastVFX, UKismetMathLibrary::MakeRotFromZ(InSkillContext.StartSurfaceNormal), InSkillContext.StartLocation);
		}
		else
		{
			NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				CastVFX,
				EntityOwner->GetMesh(),
				FName("RightHandSocket"),
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				true);
			/*
			NiagaraComp = SpawnVFXAtLocation(CastVFX, FRotator::ZeroRotator, EntityOwner->GetMesh()->GetSocketLocation(FName("RightHandSocket")));
		*/
		}

		if (!InSkillContext.Direction.IsZero())
			NiagaraComp->SetVectorParameter(FName("Direction"), InSkillContext.Direction);
			
		NiagaraComp->SetFloatParameter(FName("MinCastTime"), MinCastTime);
		NiagaraComp->SetFloatParameter(FName("MaxCastTime"), MaxCastTime);
		SpawnedNiagaraComponents.Add(NiagaraComp);
	}
	
	if (GetWorld()->GetTimerManager().IsTimerActive(this->TimerHandle))
		GetWorld()->GetTimerManager().ClearTimer(this->TimerHandle);
	
	TWeakObjectPtr<UActivationCastWithHoldFeature> WeakThis(this);
	TWeakObjectPtr<USkillInstance> WeakSkillInstance = InSkillContext.SkillInstance;
	GetWorld()->GetTimerManager().SetTimer(
		this->TimerHandle, 
		[WeakThis, WeakSkillInstance]() 
		{
			if (!WeakThis.IsValid() || !WeakSkillInstance.IsValid())
			{
				return;
			}

			WeakThis->CompleteActivation(WeakSkillInstance->CurrentContext);
		},
            MaxCastTime,
		false,
		-1
	);
	
	UE_LOG(LogTemp, Warning, TEXT("CastWithHoldActivation Start OK"));
}

void UActivationCastWithHoldFeature::CompleteActivation(FSkillContext& InSkillContext)
{
	Super::CompleteActivation(InSkillContext);
	if (InSkillContext.bActivated) return;

	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UActivationCastWithHoldFeature::CompleteActivation - SkillInstance invalido."));
		return;
	}
	
   if (InSkillContext.HoldDuration < MinCastTime)
	{
		CleanNiagara();
		return;
	}

	InSkillContext.ChargeRatio = FMath::Clamp(InSkillContext.HoldDuration / MaxCastTime, 0.0f, 1.0f);
	
	this->CleanNiagara();
	
	UE_LOG(LogTemp, Warning, TEXT("CompleteActivation"));
	InSkillContext.bActivated = true;
	InSkillContext.SkillInstance->OnSkillActivateDelegate.Broadcast(InSkillContext);
}

void UActivationCastWithHoldFeature::CleanNiagara()
{
	for (UNiagaraComponent* SpawnedNiagaraComponent : this->SpawnedNiagaraComponents)
	{
		if (IsValid(SpawnedNiagaraComponent))
		{
			SpawnedNiagaraComponent->SetFloatParameter(FName("SpawnRate"), 0.0f);
			
			FTimerHandle Timer;
			
			GetWorld()->GetTimerManager().SetTimer(
				Timer, 
				[SpawnedNiagaraComponent]() 
				{
					SpawnedNiagaraComponent->Deactivate();
				},
					6.0f,
				false,
				-1
			);
		}
	}

}

void UActivationCastWithHoldFeature::OnNiagaraSystemFinished(class UNiagaraComponent* FinishedComponent)
{
	Super::OnNiagaraSystemFinished(FinishedComponent);
}

void UActivationCastWithHoldFeature::OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	Super::OnAuraNiagaraSystemFinished(FinishedComponent);
}

void UActivationCastWithHoldFeature::BeginDestroy()
{
	// Verificamos se o mundo ainda existe e limpamos o timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(this->TimerHandle);
	}

	Super::BeginDestroy();
}
