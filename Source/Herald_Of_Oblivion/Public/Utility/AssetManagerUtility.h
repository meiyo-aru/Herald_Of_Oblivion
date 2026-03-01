// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Enumerators/EntityEnums.h"
#include "AssetManagerUtility.generated.h"

/**
 * UAssetManagerUtility
 * Uma classe utilitária que fornece métodos estáticos para gerenciar os assets do projeto
 */

class APlayerClass;

UCLASS()
class HERALD_OF_OBLIVION_API UAssetManagerUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// static TArray<FPrimaryAssetId> GetSkillsBySpecialization(FName TargetSpecialization);
	static TArray<FPrimaryAssetId> GetInitialSkillsBySpecialization(FName TargetSpecialization);
	static TArray<FPrimaryAssetId> GetPlayerSkills(APlayerClass Player);
	static TArray<FPrimaryAssetId> GetPrimaryAssetIdList(FName AssetType);
};
