// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ItemActor.h"


// Sets default values
AItemActor::AItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemActor::PrepareForPooling()
{
	if (IsValid(StaticMeshComponent))
	{
		StaticMeshComponent->SetStaticMesh(nullptr); // Remove a malha pesada, mantém o componente "vazio"
		StaticMeshComponent->SetHiddenInGame(true);
		StaticMeshComponent->SetComponentTickEnabled(false);
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);;
	}
}

