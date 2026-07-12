// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EquipmentInstance.h"

#include "Subsystems/GameInstanceClass.h"
#include "Data/EquipmentDataAsset.h"
#include "Subsystems/PoolingManager.h"

UEquipmentInstance::UEquipmentInstance() {}


void UEquipmentInstance::Initialize(FItemRarityStruct InRarity, int8 InLevel, UEquipmentDataAsset* InDataAsset, const TArray<UOnHitFeature*>& InOnHitFeatures, AEntityClass* InOwner)
{
	EntityOwner = InOwner;
	DataAsset = InDataAsset;
	Rarity = InRarity;
	Level = InLevel;
	OnHitFeature = InOnHitFeatures;
}

AEquipmentActor* UEquipmentInstance::GetEquipmentActor(FVector Location, FRotator Rotation)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UPoolingManager* PoolingManager = Cast<UPoolingManager>(World->GetGameInstance()->GetSubsystem<UPoolingManager>()))
			if (AEquipmentActor* Actor = Cast<AEquipmentActor>(PoolingManager->GetActorFromPool(AEquipmentActor::StaticClass())))
			{
				Actor->SetActorLocation(Location);
				Actor->SetActorRotation(Rotation);
				EquipmentActor = Actor;
				return Actor;
			}
	}
	UE_LOG(LogTemp, Error, TEXT("UEquipmentInstance::GetEquipmentActor - AEquipmentActor inválido."))
	return nullptr;
}

AEquipmentActor* UEquipmentInstance::GetEquipmentActorAndAttach()
{	
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UPoolingManager* PoolingManager = Cast<UPoolingManager>(World->GetGameInstance()->GetSubsystem<UPoolingManager>()))
			if (AEquipmentActor* Actor = Cast<AEquipmentActor>(PoolingManager->GetActorFromPool(AEquipmentActor::StaticClass())))
			{
				if (AEntityClass* Owner = EntityOwner.Get())
				{
					Actor->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, DataAsset->GetEquipmentSocketName());
				}
				EquipmentActor = Actor;
				return Actor;
			}
	}
	UE_LOG(LogTemp, Error, TEXT("UEquipmentInstance::GetEquipmentActorAndAttach - AEquipmentActor inválido."))
	return nullptr;
}