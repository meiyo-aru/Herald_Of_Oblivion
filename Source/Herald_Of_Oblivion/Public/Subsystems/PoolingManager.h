// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PoolingManager.generated.h"
class USpecializationDataAsset;
class AItemActor;
class UObject;
class AActor;

USTRUCT()
struct FObjectsStruct
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<TObjectPtr<UObject>> Objects;	
	FObjectsStruct();
	explicit FObjectsStruct(const TArray<TObjectPtr<UObject>>& InObjects);
};
USTRUCT()
struct FActorsStruct
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<TObjectPtr<AActor>> Actors;	
	FActorsStruct();
	explicit FActorsStruct(const TArray<TObjectPtr<AActor>>& InActors);
};

/**
 * 
 */
UCLASS()
class HERALD_OF_OBLIVION_API UPoolingManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	// Pool de AActors disponíveis
	UPROPERTY()
	TMap<TSubclassOf<AActor>, FActorsStruct> ActorsPool;
	
	// Actors ativos no momento, isto é, fora do pool
	UPROPERTY()
	TSet<TObjectPtr<AActor>> ActiveActors;
	
	// Pool de UObject
	UPROPERTY()
	TMap<TSubclassOf<UObject>, FObjectsStruct> ObjectsPool;
	
	// Objects ativos no momento, isto é, fora do pool
	UPROPERTY()
	TSet<TObjectPtr<UObject>> ActiveObjects;
	
	// Retorna um Objeto da classe especificada
	AActor* GetActorFromPool(const TSubclassOf<AActor> Class);
	UObject* GetObjectFromPool(const TSubclassOf<UObject> Class);
	
	// Recebe um TArray e o insere no pool, caso não exista
	void PopulateActorPool(TSubclassOf<AActor> Class, int8 Amount);
	void PopulateObjectPool(TSubclassOf<UObject> Class, int8 Amount);
	void SaveObjectInPool(TSubclassOf<UObject> ObjectClass, UObject* ObjectToSave);
	void SaveActorInPool(TSubclassOf<AActor> ActorClass, AActor* ActorToSave);
};
