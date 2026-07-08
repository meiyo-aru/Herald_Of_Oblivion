// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/EquipmentDataAsset.h"

#include "Core/EntityClass.h"
#include "Core/EquipmentInstance.h"
#include "Subsystems/GameInstanceClass.h"
#include "Engine/AssetManager.h"
#include "SkillFeatures/OnHit/OnHitFeature.h"
#include "Subsystems/PoolingManager.h"

UEquipmentDataAsset::UEquipmentDataAsset() 
{
}

UEquipmentInstance* UEquipmentDataAsset::GetInstance(FItemRarityStruct InRarity,
                                                     int8 InLevel, float InAmount, AEntityClass* InEntityOwner)
{
	if (UWorld* World = InEntityOwner->GetWorld())
	{
		UPoolingManager* PoolingManager = Cast<UPoolingManager>(World->GetGameInstance()->GetSubsystem<UPoolingManager>());
		
		if (UEquipmentInstance* Instance = Cast<UEquipmentInstance>(PoolingManager->GetObjectFromPool(UEquipmentInstance::StaticClass())))
		{
			TArray<UOnHitFeature*> Features;
			
			for (UOnHitFeature* Feature : OnHitFeatures)
			{
				Features.Add(DuplicateObject<UOnHitFeature>(Feature, Instance));
			}
			Instance->Initialize(InRarity, InLevel, this, Features, InEntityOwner);
			return Instance;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("UEquipmentDataAsset::GetInstance Error"));
	return nullptr;
}

FName UEquipmentDataAsset::GetEquipmentSocketName()
{
	// 1. Pegamos o texto puro (FString) do Enum usando a reflexão
	FString EquipmentSlotString = StaticEnum<EEquipmentSlot>()->GetNameStringByValue((int64)EquipmentSlot);
	FString WeaponTypeString = StaticEnum<EWeaponType>()->GetNameStringByValue((int64)WeaponType);
	return FName(*(EquipmentSlotString + WeaponTypeString));
}

