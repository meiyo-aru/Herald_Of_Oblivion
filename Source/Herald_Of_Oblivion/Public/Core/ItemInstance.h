// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemInstance.generated.h"

class AItemActor;
class UItemDataAsset;
class AEntityClass;
/**
 * UItemInstance
 * Classe que representa uma instância de um item.
 * As instâncias são únicas para cada entidade e não são compartilhadas.
 * Guarda informações exclusivas para a entidade a qual pertence, como o nível.
 */
UCLASS()
class HERALD_OF_OBLIVION_API UItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	// Ponteiro para o USkillDataAsset do item
	UPROPERTY(EditAnywhere, Category="Properties", meta=(AllowedTypes="Item"))
	FPrimaryAssetId AssetId;

	// A entidade dona do item atualmente
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AEntityClass> EntityOwner;
	
	// O Actor do item, existe um actor para cada instancia
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TObjectPtr<AItemActor> ItemActor;
	
	// A quantidade do item
	UPROPERTY(EditAnywhere, Category="Properties")
	int8 Amount;
	
	UItemInstance();
	
	void Initialize(AEntityClass* InOwner, FPrimaryAssetId InDataAsset, int8 InAmount);
	virtual void PrepareForPooling();
	
	AItemActor* CreateItemActorAndSpawn(FVector Location, FRotator Rotation);
	AItemActor* CreateItemActorAndSpawnAttached(FName SocketToAttach);

};
