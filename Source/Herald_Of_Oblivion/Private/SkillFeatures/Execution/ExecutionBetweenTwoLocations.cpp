// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Execution/ExecutionBetweenTwoLocations.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Character/PlayerClass.h"
#include "Core/EntityClass.h"
#include "Core/EquipmentActor.h"
#include "Core/SkillActor.h"
#include "Core/SkillInstance.h"
#include "Kismet/KismetMathLibrary.h"

void UExecutionBetweenTwoLocations::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UExecutionBetweenTwoLocations::Execute(FSkillContext& InSkillContext)
{
	Super::Execute(InSkillContext);
	
	AEntityClass* EntityOwner = InSkillContext.EntityOwner.Get();
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
	
	if (UNiagaraSystem* VFX = ExecutionEffect.Get())
	{
		const FVector SurfaceNormal = InSkillContext.StartSurfaceNormal;
	
		FRotator SpawnRotation;
		if (bMakeRotFromZ)
			SpawnRotation = !SurfaceNormal.IsZero()
				? UKismetMathLibrary::MakeRotFromZ(SurfaceNormal)
				: FRotator::ZeroRotator;
		else SpawnRotation = FRotator::ZeroRotator;

		
		FTransform SpawnTransform(SpawnRotation, bSpawnOnEndLocation ? InSkillContext.EndLocation : InSkillContext.StartLocation);

		ASkillActor* SkillActor = SpawnSkillActor(EntityOwner, SpawnTransform);
		InSkillContext.SkillActor = SkillActor;
		SkillActor->NiagaraComponent->SetAsset(VFX);
		SkillActor->Initialize(SkillInstance, EntityOwner, InSkillContext);
		
		
		
		if (bUseStartLocation && bUseEndLocation)
		{
			SkillActor->NiagaraComponent->SetVectorParameter(FName("StartLocation"), InSkillContext.StartLocation);
			SkillActor->NiagaraComponent->SetVectorParameter(FName("EndLocation"), InSkillContext.EndLocation);
			SkillActor->NiagaraComponent->SetVectorParameter(FName("Direction"), (InSkillContext.EndLocation - InSkillContext.StartLocation).GetSafeNormal());

		} else if (!bUseStartLocation && !bUseEndLocation)
		{
			if (APlayerClass* Char = Cast<APlayerClass>(EntityOwner))
			{
				FVector SpawnLocation = bSpawnOnEndLocation ? InSkillContext.EndLocation : InSkillContext.StartLocation;
				
				FVector RightVector = Char->GetCameraComponent()->GetRightVector().GetSafeNormal() * MaxLenght;
				
				FVector StartLocation = SpawnLocation - RightVector/2;
				FVector EndLocation = SpawnLocation + RightVector/2;
				
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
					StartLocation = Actor->GetMesh()->GetSocketLocation(FName("CastSocket"));
				else StartLocation = EntityOwner->GetMesh()->GetSocketLocation(FName("RightWeaponSocket"));
			} else
			{
				if (AEquipmentActor* Actor = EntityOwner->GetEquipmentActor(EEquipmentSlot::LeftWeapon))
					StartLocation = Actor->GetMesh()->GetSocketLocation(FName("CastSocket"));
				else StartLocation = EntityOwner->GetMesh()->GetSocketLocation(FName("LeftWeaponSocket"));
			}
			
			SkillActor->NiagaraComponent->SetVectorParameter(FName("StartLocation"), StartLocation);
			SkillActor->NiagaraComponent->SetVectorParameter(FName("EndLocation"), InSkillContext.EndLocation);
			SkillActor->NiagaraComponent->SetVectorParameter(FName("Direction"), (InSkillContext.EndLocation - StartLocation).GetSafeNormal());
		} else
		{
			FVector EndLocation;
			if (bUseRight)
			{
				if (AEquipmentActor* Actor = EntityOwner->GetEquipmentActor(EEquipmentSlot::RightWeapon))
					EndLocation = Actor->GetMesh()->GetSocketLocation(FName("CastSocket"));
				else EndLocation = EntityOwner->GetMesh()->GetSocketLocation(FName("RightWeaponSocket"));					
			} else
			{
				if (AEquipmentActor* Actor = EntityOwner->GetEquipmentActor(EEquipmentSlot::LeftWeapon))
					EndLocation = Actor->GetMesh()->GetSocketLocation(FName("CastSocket"));
				else EndLocation = EntityOwner->GetMesh()->GetSocketLocation(FName("RightWeaponSocket"));					
			}
			SkillActor->NiagaraComponent->SetVectorParameter(FName("StartLocation"), EndLocation);
			SkillActor->NiagaraComponent->SetVectorParameter(FName("EndLocation"), InSkillContext.StartLocation);
			SkillActor->NiagaraComponent->SetVectorParameter(FName("Direction"), (InSkillContext.StartLocation - EndLocation).GetSafeNormal());
		}
	}
}

void UExecutionBetweenTwoLocations::CleanNiagara(
	TArray<TWeakObjectPtr<UNiagaraComponent>> SpawnedNiagaraComponents)
{
	Super::CleanNiagara(SpawnedNiagaraComponents);
}

void UExecutionBetweenTwoLocations::ProccessParticles(const TArray<struct FBasicParticleData>& Data,
	FSkillContext& SkillContext)
{
	Super::ProccessParticles(Data, SkillContext);
}
