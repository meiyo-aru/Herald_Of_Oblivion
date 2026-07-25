// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Activation/ActivationFeature.h"

#include "NiagaraComponent.h"
#include "Character/PlayerClass.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSequenceBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/EquipmentActor.h"
#include "Core/EquipmentInstance.h"
#include "Engine/Engine.h"

void UActivationFeature::LoadFXSync()
{
	UNiagaraSystem* CachedEffect = ActivationEffect.LoadSynchronous();
	CachedEffects.Add(FName("ActivationEffect"), CachedEffect);
	WarmupNiagara(CachedEffect);
	
	USoundCue* CachedSound = ActivationSound.LoadSynchronous();
	CachedSounds.Add(FName("ActivationSound"), CachedSound);
	
	CachedEffect = CastEffect.LoadSynchronous();
	CachedEffects.Add(FName("CastEffect"), CachedEffect);
	WarmupNiagara(CachedEffect);
	
	CachedSound = CastSound.LoadSynchronous();
	CachedSounds.Add(FName("CastSound"), CachedSound);
	
	UAnimSequenceBase* CachedAnimation = CastAnimation.LoadSynchronous();
	CachedAnimations.Add(FName("CastAnimation"), CachedAnimation);
}

void UActivationFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("UActivationFeature::Initialize - Owner (USkillInstance) invalido."));
		return;
	}
	
	Owner->OnSkillCastDelegate.AddUObject(this, &UActivationFeature::StartActivation);
	Owner->OnSkillReleasedDelegate.AddUObject(this, &UActivationFeature::CompleteActivation);
}

