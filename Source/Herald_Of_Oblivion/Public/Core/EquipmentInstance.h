// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.h"
#include "Structs/ItemStructs.h"
#include "Data/EquipmentDataAsset.h"
#include "EquipmentInstance.generated.h"

class USkillDataAsset;
class AEquipmentActor;
class AEntityClass;
class UOnHitFeature;
class UStaticMesh;

/**
 * UEquipmentInstance
 * Classe que representa uma instância de um equipamento.
 * As instâncias são únicas para cada entidade e não são compartilhadas.
 * Guarda informações exclusivas para a entidade a qual pertence, como o nível.
 */
UCLASS()
class HERALD_OF_OBLIVION_API UEquipmentInstance : public UItemInstance
{
	GENERATED_BODY()

private:
	// Ponteiro para o DataAsset do equipamento
	UPROPERTY(Transient, EditAnywhere, Category="Properties")
	TObjectPtr<UEquipmentDataAsset> EquipmentDataAsset;
	
	// O Actor do Equipamento, existe um actor para cada instancia
	UPROPERTY(Transient, VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AEquipmentActor> EquipmentActor;
	
public:	
	// Define se o equipamento está nas mão, caso true e esteja equipado ele aparece na mão, caso false e esteja equipado 
	// ele aparece na bainha caso seja uma arma
	UPROPERTY(Transient, VisibleAnywhere, Category="Properties", meta=(EditCondition="EquipmentDataAsset->EquipmentType == EEquipmentType::Weapon"))
	bool InHands = false;
	
	// A raridade do equipamento
	UPROPERTY(Transient, VisibleAnywhere, Category="Properties")
	FItemRarityStruct Rarity;
	
	// O nível do equipamento
	UPROPERTY(Transient, EditAnywhere, Category="Properties")
	int8 Level;
	
	// Mapa das animações carregadas da arma
	UPROPERTY(Transient, VisibleAnywhere, Category="Animation")
	TMap<FName, TObjectPtr<UAnimSequence>> CachedAnimations;

	// Feature de OnHit
	UPROPERTY(Transient, EditAnywhere, Category = "Features")
	TArray<TObjectPtr<UOnHitFeature>> OnHitFeature;
	
	UEquipmentInstance();
	
	void Initialize(FItemRarityStruct InRarity, int8 InLevel, UEquipmentDataAsset* InDataAsset, const TArray<
	                UOnHitFeature*>& InOnHitFeatures, AEntityClass* InOwner = nullptr);
	
	// Procura um Actor no Pool e o Retorna
	AEquipmentActor* GetEquipmentActorFromPool(UStaticMesh* InStaticMesh, FVector Location = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator);
	// Procura um Actor no Pool, o anexa ao EntityOwner e o retorna
	AEquipmentActor* GetEquipmentActorFromPoolAndAttach(UStaticMesh* InStaticMesh);
	
	UAnimSequence* GetAnimation(FName AnimationName);
	
	UEquipmentDataAsset* GetEquipmentDataAsset() const { return EquipmentDataAsset; };
	TWeakObjectPtr<AEquipmentActor> GetEquipmentActor() const { return EquipmentActor; };
	UEquipmentDataAsset* GetEquipmentData() const { return EquipmentDataAsset; };
};
