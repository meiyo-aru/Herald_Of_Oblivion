// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntityDataAsset.h"
#include "EnemyDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class HERALD_OF_OBLIVION_API UEnemyDataAsset : public UEntityDataAsset
{
	GENERATED_BODY()
public:
	FPrimaryAssetId GetPrimaryAssetId() const {
		return FPrimaryAssetId("Enemy", GetFName());
	}
	// A especialização (classe) da entidade 
	UPROPERTY(EditAnywhere, Category="Properties")
	TObjectPtr<USpecializationDataAsset> Specialization;
	
	// Raridade da entidade
	UPROPERTY(EditAnywhere, Category="Properties")
	FEntityRarityStruct Rarity;
};
