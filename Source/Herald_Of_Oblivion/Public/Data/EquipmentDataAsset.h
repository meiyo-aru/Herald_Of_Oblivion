// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "Core/EntityClass.h"
#include "Core/EquipmentActor.h"
#include "Engine/DataAsset.h"
#include "Structs/ItemStructs.h"
#include "EquipmentDataAsset.generated.h"

class UEquipmentInstance;
/**
 * UEquipmentDataAsset
 * DataAsset que guarda todos os dados do equipamento.
 * um novo equipamento pode ser facilmente criado usando um data asset.
 * As instâncias utilizam os dados do DataAsset para trabalhar
 */
UCLASS()
class HERALD_OF_OBLIVION_API UEquipmentDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override{
		return FPrimaryAssetId("Equipment", GetFName());
	}

	UEquipmentDataAsset();
	
	// Retorna uma instância devidamente inicializada
	UEquipmentInstance* GetInstance(FItemRarityStruct InRarity, int8 InLevel,
	                                float InAmount, AEntityClass* InEntityOwner = nullptr);
	
	// Retorna o nome do socket que o equipamento deve usar
	FName GetEquipmentSocketName();
	
	// Define se haverá um componente de colisão
	UPROPERTY(EditAnywhere, Category="Collision")
	bool bHaveCollisionComponent = false;
	
	// Tamanho da cápsula de colisão
	UPROPERTY(EditAnywhere, Category="Collision")
	FVector CollisionSize = FVector::ZeroVector; 

	// Tipo do item
	UPROPERTY(EditAnywhere, Category="Properties")
	EEquipmentType EquipmentType = EEquipmentType::None;

	// Tipo de arma, caso seja uma
	UPROPERTY(EditAnywhere, Category="Properties", meta=(EditCondition="EquipmentType == EEquipmentType::Weapon", EditConditionHides))
	EWeaponType WeaponType = EWeaponType::None;

	// Slot usado para equipar o item
	UPROPERTY(EditAnywhere, Category="Properties")
	EEquipmentSlot EquipmentSlot = EEquipmentSlot::None;
	
	// Feature que executa lógica ao atingir ou dar overlap em algo
	UPROPERTY(EditAnywhere, Instanced, Category = "Features")
	TArray<TObjectPtr<UOnHitFeature>> OnHitFeatures;
};
