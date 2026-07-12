// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/PoolingManager.h"

#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Core/EffectInstance.h"
#include "Core/EquipmentActor.h"
#include "Core/EquipmentInstance.h"
#include "Core/ItemInstance.h"
#include "Core/SaveGameClass.h"
#include "Core/SkillActor.h"
#include "Core/SkillInstance.h"
#include "Kismet/GameplayStatics.h"

FNiagaraComponentsStruct::FNiagaraComponentsStruct(const TArray<UNiagaraComponent*>& InNiagaraComponents)
	: NiagaraComponents(InNiagaraComponents)
{}

FObjectsStruct::FObjectsStruct(const TArray<UObject*>& InObjects)
	: Objects(InObjects)
{}

FActorsStruct::FActorsStruct(const TArray<AActor*>& InActors)
	: Actors(InActors)
{}

void UPoolingManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	PopulateObjectPool(UEquipmentInstance::StaticClass(), 20);
	PopulateObjectPool(UItemInstance::StaticClass(), 20);
	PopulateObjectPool(UEffectInstance::StaticClass(), 20);
	
	PopulateActorPool(AEquipmentActor::StaticClass(), 20);
	PopulateActorPool(AItemActor::StaticClass(), 20);
	PopulateActorPool(ASkillActor::StaticClass(), 20);
}

AActor* UPoolingManager::GetActorFromPool(const TSubclassOf<AActor> Class)
{
	if (!Class) return nullptr; 
	
	FActorsStruct* StructActorsPtr = ActorsPool.Find(Class);
	
	if (!StructActorsPtr)
	{
		UE_LOG(LogTemp, Log, TEXT("No Pool registered for this class: %s"), *Class->GetName());
		PopulateActorPool(Class, 5);
		StructActorsPtr = ActorsPool.Find(Class);
	}
	
	FActorsStruct& StructActors = *StructActorsPtr;
	
	if (!StructActors.Actors.IsEmpty())
	{
		AActor* ReleasedActor = StructActors.Actors.Pop();
		ActiveActors.Add(ReleasedActor);
		ReleasedActor->SetActorHiddenInGame(false);
		ReleasedActor->SetActorEnableCollision(true);
		UE_LOG(LogTemp, Log, TEXT("Found Actor %s"), *ReleasedActor->GetName());
		return ReleasedActor;
	} 
	UE_LOG(LogTemp, Log, TEXT("No Actor found"));
	return nullptr;
	
}

UObject* UPoolingManager::GetObjectFromPool(const TSubclassOf<UObject> Class)
{
	if (!Class) return nullptr; 
	
	FObjectsStruct* StructObjectsPtr = ObjectsPool.Find(Class);
	
	if (!StructObjectsPtr)
	{
		UE_LOG(LogTemp, Log, TEXT("No Pool registered for this class: %s \nCreating new objects for the Pool..."), *Class->GetName());
		PopulateObjectPool(Class, 5);
		StructObjectsPtr = ObjectsPool.Find(Class);
	}
	
	FObjectsStruct& StructObjects = *StructObjectsPtr;
	
	if (!StructObjects.Objects.IsEmpty())
	{
			UObject* ReleasedObject = StructObjects.Objects.Pop();
			ActiveObjects.Add(ReleasedObject);
			UE_LOG(LogTemp, Log, TEXT("Found Object %s"), *ReleasedObject->GetName());
			return ReleasedObject;
	} 
	UE_LOG(LogTemp, Log, TEXT("No Object found"));
	return nullptr;
}

void UPoolingManager::PopulateActorPool(const TSubclassOf<AActor> Class, int8 Amount)
{
	if (!Class) return; 
	FActorsStruct* ActorPtr = ActorsPool.Find(Class);
	
	if (!ActorPtr || !ActorPtr->Actors.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("No Actor found in Pool, creating new Actors."));

		TArray<TObjectPtr<AActor>>	Actors;
		// Aloca a memória do Array de uma vez para evitar realocações no loop
		Actors.Reserve(Amount);
		// Loop para popular o pool com Actor
		for (int32 i = 0; i < Amount; ++i)
		{
			// Spawna o ator vazio no "limbo" (0,0,0)
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			if (UWorld* World = GetWorld())
			{
				if (AActor* NewActor = World->SpawnActor<AActor>(Class,FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams))
				{
					// Deixa ele totalmente desligado
					NewActor->SetActorHiddenInGame(true);
					NewActor->SetActorEnableCollision(false);
					Actors.Add(NewActor);
				}
			}
		}
		
		if (!Actors.IsEmpty())
		{
			FActorsStruct StructActors = FActorsStruct(Actors);
			ActorsPool.Add(Class, StructActors);
			UE_LOG(LogTemp, Log, TEXT("Actor added in Pool: %s"), *Class->GetName());
		}
	}
}

void UPoolingManager::PopulateObjectPool(const TSubclassOf<UObject> Class, int8 Amount)
{
	if (!Class) return; 
		
	FObjectsStruct* ObjectsStructPtr = ObjectsPool.Find(Class);
	
	if (!ObjectsStructPtr || !ObjectsStructPtr->Objects.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("No Object found in Pool, creating new Objects."));
		
		TArray<TObjectPtr<UObject>>	Objects;
		// Aloca a memória do Array de uma vez para evitar realocações no loop
		Objects.Reserve(Amount);
		
		// Loop para popular o pool com Objects
		for (int32 i = 0; i < Amount; ++i)
		{
			UObject* Instance;
			if (Class == USkillInstance::StaticClass())
			{
				
			} else
			{
				Instance = NewObject<UObject>(this, Class);
			}
			Objects.Add(Instance);
		}
		
		if (!Objects.IsEmpty())
		{
			FObjectsStruct StructObjects = FObjectsStruct(Objects);
			ObjectsPool.Add(Class, StructObjects);
			UE_LOG(LogTemp, Log, TEXT("Object added in Pool: %s"), *Class->GetName());
		}
	}
}

