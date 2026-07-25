// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpecializationDataAsset.generated.h"

class UItemDataAsset;
class USkillDataAsset;
class UEquipmentDataAsset;
enum class EEquipmentSlot : uint8;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class HERALD_OF_OBLIVION_API USpecializationDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FPrimaryAssetId GetPrimaryAssetId() const {
		return FPrimaryAssetId("Specialization", GetFName());
	}
	
	// O nome da especializacao
	UPROPERTY(EditAnywhere, Category="Properties")
	FText Name;
	
	// A descrição da especializacao
	UPROPERTY(EditAnywhere, Category="Properties")
	FText Description;
	
	UPROPERTY(EditAnywhere, Category="Properties")
	TArray<TSoftObjectPtr<UEquipmentDataAsset>> InitialEquipments;
	
	UPROPERTY(EditAnywhere, Category="Properties")
	TArray<TSoftObjectPtr<UItemDataAsset>> InitialItems;
	
	UPROPERTY(EditAnywhere, Category="Properties")
	TArray<TSoftObjectPtr<USkillDataAsset>> InitialSkills;
};
