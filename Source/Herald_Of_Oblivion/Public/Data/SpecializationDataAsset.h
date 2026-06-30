// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpecializationDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class HERALD_OF_OBLIVION_API USpecializationDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// O nome da especializacao
	UPROPERTY(EditAnywhere, Category="Properties")
	FText Name;
	
	// A descrição da especializacao
	UPROPERTY(EditAnywhere, Category="Properties")
	FText Description;
};
