// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Execution/ExecutionBetweenTwoLocations.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Character/PlayerClass.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/EntityClass.h"
#include "Core/EquipmentActor.h"
#include "Core/SkillActor.h"
#include "Core/SkillInstance.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Subsystems/PoolingManager.h"

void UExecutionBetweenTwoLocations::LoadFXSync()
{
	Super::LoadFXSync();
}

void UExecutionBetweenTwoLocations::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UExecutionBetweenTwoLocations::PrimaryExecute(FSkillContext& InSkillContext)
{
	Super::PrimaryExecute(InSkillContext);
}

void UExecutionBetweenTwoLocations::FinallyExecute(FSkillContext& InSkillContext)
{
	Super::FinallyExecute(InSkillContext);
	
	AEntityClass* EntityOwner = Cast<AEntityClass>(InSkillContext.EntityOwner.Get());
	if (!IsValid(EntityOwner))
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionEffectBetweenTwoLocations::Execute - EntityOwner invalido."));
		return;
	}
	
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!IsValid(SkillInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionEffectBetweenTwoLocations::Execute - SkillInstance invalido."));
		return;
	}
	
	UNiagaraSystem* VFX;
	TObjectPtr<UNiagaraSystem>* CachedExecutionEffect = CachedEffects.Find(FName("ExecutionEffect"));

	if (CachedExecutionEffect) 
		VFX = *CachedExecutionEffect;
	else 
		VFX = ExecutionEffect.Get();

	if (VFX)
	{
		const FVector SurfaceNormal = InSkillContext.StartSurfaceNormal;
	
		FRotator SpawnRotation;
		if (bMakeRotFromZ)
			SpawnRotation = !SurfaceNormal.IsZero()
				? UKismetMathLibrary::MakeRotFromZ(SurfaceNormal)
				: FRotator::ZeroRotator;
		else SpawnRotation = FRotator::ZeroRotator;
		
		FTransform SpawnTransform(SpawnRotation, bSpawnOnEndLocation ? InSkillContext.EndLocation : InSkillContext.StartLocation);

		if (ASkillActor* SkillActor = SpawnSkillActor(EntityOwner, SpawnTransform))
		{
			InSkillContext.SkillActor = SkillActor;
			
			if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
				if (UPoolingManager* PoolingManager = World->GetGameInstance()->GetSubsystem<UPoolingManager>())
				{
					SkillActor->NiagaraComponent = PoolingManager->GetNiagaraComponentFromPool(VFX);
				}
			
			SkillActor->Initialize(SkillInstance, EntityOwner, InSkillContext);
			
			TArray<FOverlapResult> OverlapResults;
			
			if (bUseStartLocation && bUseEndLocation)
			{
				SkillActor->NiagaraComponent->SetFloatParameter(FName("DistanceBetweenLocations"), FVector::Dist(InSkillContext.StartLocation, InSkillContext.EndLocation));
				SkillActor->NiagaraComponent->SetVectorParameter(FName("StartLocation"), InSkillContext.StartLocation);
				SkillActor->NiagaraComponent->SetVectorParameter(FName("EndLocation"), InSkillContext.EndLocation);
				FVector Direction = (InSkillContext.EndLocation - InSkillContext.StartLocation).GetSafeNormal();
				SkillActor->NiagaraComponent->SetVectorParameter(FName("Direction"), Direction);
				
				float Distance = FVector::Dist(InSkillContext.StartLocation, InSkillContext.EndLocation) + 30;
				
				FVector Size = FVector(Distance / 2, CollisionDepth,CollisionHeight);
				
				FQuat RotationQuat = FQuat::FindBetweenVectors(Size.ForwardVector, Direction);
				FVector Center = InSkillContext.StartLocation + ((Distance / 2) * Direction);
				
				OverlapResults = MakeOverlapBox(Size, RotationQuat, InSkillContext, Center);
			} else if (!bUseStartLocation && !bUseEndLocation)
			{
				if (APlayerClass* Char = Cast<APlayerClass>(EntityOwner))
				{
					FVector SpawnLocation = bSpawnOnEndLocation ? InSkillContext.EndLocation : InSkillContext.StartLocation;
					
					FVector RightVector = Char->GetCameraComponent()->GetRightVector().GetSafeNormal() * MaxLenght;
					
					FVector StartLocation = SpawnLocation - RightVector/2;
					FVector EndLocation = SpawnLocation + RightVector/2;
					
					FVector Direction = (EndLocation - StartLocation).GetSafeNormal();
					
					float Distance = FVector::Dist(StartLocation, EndLocation) + 30;
				
					FVector Size = FVector(Distance / 2, CollisionDepth,CollisionHeight);
				
					FQuat RotationQuat = FQuat::FindBetweenVectors(Size.ForwardVector, Direction);
					FVector Center = StartLocation + ((Distance / 2) * Direction);
				
					OverlapResults = MakeOverlapBox(Size, RotationQuat, InSkillContext, Center);
					
					SkillActor->NiagaraComponent->SetFloatParameter(FName("DistanceBetweenLocations"), FVector::Dist(StartLocation, EndLocation));
					SkillActor->NiagaraComponent->SetVectorParameter(FName("StartLocation"), StartLocation);
					SkillActor->NiagaraComponent->SetVectorParameter(FName("EndLocation"), EndLocation);
					SkillActor->NiagaraComponent->SetVectorParameter(FName("Direction"), (EndLocation - StartLocation).GetSafeNormal());
				}
			} else if (!bUseStartLocation)
			{
				FVector StartLocation;
				if (bUseRight)
				{
					if (AEquipmentActor* Actor = EntityOwner->GetEquipmentActor(EEquipmentSlot::RightWeapon))
						StartLocation = Actor->GetMesh()->GetSocketLocation(FName("Charge"));
					else StartLocation = EntityOwner->GetMesh()->GetSocketLocation(FName("RightHand"));
				} else
				{
					if (AEquipmentActor* Actor = EntityOwner->GetEquipmentActor(EEquipmentSlot::LeftWeapon))
						StartLocation = Actor->GetMesh()->GetSocketLocation(FName("Charge"));
					else StartLocation = EntityOwner->GetMesh()->GetSocketLocation(FName("LeftHand"));
				}
				
				FVector Direction = (InSkillContext.EndLocation - StartLocation).GetSafeNormal();
					
				float Distance = FVector::Dist(StartLocation, InSkillContext.EndLocation) + 30;
				
				FVector Size = FVector(Distance / 2, CollisionDepth,CollisionHeight);
				
				FQuat RotationQuat = FQuat::FindBetweenVectors(Size.ForwardVector, Direction);
				FVector Center = StartLocation + ((Distance / 2) * Direction);
				
				OverlapResults = MakeOverlapBox(Size, RotationQuat, InSkillContext, Center);
				SkillActor->NiagaraComponent->SetFloatParameter(FName("DistanceBetweenLocations"), FVector::Dist(StartLocation, InSkillContext.EndLocation));
				SkillActor->NiagaraComponent->SetVectorParameter(FName("StartLocation"), StartLocation);
				SkillActor->NiagaraComponent->SetVectorParameter(FName("EndLocation"), InSkillContext.EndLocation);
				SkillActor->NiagaraComponent->SetVectorParameter(FName("Direction"), (InSkillContext.EndLocation - StartLocation).GetSafeNormal());
			} else
			{
				FVector EndLocation;
				if (bUseRight)
				{
					if (AEquipmentActor* Actor = EntityOwner->GetEquipmentActor(EEquipmentSlot::RightWeapon))
						EndLocation = Actor->GetMesh()->GetSocketLocation(FName("Charge"));
					else EndLocation = EntityOwner->GetMesh()->GetSocketLocation(FName("RightHand"));					
				} else
				{
					if (AEquipmentActor* Actor = EntityOwner->GetEquipmentActor(EEquipmentSlot::LeftWeapon))
						EndLocation = Actor->GetMesh()->GetSocketLocation(FName("Charge"));
					else EndLocation = EntityOwner->GetMesh()->GetSocketLocation(FName("LeftHand"));					
				}
				
				FVector Direction = (EndLocation - InSkillContext.StartLocation).GetSafeNormal();
					
				float Distance = FVector::Dist(InSkillContext.StartLocation, EndLocation) + 30;
				
				FVector Size = FVector(Distance / 2, CollisionDepth,CollisionHeight);
				
				FQuat RotationQuat = FQuat::FindBetweenVectors(Size.ForwardVector, Direction);
				FVector Center = InSkillContext.StartLocation + ((Distance / 2) * Direction);
				
				OverlapResults = MakeOverlapBox(Size, RotationQuat, InSkillContext, Center);
				SkillActor->NiagaraComponent->SetFloatParameter(FName("DistanceBetweenLocations"), FVector::Dist(InSkillContext.StartLocation, EndLocation));
				SkillActor->NiagaraComponent->SetVectorParameter(FName("StartLocation"), EndLocation);
				SkillActor->NiagaraComponent->SetVectorParameter(FName("EndLocation"), InSkillContext.StartLocation);
				SkillActor->NiagaraComponent->SetVectorParameter(FName("Direction"), (InSkillContext.StartLocation - EndLocation).GetSafeNormal());
			}
			
			if (!OverlapResults.IsEmpty())
			{
				InSkillContext.HitOverlapResult = FHitOverlapResult(OverlapResults);
				InSkillContext.SkillInstance->OnSkillHitDelegate.Broadcast(InSkillContext);
			}
		}
	}
}

void UExecutionBetweenTwoLocations::OnPlayMontageNotifyBegin(FName NotifyName)
{
	Super::OnPlayMontageNotifyBegin(NotifyName);
}

void UExecutionBetweenTwoLocations::CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents)
{
	Super::CleanNiagara(SpawnedNiagaraComponents);
}

void UExecutionBetweenTwoLocations::ProccessParticles(const TArray<struct FBasicParticleData>& Data,
	FSkillContext& InSkillContext)
{
	Super::ProccessParticles(Data, InSkillContext);
}
