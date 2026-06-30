// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemActor.h"
#include "GameFramework/Actor.h"
#include "EquipmentActor.generated.h"

class UOnHitFeature;

UCLASS()
class HERALD_OF_OBLIVION_API AEquipmentActor : public AItemActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEquipmentActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void PrepareForPooling() override;
};
