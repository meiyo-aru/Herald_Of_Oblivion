// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceClass.generated.h"

class AItemActor;
/**
 * 
 */
UCLASS()
class HERALD_OF_OBLIVION_API UGameInstanceClass : public UGameInstance
{
	GENERATED_BODY()
	public:
	
	virtual void Init() override;

	TMap<TSubclassOf<AActor>, TArray<TObjectPtr<AActor>>> ActorPool;
	TMap<TSubclassOf<UObject>, TArray<TObjectPtr<UObject>>> ObjectPool;
	
	AActor* GetActorFromPool(TSubclassOf<AActor> Class);
	UObject* GetObjectFromPool(TSubclassOf<UObject> Class);
	void PopulateActorPool(TSubclassOf<AActor> Class, TArray<TObjectPtr<AActor>> Actors);
	void PopulateObjectPool(TSubclassOf<UObject> Class, TArray<TObjectPtr<UObject>> Objects);
};
