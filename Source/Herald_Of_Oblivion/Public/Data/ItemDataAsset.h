// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"

enum class EItemType : uint8;

/**
 * UItemDataAsset
 * DataAsset que guarda todos os dados do item.
 * um novo item pode ser facilmente criado usando um data asset.
 * As instâncias utilizam os dados do DataAsset para trabalhar
 */
UCLASS(Blueprintable, BlueprintType)
class HERALD_OF_OBLIVION_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override{
		return FPrimaryAssetId("Item", GetFName());
	}
	
	// O tipo do item
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	EItemType ItemType;

	// O nome do item
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	FText Name;
	
	// A descrição do item
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	FText Description;
	
	// Define se é consumível
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	bool bConsumable = false;
	
	// A mesh do item
	UPROPERTY(EditDefaultsOnly, Category="Properties", meta = (AssetBundles = "StaticMesh"))
	TSoftObjectPtr<UStaticMesh> StaticMesh;
	
	// O ícone do item
	UPROPERTY(EditDefaultsOnly, Category="Properties", meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> Icon;
};
