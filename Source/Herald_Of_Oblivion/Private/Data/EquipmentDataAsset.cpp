// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/EquipmentDataAsset.h"

#include "Core/EntityClass.h"
#include "Core/EquipmentInstance.h"

UEquipmentInstance* UEquipmentDataAsset::GetInstance(AEntityClass* InEntityOwner, FItemRarityStruct InRarity,
                                                        int8 InLevel, float InAmount)
{
	// Define uma entidade como o Outer e cria o objeto
	UEquipmentInstance* Instance =  NewObject<UEquipmentInstance>(InEntityOwner);

	Instance->Initialize(InRarity, InLevel, InEntityOwner, GetPrimaryAssetId(), OnHitFeature);
	return Instance;
}
