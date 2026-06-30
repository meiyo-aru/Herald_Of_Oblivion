// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/ItemStructs.h"
#include "EquipmentInstance.generated.h"

class UEquipmentDataAsset;
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
	UPROPERTY(EditAnywhere, Category="Properties", meta=(AllowedTypes="Equipment"))
	FPrimaryAssetId AssetId;

	// A entidade dona do item atualmente
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AEntityClass> EntityOwner;

	// A raridade do equipamento
	UPROPERTY(VisibleAnywhere, Category="Properties")
	FItemRarityStruct Rarity;
	
	// O nível do equipamento
	UPROPERTY(EditAnywhere, Category="Properties")
	int8 Level;
	
	// O Actor do Equipamento, existe um actor para cada instancia
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TObjectPtr<AEquipmentActor> EquipmentActor;

	// Feature de OnHit
	UPROPERTY(EditAnywhere, Category = "Features")
	TArray<TObjectPtr<UOnHitFeature>> OnHitFeature;
	
	UEquipmentInstance();
	
	void Initialize(FItemRarityStruct InRarity, int8 InLevel, AEntityClass* InOwner, FPrimaryAssetId InDataAsset, TArray<TObjectPtr<
	                UOnHitFeature>> InOnHitFeatures);
	
	AEquipmentActor* GetEquipmentActor(FVector Location, FRotator Rotation);
	AEquipmentActor* GetEquipmentActorAndAttach(FName SocketToAttach);
};
