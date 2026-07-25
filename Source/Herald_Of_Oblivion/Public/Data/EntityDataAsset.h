// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs/EntityStructs.h"
#include "EntityDataAsset.generated.h"

class UAnimationDataAsset;
class UAnimMontage;
class USpecializationDataAsset;
class UAnimInstance;

enum class EEntityRaceEnum : uint8;

/**
 *
 */
UCLASS()
class HERALD_OF_OBLIVION_API UEntityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	FPrimaryAssetId GetPrimaryAssetId() const {
		return FPrimaryAssetId("Entity", GetFName());
	}
	
	UEntityDataAsset() {};
	
	// Nome da entidade
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	FText Name; // Nome da entidade
	
	// Raca da entidade, Ex: Elfo, Humano
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	EEntityRaceEnum Race;
	 
	// AnimationBlueprint da entidade
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TSubclassOf<UAnimInstance> AnimationBlueprint;
};
