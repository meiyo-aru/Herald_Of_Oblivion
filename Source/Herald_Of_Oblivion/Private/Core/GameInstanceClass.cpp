// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameInstanceClass.h"

#include "InterchangeTranslatorBase.h"
#include "Core/EquipmentActor.h"
#include "Core/EquipmentInstance.h"
#include "Engine/AssetManager.h"

void UGameInstanceClass::Init()
{
	Super::Init();
	
	TArray<TObjectPtr<AActor>>	EquipmentActors;
	// Loop para popular o pool com 20 actor de EquipmentActor
	for (int32 i = 0; i < 20; ++i)
	{
		// Spawna o ator vazio no "limbo" (0,0,0)
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
		AEquipmentActor* NewActor = GetWorld()->SpawnActor<AEquipmentActor>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        
		if (NewActor)
		{
			// Deixa ele totalmente desligado
			NewActor->SetActorHiddenInGame(true);
			NewActor->SetActorEnableCollision(false);
			EquipmentActors.Add(NewActor);
		}
	}
	PopulateActorPool(AEquipmentActor::StaticClass(), EquipmentActors);
	
	TArray<TObjectPtr<AActor>>	ItemActors;
	// Loop para popular o pool com 20 actor de EquipmentActor
	for (int32 i = 0; i < 20; ++i)
	{
		// Spawna o ator vazio no "limbo" (0,0,0)
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
		AItemActor* NewActor = GetWorld()->SpawnActor<AItemActor>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        
		if (NewActor)
		{
			// Deixa ele totalmente desligado
			NewActor->SetActorHiddenInGame(true);
			NewActor->SetActorEnableCollision(false);
			ItemActors.Add(NewActor);
		}
	}
	PopulateActorPool(AItemActor::StaticClass(), ItemActors);
	
	UE_LOG(LogTemp, Log, TEXT("UGameInstanceClass::Init"));
	
}

AActor* UGameInstanceClass::GetActorFromPool(TSubclassOf<AActor> Class)
{
	TArray<TObjectPtr<AActor>>* ActorsPtr = ActorPool.Find(Class);
	
	if (!ActorsPtr)
	{
		UE_LOG(LogTemp, Log, TEXT("No Pool registered for this class: %s"), *Class->GetName());
		return nullptr;
	}
	
	TArray<TObjectPtr<AActor>>& Actors = *ActorsPtr;
	
	if (!Actors.IsEmpty())
	{
		if (auto Actor = Actors.Last())
		{
			UE_LOG(LogTemp, Log, TEXT("Found Actor %s"), *Actor->GetName());
			Actor->SetActorHiddenInGame(false);
			Actor->SetActorEnableCollision(true);
			Actors.Remove(Actor);
			return Actor;
		}
	} 
	UE_LOG(LogTemp, Log, TEXT("No Actor found"));
	return nullptr;
	
}

UObject* UGameInstanceClass::GetObjectFromPool(TSubclassOf<UObject> Class)
{
	TArray<TObjectPtr<UObject>>* ObjectsPtr = ObjectPool.Find(Class);
	
	if (!ObjectsPtr)
	{
		UE_LOG(LogTemp, Log, TEXT("No Pool registered for this class: %s"), *Class->GetName());
		return nullptr;
	}
	
	TArray<TObjectPtr<UObject>>& Objects = *ObjectsPtr;
	
	if (!Objects.IsEmpty())
	{
		if (auto Object = Objects.Last())
		{
			UE_LOG(LogTemp, Log, TEXT("Found Object %s"), *Object->GetName());
			Objects.Remove(Object);
			return Object;
		}
	} 
	UE_LOG(LogTemp, Log, TEXT("No Object found"));
	return nullptr;
}

void UGameInstanceClass::PopulateActorPool(TSubclassOf<AActor> Class, TArray<TObjectPtr<AActor>> Actors)
{
	if (!Actors.IsEmpty())
	{
		ActorPool.FindOrAdd(Class, Actors);
		UE_LOG(LogTemp, Log, TEXT("Actor added in Pool: %s"), *Class->GetName());
	}
}

void UGameInstanceClass::PopulateObjectPool(TSubclassOf<UObject> Class, TArray<TObjectPtr<UObject>> Objects)
{
	if (!Objects.IsEmpty())
	{
		ObjectPool.FindOrAdd(Class, Objects);
		UE_LOG(LogTemp, Log, TEXT("Object added in Pool: %s"), *Class->GetName());
	}
}