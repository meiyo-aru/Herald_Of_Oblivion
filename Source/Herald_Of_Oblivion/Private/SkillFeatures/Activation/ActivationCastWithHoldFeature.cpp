// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Activation/ActivationCastWithHoldFeature.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Core/EntityClass.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
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
	
	if (SkillDataAsset->bAuraInCast)
	{
		if (SkillDataAsset->bAuraInStaticMesh)
		{
			if (UNiagaraSystem* StaticMeshAuraEffect = SkillDataAsset->StaticMeshAuraEffect.LoadSynchronous())
			{
				if (SkillDataAsset->bLeftHand)
					SpawnedNiagaraComponents.Add(SpawnAuraVFX(StaticMeshAuraEffect, SkillDataAsset, EntityOwner, InSkillContext, EAuraType::StaticMesh, EntityOwner->GetLeftEquippedWeapon(), FName("LeftSocketHand")));
				if (SkillDataAsset->bRightHand)
					SpawnedNiagaraComponents.Add(SpawnAuraVFX(StaticMeshAuraEffect, SkillDataAsset, EntityOwner, InSkillContext, EAuraType::StaticMesh, EntityOwner->GetRightEquippedWeapon(), FName("RightSocketHand")));
			}
		}
		
		if (SkillDataAsset->bAuraInSkeletalMesh)
		{
			if (UNiagaraSystem* SkeletalMeshAuraEffect = SkillDataAsset->SkeletalMeshAuraEffect.LoadSynchronous())
			{
				SpawnedNiagaraComponents.Add(SpawnAuraVFX(SkeletalMeshAuraEffect, SkillDataAsset, EntityOwner, InSkillContext, EAuraType::SkeletalMesh, EntityOwner->GetMesh(), NAME_None));
			}
		}
	}
	
	UNiagaraComponent* NiagaraComp;
	
	if (UNiagaraSystem* CastVFX = SkillDataAsset->CastEffect.LoadSynchronous())
	{
		UE_LOG(LogTemp, Warning, TEXT("cast on target %d"), SkillDataAsset->bCastOnTarget);
		
		this->Context = InSkillContext;
		if (SkillDataAsset->bCastOnTarget && !InSkillContext.StartSurfaceNormal.IsZero())
		{
			UE_LOG(LogTemp, Warning, TEXT("VFX COM ROTATOR %s"), *InSkillContext.StartSurfaceNormal.ToString());
			NiagaraComp = SpawnVFXAtLocation(CastVFX, UKismetMathLibrary::MakeRotFromZ(InSkillContext.StartSurfaceNormal), InSkillContext.StartLocation);
		}
		else
		{
			NiagaraComp = SpawnVFXAtLocation(CastVFX, FRotator::ZeroRotator, EntityOwner->GetMesh()->GetSocketLocation(FName("RightHandSocket")));
		}
		
		if (!InSkillContext.Direction.IsZero())
			NiagaraComp->SetVectorParameter(FName("Direction"), InSkillContext.Direction);
			
		NiagaraComp->SetFloatParameter(FName("MinCastTime"), SkillDataAsset->MinCastTime);
		NiagaraComp->SetFloatParameter(FName("MaxCastTime"), SkillDataAsset->MaxCastTime);
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
		SkillDataAsset->MaxCastTime,
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
	
	if (InSkillContext.HoldDuration < SkillInstance->GetDataAsset()->MinCastTime)
	{
		CleanNiagara();
		return;
	}
	
	const USkillDataAsset* SkillDataAsset = SkillInstance->GetDataAsset();
	AEntityClass* EntityOwner = InSkillContext.EntityOwner.Get();
	
	InSkillContext.ChargeRatio = FMath::Clamp(InSkillContext.HoldDuration / SkillDataAsset->MaxCastTime, 0.0f, 1.0f);
	
	this->CleanNiagara();
	
	UE_LOG(LogTemp, Warning, TEXT("CompleteActivation"));
	InSkillContext.bActivated = true;
	InSkillContext.SkillInstance->OnSkillActivateDelegate.Broadcast(InSkillContext);
}

void UActivationCastWithHoldFeature::CleanNiagara()
{
	for (UNiagaraComponent* SpawnedNiagaraComponent : this->SpawnedNiagaraComponents)
	{
		if (SpawnedNiagaraComponent)
			SpawnedNiagaraComponent->Deactivate();
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
