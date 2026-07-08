// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceClass.generated.h"

class APlayerClass;

/**
 * 
 */

class USpecializationDataAsset;

UCLASS()
class HERALD_OF_OBLIVION_API UGameInstanceClass : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	void InitializeNewPlayer(APlayerClass& Player);

	// Um mapa dos equipamentos e habilidades iniciais de cada classe, a Key é o AssetId da classe
	UPROPERTY(EditAnywhere, Category="Properties", meta=(AllowedTypes="Specialization"))
	TMap<FPrimaryAssetId, TSoftObjectPtr<USpecializationDataAsset>> InitialEquipmentsAndSkillsBySpecialization;
};
