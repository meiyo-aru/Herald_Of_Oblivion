// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

UCLASS()
class HERALD_OF_OBLIVION_API AItemActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// O StaticMesh
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TObjectPtr<UStaticMesh>	StaticMesh;
	
	// O componente de StaticMesh do Actor
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	// Retorna o Mesh
	TObjectPtr<UStaticMeshComponent> GetMesh() const {return StaticMeshComponent;}

	// Prepara para enviar para o Pool
	virtual void PrepareForPooling();
};
