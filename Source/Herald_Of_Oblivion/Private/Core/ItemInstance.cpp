// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ItemInstance.h"

#include "Data/ItemDataAsset.h"
#include "Core/EntityClass.h"


UItemInstance::UItemInstance()
{
}

void UItemInstance::Initialize(AEntityClass* InOwner, FPrimaryAssetId InDataAsset, int8 InAmount)
{
	EntityOwner = InOwner;
	AssetId = InDataAsset;
	Amount = InAmount;
}

void UItemInstance::PrepareForPooling()
{
	// Reseta variaveis
	EntityOwner = nullptr;
	Amount = 0;
}