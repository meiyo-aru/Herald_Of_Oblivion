// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Activation/ActivationChargeFeature.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Core/EntityClass.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "Core/EquipmentActor.h"
#include "Engine/AssetManager.h"
#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "Kismet/KismetMathLibrary.h"

void UActivationChargeFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UActivationChargeFeature::StartActivation(FSkillContext& InSkillContext)
{

	Super::StartActivation(InSkillContext);
	
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UActivationCastWithHoldFeature::StartActivation - SkillInstance invalido."));
		return;
	}
	
	UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
	if (!AssetManager)
	{
		UE_LOG(LogTemp, Error, TEXT("USkillInstance::CastSkill - AssetManager invalido."));
		return;
	}

	
	USkillDataAsset* SkillDataAsset = AssetManager->Get().GetPrimaryAssetObject<USkillDataAsset>(SkillInstance->GetAssetId());
	
	AEntityClass* EntityOwner = InSkillContext.EntityOwner.Get();
	
	if (!SkillDataAsset || !EntityOwner) return;
	

	if (bAura && bAuraInEntityOwner)
	{
		if (UNiagaraSystem* AuraEffect = this->EntityOwnerAuraEffect.Get())
		{
			UNiagaraComponent* NC = SpawnAuraVFX(AuraEffect, EntityOwner, InSkillContext, EAuraType::SkeletalMesh, EntityOwner->GetMesh(), NAME_None);
			NC->SetFloatParameter(FName("NormalOffset"), this->NormalOffsetAura);
			NC->SetFloatParameter(FName("SpawnRate"), this->SpawnRateAura);
			NC->SetFloatParameter(FName("MaxLifeTime"), this->MaxLifeTimeAura);
			NC->SetFloatParameter(FName("MinLifeTime"), this->MinLifeTimeAura);

			InSkillContext.SpawnedNiagaraComponents.Add(NC);
		}
	}
	
	if (bAura && bAuraInWeapon)
	{
		if (UNiagaraSystem* AuraEffect = this->WeaponAuraEffect.Get())
		{
			if (AEquipmentActor* RightWeapon = EntityOwner->GetEquipmentActor(EEquipmentSlot::RightWeapon))
			{
				UNiagaraComponent* RightWeaponNC = SpawnAuraVFX(AuraEffect, EntityOwner, InSkillContext, EAuraType::StaticMesh, RightWeapon->GetMesh(), NAME_None);
				RightWeaponNC->SetFloatParameter(FName("NormalOffset"), this->NormalOffsetAura);
				RightWeaponNC->SetFloatParameter(FName("SpawnRate"), this->SpawnRateAura);
				RightWeaponNC->SetFloatParameter(FName("MaxLifeTime"), this->MaxLifeTimeAura);
				RightWeaponNC->SetFloatParameter(FName("MinLifeTime"), this->MinLifeTimeAura);
				InSkillContext.SpawnedNiagaraComponents.Add(RightWeaponNC);
			}
			
			if (AEquipmentActor* LeftWeapon = EntityOwner->GetEquipmentActor(EEquipmentSlot::LeftWeapon))
			{
				UNiagaraComponent* LeftWeaponNC = SpawnAuraVFX(AuraEffect, EntityOwner, InSkillContext, EAuraType::StaticMesh, LeftWeapon->GetMesh(), NAME_None);
				LeftWeaponNC->SetFloatParameter(FName("NormalOffset"), this->NormalOffsetAura);
				LeftWeaponNC->SetFloatParameter(FName("SpawnRate"), this->SpawnRateAura);
				LeftWeaponNC->SetFloatParameter(FName("MaxLifeTime"), this->MaxLifeTimeAura);
				LeftWeaponNC->SetFloatParameter(FName("MinLifeTime"), this->MinLifeTimeAura);
				InSkillContext.SpawnedNiagaraComponents.Add(LeftWeaponNC);
			}
			
		}
	}
	
	FRotator SpawnRotation;
	if (bMakeRotFromZ)
		SpawnRotation = !InSkillContext.StartSurfaceNormal.IsZero()
			? UKismetMathLibrary::MakeRotFromZ(InSkillContext.StartSurfaceNormal)
			: FRotator::ZeroRotator;
	else SpawnRotation = FRotator::ZeroRotator;


	if (bChargeOnTarget && !InSkillContext.StartSurfaceNormal.IsZero())
	{
		if (UNiagaraSystem* ActivationFollowingAimVFX = ActivationFollowingAimEffect.Get())
		{
			UNiagaraComponent* NiagaraComp = SpawnVFXAtLocation(ActivationFollowingAimVFX, SpawnRotation, InSkillContext.StartLocation, InSkillContext);
			NiagaraComp->SetVectorParameter(FName("AimLocation"), InSkillContext.StartLocation);
			NiagaraComp->SetBoolParameter(FName("bUseLocalSpace"), false);
			if (bChargeFollowAim)
			{
				TWeakObjectPtr WeakThis(this);
				TWeakObjectPtr NiagaraWeak(NiagaraComp);

				GetWorld()->GetTimerManager().SetTimer(TimerHandleChargeFollowAim, 
					[WeakThis, NiagaraWeak, &InSkillContext]{
						if (UActivationChargeFeature* CastWithHoldFeature = WeakThis.Get())
						{
							FHitResult Hit = CastWithHoldFeature->GetAimTarget(InSkillContext, 0.05f);
							if (Hit.bBlockingHit)
							{
								if (UNiagaraComponent* Nc = NiagaraWeak.Get())
								{
									Nc->SetVectorParameter(FName("AimLocation"), Hit.ImpactPoint);
									
									FRotator SpawnRotation;
									if (CastWithHoldFeature->bMakeRotFromZ)
										SpawnRotation = !Hit.ImpactNormal.IsZero()
											? UKismetMathLibrary::MakeRotFromZ(Hit.ImpactNormal)
											: FRotator::ZeroRotator;
									else SpawnRotation = FRotator::ZeroRotator;
									
									Nc->SetWorldRotation(SpawnRotation);
								}
							}
						}
					},
					RateToFollow,
					true);
			}
			NiagaraComp->SetFloatParameter(FName("MinChargeTime"), MinChargeTime);
			NiagaraComp->SetFloatParameter(FName("MaxChargeTime"), MaxChargeTime);
			InSkillContext.SpawnedNiagaraComponents.Add(NiagaraComp);
		}
	}
	if (bChargeOnRightHand || bChargeOnLeftHand)
	{
		if (UNiagaraSystem* ActivationFX = ActivationEffect.Get())
		{
			if (bChargeOnRightHand)
			{
				if (AEquipmentActor* EquippedWeapon = EntityOwner->GetEquipmentActor(EEquipmentSlot::RightWeapon))
				{
					UE_LOG(LogTemp, Warning, TEXT("ENTROU NO WEAPON: %s"), *EquippedWeapon->GetName());
					UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
						ActivationFX,
						EquippedWeapon->GetMesh(),
						FName("Cast"),
						FVector::ZeroVector,
						FRotator::ZeroRotator,
						EAttachLocation::SnapToTarget,
						true);
					NiagaraComp->SetFloatParameter(FName("MinChargeTime"), MinChargeTime);
					NiagaraComp->SetFloatParameter(FName("MaxChargeTime"), MaxChargeTime);
					InSkillContext.SpawnedNiagaraComponents.Add(NiagaraComp);
				} else
				{
					UE_LOG(LogTemp, Warning, TEXT("ENTROU NO HAND"));
					UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
					ActivationFX,
					EntityOwner->GetMesh(),
					FName("RightHand"),
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget,
					true);
					NiagaraComp->SetFloatParameter(FName("MinChargeTime"), MinChargeTime);
					NiagaraComp->SetFloatParameter(FName("MaxChargeTime"), MaxChargeTime);
					InSkillContext.SpawnedNiagaraComponents.Add(NiagaraComp);
					
				}
			}
			
			if (bChargeOnLeftHand)
			{
				if (AEquipmentActor* EquippedWeapon = EntityOwner->GetEquipmentActor(EEquipmentSlot::LeftWeapon))
				{
					UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
						ActivationFX,
						EquippedWeapon->GetMesh(),
						FName("Cast"),
						FVector::ZeroVector,
						FRotator::ZeroRotator,
						EAttachLocation::SnapToTarget,
						true);
					NiagaraComp->SetFloatParameter(FName("MinChargeTime"), MinChargeTime);
					NiagaraComp->SetFloatParameter(FName("MaxChargeTime"), MaxChargeTime);
					InSkillContext.SpawnedNiagaraComponents.Add(NiagaraComp);
				} else
				{
					UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
					ActivationFX,
					EntityOwner->GetMesh(),
					FName("LeftHand"),
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget,
					true);
					NiagaraComp->SetFloatParameter(FName("MinChargeTime"), MinChargeTime);
					NiagaraComp->SetFloatParameter(FName("MaxChargeTime"), MaxChargeTime);
					InSkillContext.SpawnedNiagaraComponents.Add(NiagaraComp);
				}
				
			}
		}
	}
	
	TWeakObjectPtr WeakThis(this);
	TWeakObjectPtr<USkillInstance> WeakSkillInstance = InSkillContext.SkillInstance;
	GetWorld()->GetTimerManager().SetTimer(
		this->TimerHandle, 
		[WeakThis, WeakSkillInstance, this]() 
		{
			if (!WeakThis.IsValid() || !WeakSkillInstance.IsValid() || !TimerHandle.IsValid())
			{
				return;
			}

			WeakThis->CompleteActivation(WeakSkillInstance->CurrentContext);
		},
            MaxChargeTime,
		false,
		-1
	);
	
	UE_LOG(LogTemp, Warning, TEXT("CastWithHoldActivation Start OK"));
}

