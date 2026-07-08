// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/ItemStructs.h"
#include "Data/EquipmentDataAsset.h"
#include "EquipmentInstance.generated.h"

class USkillDataAsset;
class AEquipmentActor;
class AEntityClass;
class UOnHitFeature;

/**
 * UEquipmentInstance
 * Classe que representa uma instância de um equipamento.
 * As instâncias são únicas para cada entidade e não são compartilhadas.
 * Guarda informações exclusivas para a entidade a qual pertence, como o nível.
 */
UCLASS()
class HERALD_OF_OBLIVION_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()
	
public:	
	// Ponteiro para o DataAsset do equipamento
	UPROPERTY(EditAnywhere, Category="Properties")
	TObjectPtr<UEquipmentDataAsset> DataAsset;

	// A entidade dona do item atualmente
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AEntityClass> EntityOwner;

	// Define se o equipamento está nas mão, caso true e esteja equipado ele aparece na mão, caso false e esteja equipado 
	// ele aparece na bainha caso seja uma arma
	UPROPERTY(VisibleAnywhere, Category="Properties", meta=(EditCondition="DataAsset->EquipmentType == EEquipmentType::Weapon"))
	bool InHands = false;
	
	// A raridade do equipamento
	UPROPERTY(VisibleAnywhere, Category="Properties")
	FItemRarityStruct Rarity;
	
	// O nível do equipamento
	UPROPERTY(EditAnywhere, Category="Properties")
	int8 Level;
	
	// O Actor do Equipamento, existe um actor para cada instancia
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AEquipmentActor> EquipmentActor;

	// Feature de OnHit
	UPROPERTY(EditAnywhere, Category = "Features")
	TArray<TObjectPtr<UOnHitFeature>> OnHitFeature;
	
	UEquipmentInstance();
	
	void Initialize(FItemRarityStruct InRarity, int8 InLevel, UEquipmentDataAsset* InDataAsset, const TArray<
	                UOnHitFeature*>& InOnHitFeatures, AEntityClass* InOwner = nullptr);
	
	// Procura um Actor no Pool e o Retorna
	AEquipmentActor* GetEquipmentActor(FVector Location = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator);
	// Procura um Actor no Pool, o anexa ao EntityOwner e o retorna
	AEquipmentActor* GetEquipmentActorAndAttach();
};
