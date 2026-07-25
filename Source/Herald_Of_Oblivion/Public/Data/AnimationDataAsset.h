// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AnimationDataAsset.generated.h"


class UAnimMontage;
class USpecializationDataAsset;
class UAnimInstance;

/**
 * 
 */
UCLASS()
class HERALD_OF_OBLIVION_API UAnimationDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FPrimaryAssetId GetPrimaryAssetId() const {
		return FPrimaryAssetId("Animation", GetFName());
	}
	
	UAnimationDataAsset() {};
	
	/*
	// Animação de Turn in Place Right
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TSoftObjectPtr<UAnimMontage> TurnInPlaceRightAnim;
	// Animação de Turn In Place Left
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TSoftObjectPtr<UAnimMontage> TurnInPlaceLeftAnim;
	*/
	 
	// AnimationBlueprint da entidade
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TSubclassOf<UAnimInstance> AnimationBlueprint;
};
