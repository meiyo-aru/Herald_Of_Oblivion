// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemInstance.generated.h"

class USkillDataAsset;
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
	TObjectPtr<USkillDataAsset> DataAsset;

	// A entidade dona do item atualmente
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AEntityClass> EntityOwner;
	
	// O Actor do item, existe um actor para cada instancia
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AItemActor> ItemActor;
	
	// A quantidade do item
	UPROPERTY(EditAnywhere, Category="Properties")
	int8 Amount;
	
	UItemInstance();
	
	// Inicializa a instancia com os valores
	void Initialize(AEntityClass* InOwner, USkillDataAsset* InDataAsset, int8 InAmount);
	
	// Prepara para mandar para o Pool
	virtual void PrepareForPooling();
	
	// Procura um Actor no Pool e o Retorna
	AItemActor* GetItemActor(FVector Location, FRotator Rotation);
	// Procura um Actor no Pool, o anexa ao EntityOwner e o retorna
	AItemActor* GetItemActorAndAttach(FName SocketToAttach);
};
