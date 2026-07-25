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
	UPROPERTY(EditDefaultsOnly, Category="Collision")
	bool bHaveCollisionComponent = false;
	
	// Tamanho da cápsula de colisão
	UPROPERTY(EditDefaultsOnly, Category="Collision")
	FVector CollisionSize = FVector::ZeroVector; 

	// Tipo do item
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	EEquipmentType EquipmentType = EEquipmentType::None;

	// Tipo de arma, caso seja uma
	UPROPERTY(EditDefaultsOnly, Category="Properties", meta=(EditCondition="EquipmentType == EEquipmentType::Weapon", EditConditionHides))
	EWeaponType WeaponType = EWeaponType::None;

	// Slot usado para equipar o item
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	EEquipmentSlot EquipmentSlot = EEquipmentSlot::None;
	
	// Feature que executa lógica ao atingir ou dar overlap em algo
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Features")
	TArray<TObjectPtr<UOnHitFeature>> OnHitFeatures;

	// Animação Idle da arma
	UPROPERTY(EditDefaultsOnly, Category="Animation", meta=(EditCondition="EquipmentType == EEquipmentType::Weapon || EquipmentSlot == EEquipmentSlot::Shield", EditConditionHides))
	TSoftObjectPtr<UAnimSequence> IdleAnimation;
	
	// Animação Walk da arma
	UPROPERTY(EditDefaultsOnly, Category="Animation", meta=(EditCondition="EquipmentType == EEquipmentType::Weapon || EquipmentSlot == EEquipmentSlot::Shield", EditConditionHides))
	TSoftObjectPtr<UAnimSequence> WalkAnimation;
	
	// Animação Run da arma
	UPROPERTY(EditDefaultsOnly, Category="Animation", meta=(EditCondition="EquipmentType == EEquipmentType::Weapon || EquipmentSlot == EEquipmentSlot::Shield", EditConditionHides))
	TSoftObjectPtr<UAnimSequence> RunAnimation;
	
	// Animação de Basic Attack da arma
	UPROPERTY(EditDefaultsOnly, Category="Animation", meta=(EditCondition="EquipmentType == EEquipmentType::Weapon || EquipmentSlot == EEquipmentSlot::Shield", EditConditionHides))
	TSoftObjectPtr<UAnimSequence> BasicAttackAnimation;
};
