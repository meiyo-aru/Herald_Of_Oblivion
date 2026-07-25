// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EquipmentInstance.h"

#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystems/GameInstanceClass.h"
#include "Data/EquipmentDataAsset.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Subsystems/PoolingManager.h"

UEquipmentInstance::UEquipmentInstance() {}


void UEquipmentInstance::Initialize(FItemRarityStruct InRarity, int8 InLevel, UEquipmentDataAsset* InDataAsset, const TArray<UOnHitFeature*>& InOnHitFeatures, AEntityClass* InOwner)
{
	EntityOwner = InOwner;
	EquipmentDataAsset = InDataAsset;
	Rarity = InRarity;
	Level = InLevel;
	OnHitFeature = InOnHitFeatures;
}

AEquipmentActor* UEquipmentInstance::GetEquipmentActorFromPool(UStaticMesh* InStaticMesh, FVector Location, FRotator Rotation)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UPoolingManager* PoolingManager = Cast<UPoolingManager>(World->GetGameInstance()->GetSubsystem<UPoolingManager>()))
			if (AEquipmentActor* Actor = Cast<AEquipmentActor>(PoolingManager->GetActorFromPool(AEquipmentActor::StaticClass())))
			{
				Actor->SetActorLocation(Location);
				Actor->SetActorRotation(Rotation);
				EquipmentActor = Actor;
				if (InStaticMesh)
					EquipmentActor->StaticMeshComponent->SetStaticMesh(InStaticMesh);
				EquipmentActor->Initialize(this);
				return Actor;
			}
	}
	UE_LOG(LogTemp, Error, TEXT("UEquipmentInstance::GetEquipmentActor - AEquipmentActor inválido."))
	return nullptr;
}

AEquipmentActor* UEquipmentInstance::GetEquipmentActorFromPoolAndAttach(UStaticMesh* InStaticMesh)
{	
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UPoolingManager* PoolingManager = Cast<UPoolingManager>(World->GetGameInstance()->GetSubsystem<UPoolingManager>()))
			if (AEquipmentActor* Actor = Cast<AEquipmentActor>(PoolingManager->GetActorFromPool(AEquipmentActor::StaticClass())))
			{
				if (AEntityClass* Owner = EntityOwner.Get())
				{
					Actor->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquipmentDataAsset->GetEquipmentSocketName());
				}
				EquipmentActor = Actor;
				if (InStaticMesh)
					EquipmentActor->StaticMeshComponent->SetStaticMesh(InStaticMesh);
				EquipmentActor->Initialize(this);
				return Actor;
			}
	}
	UE_LOG(LogTemp, Error, TEXT("UEquipmentInstance::GetEquipmentActorAndAttach - AEquipmentActor inválido."))
	return nullptr;
}

UAnimSequence* UEquipmentInstance::GetAnimation(FName AnimationName)
{
	if (TObjectPtr<UAnimSequence>* Animation = CachedAnimations.Find(AnimationName))
	{
		return *Animation;
	}
	return nullptr;
}