void UActivationChargeFeature::CompleteActivation(FSkillContext& InSkillContext)
{
	Super::CompleteActivation(InSkillContext);
	GetWorld()->GetTimerManager().ClearTimer(this->TimerHandleChargeFollowAim);

	if (InSkillContext.bActivated) return;

	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UActivationCastWithHoldFeature::CompleteActivation - SkillInstance invalido."));
		return;
	}
	
   if (InSkillContext.HoldDuration < MinChargeTime)
	{
		CleanNiagara(InSkillContext.SpawnedNiagaraComponents);
		return;
	}

	InSkillContext.ChargeRatio = FMath::Clamp(InSkillContext.HoldDuration / MaxChargeTime, 0.0f, 1.0f);
	
	InSkillContext.bActivated = true;
	InSkillContext.SkillInstance->OnSkillActivateDelegate.Broadcast(InSkillContext);
	this->CleanNiagara(InSkillContext.SpawnedNiagaraComponents);
}

void UActivationChargeFeature::CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>> SpawnedNiagaraComponents)
{
	Super::CleanNiagara(SpawnedNiagaraComponents);
}



void UActivationChargeFeature::BeginDestroy()
{
	// Verificamos se o mundo ainda existe e limpamos o timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(this->TimerHandle);
	}

	Super::BeginDestroy();
}