void UActivationFeature::CastOnHands(FSkillContext& InSkillContext, USkillInstance* SkillInstance, AEntityClass* EntityOwner, TObjectPtr<UNiagaraSystem>* FX, EEquipmentSlot Hand)
{
	if (AEquipmentActor* EquipmentActorPtr = EntityOwner->GetEquipmentActor(Hand))
	{
		if (UEquipmentInstance* EquipmentInstance = EquipmentActorPtr->GetEquipmentInstanceOwner().Get())
		{
			if (SkillInstance->DataAsset->bWeaponIsNecessary)
			{
				if (SkillInstance->DataAsset->WeaponType == EquipmentInstance->GetEquipmentDataAsset()->WeaponType)
				{
					UNiagaraComponent* NC = SpawnVFXAtLocation(*FX, EquipmentActorPtr->GetMesh()->GetSocketRotation("Cast"),  EquipmentActorPtr->GetMesh()->GetSocketLocation("Cast"), InSkillContext);
					if (bCastOnHandsFollowOwner)
						NC->AttachToComponent(EquipmentActorPtr->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Cast"));
					InSkillContext.SpawnedNiagaraComponents.Add(NC);
				}
			} else
			{
				UNiagaraComponent* NC = SpawnVFXAtLocation(*FX, EquipmentActorPtr->GetMesh()->GetSocketRotation("Cast"),  EquipmentActorPtr->GetMesh()->GetSocketLocation("Cast"), InSkillContext);
				if (bCastOnHandsFollowOwner)
					NC->AttachToComponent(EquipmentActorPtr->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Cast"));
				InSkillContext.SpawnedNiagaraComponents.Add(NC);
			}
		}
	} else
	{
		if (!SkillInstance->DataAsset->bWeaponIsNecessary)
		{
			switch (Hand)
			{
				case EEquipmentSlot::RightWeapon:
					{
						UNiagaraComponent* NC = SpawnVFXAtLocation(*FX, EntityOwner->GetMesh()->GetSocketRotation("RightHand"), EntityOwner->GetMesh()->GetSocketLocation("RightHand"), InSkillContext);
						if (bCastOnHandsFollowOwner)
							NC->AttachToComponent(EntityOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RightHand"));
						InSkillContext.SpawnedNiagaraComponents.Add(NC);
						break;
					}
				case EEquipmentSlot::LeftWeapon:
					{
						UNiagaraComponent* NC = SpawnVFXAtLocation(*FX, EntityOwner->GetMesh()->GetSocketRotation("LeftHand"), EntityOwner->GetMesh()->GetSocketLocation("LeftHand"), InSkillContext);
						if (bCastOnHandsFollowOwner)
							NC->AttachToComponent(EntityOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LeftHand"));
						InSkillContext.SpawnedNiagaraComponents.Add(NC);
						break;
					}			
				default:
					break;
			}
		}
	}
}

void UActivationFeature::StartActivation(FSkillContext& InSkillContext)
{
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UActivationCastWithHoldFeature::StartActivation - SkillInstance invalido."));
		return;
	}

	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(this->CleanNiagaraTimerHandle);
		
		if (SkillInstance->bIsCasting && !SkillInstance->bInCooldown)
		{
			if (AActor* ActorOwner = InSkillContext.EntityOwner.Get())
			{
				if (AEntityClass* EntityOwner = Cast<AEntityClass>(ActorOwner))
				{
					TimerManager.ClearTimer(this->CastTimerHandle);
					
					if (EntityOwner)
					{
						if (UAnimInstance* AnimInstance = EntityOwner->GetMesh()->GetAnimInstance())
						{
							if (TObjectPtr<UAnimSequenceBase>* Anim = CachedAnimations.Find(FName("CastAnimation")))
							{
								AnimInstance->PlaySlotAnimationAsDynamicMontage(*Anim, FName("UpperBody"), 0.5, 0.5, 1, 10);
								
								if (FAttribute* Attribute = EntityOwner->GetSimbolicAttribute(SkillInstance->DataAsset->ReduceCastTimeAttribute))
								{
									float MaxCastTime = SkillInstance->DataAsset->MaxCastTime;
									float ReducedCastTime = MaxCastTime * ((Attribute->Value * SkillInstance->DataAsset->MultiplierReduceCastTime)/100);
									float CastTime = MaxCastTime - ReducedCastTime;
									
									if (TObjectPtr<UNiagaraSystem>* FX = CachedEffects.Find(FName("CastEffect")))
									{
										if (bCastOnForward)
										{
											UNiagaraComponent* NC = SpawnVFXAtLocation(*FX, FRotator::ZeroRotator, EntityOwner->GetMesh()->GetSocketLocation("Forward"), InSkillContext);
											if (bCastOnForwardFollowOwner)
												NC->AttachToComponent(EntityOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Forward"));
											InSkillContext.SpawnedNiagaraComponents.Add(NC);
										} 
										if (bCastOnRightHand)
											CastOnHands(InSkillContext, SkillInstance, EntityOwner, FX, EEquipmentSlot::RightWeapon);
										if (bCastOnLeftHand)
											CastOnHands(InSkillContext, SkillInstance, EntityOwner, FX, EEquipmentSlot::LeftWeapon);
									
									}
									
									TWeakObjectPtr WeakAnimInstance(AnimInstance);
									TWeakObjectPtr WeakThis(this);
									TWeakObjectPtr WeakSkillInstance(SkillInstance);
									
									TimerManager.SetTimer(CastTimerHandle, FTimerDelegate::CreateLambda([WeakAnimInstance, WeakThis, &InSkillContext, WeakSkillInstance]
									{
										if (UActivationFeature* StrongThis = WeakThis.Get())
										{
											if (UAnimInstance* StrongAnimInstance = WeakAnimInstance.Get())
											{
												StrongAnimInstance->StopAllMontages(1.0f);
												StrongThis->CleanNiagara(InSkillContext.SpawnedNiagaraComponents);
												if (USkillInstance* StrongSkillInstance = WeakSkillInstance.Get())
													StrongSkillInstance->bIsCasting = false;
												StrongThis->StartActivation(InSkillContext);
											}
										}
									}), CastTime, false);
									return;
								}
							}
						}
					}
				}
			}
		}
		
		// Captura as informacoes do cursor do mouse, como a localizacao e o SurfaceNormal
		FHitResult HitCursor = GetAimTarget(InSkillContext, AimRadius);
		if (AActor* HittedActor = HitCursor.GetActor())
		{
			if (AEntityClass* Entity = Cast<AEntityClass>(HittedActor))
			{
				InSkillContext.EntityOnStartLocation = Entity;
			} else
			{
				InSkillContext.EntityOnEndLocation = nullptr;
			}
		}
		
		InSkillContext.StartSurfaceNormal = HitCursor.ImpactNormal;
		InSkillContext.StartLocation = HitCursor.ImpactPoint;
		
		InSkillContext.InitChargeTime = World->GetTimeSeconds();
	}
}

void UActivationFeature::CompleteActivation(FSkillContext& InSkillContext)
{
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UActivationCastWithHoldFeature::StartActivation - SkillInstance invalido."));
		return;
	}
	
	if (!SkillInstance->bIsCasting && !SkillInstance->bInCooldown)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
		{
			FTimerManager& TimerManager = World->GetTimerManager();
			TimerManager.ClearTimer(this->CastTimerHandle);
			
			// Captura as informacoes do cursor do mouse, como a localizacao e o SurfaceNormal
			FHitResult HitCursor = GetAimTarget(InSkillContext, AimRadius);
			if (AActor* HittedActor = HitCursor.GetActor())
			{
				if (AEntityClass* Entity = Cast<AEntityClass>(HittedActor))
				{
					InSkillContext.EntityOnEndLocation = Entity;
				} else
				{
					InSkillContext.EntityOnEndLocation = nullptr;
				}
			}
			
			InSkillContext.EndSurfaceNormal = HitCursor.ImpactNormal;
			InSkillContext.EndLocation = HitCursor.ImpactPoint;
			
			InSkillContext.Direction = (InSkillContext.EndLocation - InSkillContext.StartLocation).GetSafeNormal();
			InSkillContext.EndChargeTime = World->GetTimeSeconds();
			InSkillContext.HoldDuration = InSkillContext.EndChargeTime - InSkillContext.InitChargeTime;
		}
	}
}




FHitResult UActivationFeature::GetAimTarget(FSkillContext& InContext, float InAimRadius) const
{
	FHitResult HitResult;
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (APlayerClass* Char = Cast<APlayerClass>(InContext.EntityOwner.Get()))
		{
			FVector Start = Char->GetCameraComponent()->GetComponentLocation();
			FVector End = Start + Char->GetCameraComponent()->GetForwardVector() * 50000;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(Char);
			
			World->SweepSingleByChannel(HitResult, Start, End, FQuat::MakeFromRotator(FRotator::ZeroRotator),ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(InAimRadius), CollisionParams);
		}
	}
	return HitResult;
}

FHitResult UActivationFeature::GetCursorLocation(FSkillContext& InContext) const
{
	FHitResult HitCursor;
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		APlayerController* PC = Cast<APlayerController>(World->GetFirstPlayerController());
		
		
		// Captura a posição do mouse no mundo
		if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitCursor))
		{
			HitCursor.Location.Z += 10;
		}
	}
	return HitCursor;
}

void UActivationFeature::CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents)
{
	Super::CleanNiagara(SpawnedNiagaraComponents);
}