void UPoolingManager::SaveObjectInPool(TSubclassOf<UObject> ObjectClass, UObject* ObjectToSave)
{
	if (FObjectsStruct* ObjectsStructPtr = ObjectsPool.Find(ObjectClass))
	{
		ObjectsStructPtr->Objects.Add(ObjectToSave);
		if (ActiveObjects.Find(ObjectToSave))
			ActiveObjects.Remove(ObjectToSave);
		
		UE_LOG(LogTemp, Warning, TEXT("Save object %s in pool"), ObjectToSave ? *ObjectToSave->GetName() : TEXT(""));
	} else
	{
		FObjectsStruct ObjectsStruct = FObjectsStruct(TArray<UObject*>({ObjectToSave}));
		ObjectsPool.Add(ObjectClass, ObjectsStruct);
		
		if (ActiveObjects.Find(ObjectToSave))
			ActiveObjects.Remove(ObjectToSave);

		UE_LOG(LogTemp, Warning, TEXT("Save Object %s in pool"), ObjectToSave ? *ObjectToSave->GetName() : TEXT(""));
	}
}

void UPoolingManager::SaveActorInPool(TSubclassOf<AActor> ActorClass, AActor* ActorToSave)
{
	if (FActorsStruct* ActorsStructPtr = ActorsPool.Find(ActorClass))
	{
		ActorsStructPtr->Actors.Add(ActorToSave);
		ActiveActors.Remove(ActorToSave);
	} else
	{
		FActorsStruct ActorsStruct = FActorsStruct(TArray<AActor*>({ActorToSave}));
		ActorsPool.Add(ActorClass, ActorsStruct);
		
		if (ActiveActors.Find(ActorToSave))
			ActiveActors.Remove(ActorToSave);

		UE_LOG(LogTemp, Warning, TEXT("Save Actor %s in pool"), ActorToSave ? *ActorToSave->GetName() : TEXT(""));
	}
}

void UPoolingManager::SaveNiagaraInPool(UNiagaraComponent* Niagara)
{
	if (IsValid(Niagara))
	{
		Niagara->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		Niagara->SetRelativeLocation(FVector::ZeroVector);
		Niagara->SetRelativeRotation(FRotator::ZeroRotator);
		Niagara->SetWorldLocation(FVector::ZeroVector);
		Niagara->SetWorldRotation(FRotator::ZeroRotator);
		
		if (FNiagaraComponentsStruct* NiagaraComponentsStructPtr = NiagaraComponentPool.Find(Niagara->GetAsset()->GetFName()))
		{
			NiagaraComponentsStructPtr->NiagaraComponents.Add(Niagara);
			UE_LOG(LogTemp, Warning, TEXT("PPPPPPLLLLL"));
		}
		else
		{
			FNiagaraComponentsStruct NiagaraStruct = FNiagaraComponentsStruct(TArray<UNiagaraComponent*>({Niagara}));
			NiagaraComponentPool.Add(Niagara->GetAsset()->GetFName(), NiagaraStruct);
			UE_LOG(LogTemp, Warning, TEXT("COOOOOOO"));
		}
		
		if (ActiveNiagaraComponents.Find(Niagara))
			ActiveNiagaraComponents.Remove(Niagara);
		
		UE_LOG(LogTemp, Warning, TEXT("Save Niagara %s in pool"), *Niagara->GetAsset()->GetFName().ToString());
	}
}


UNiagaraComponent* UPoolingManager::GetNiagaraComponentFromPool(UNiagaraSystem* NiagaraSystem)
{
	FName NiagaraName = NiagaraSystem->GetFName();
	
	FNiagaraComponentsStruct* StructNiagaraPtr = NiagaraComponentPool.Find(NiagaraName);
	
	UE_LOG(LogTemp, Log, TEXT("Quantidade de %i Niagaras"), StructNiagaraPtr ? StructNiagaraPtr->NiagaraComponents.Num() : 0);
	
	if (!StructNiagaraPtr || (StructNiagaraPtr && StructNiagaraPtr->NiagaraComponents.IsEmpty()))
	{
		UE_LOG(LogTemp, Log, TEXT("No Pool registered for this Niagara: %s \nCreating new Niagara for the Pool..."), *NiagaraName.ToString());
	
		if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
			if (IsValid(NiagaraSystem))
				if (UNiagaraComponent* NC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World,
					NiagaraSystem, 
					FVector::ZeroVector,
					FRotator::ZeroRotator, 
					FVector(1), 
					false,
					false,
					ENCPoolMethod::None))
				{
					NC->SetVisibility(false);
					ActiveNiagaraComponents.Add(NC);
					UE_LOG(LogTemp, Log, TEXT("Created Niagara %s"), *NC->GetName());
					return NC;
				}
	} 
	
	
	if (StructNiagaraPtr && !StructNiagaraPtr->NiagaraComponents.IsEmpty())
	{
		UNiagaraComponent* ReleasedNiagara = StructNiagaraPtr->NiagaraComponents.Pop();
		ActiveNiagaraComponents.Add(ReleasedNiagara);
		UE_LOG(LogTemp, Log, TEXT("Found Niagara %s"), *ReleasedNiagara->GetName());
		
		return ReleasedNiagara;
	} 
	
	UE_LOG(LogTemp, Log, TEXT("No Object %s found"), *NiagaraName.ToString()); 
	return nullptr;
}
