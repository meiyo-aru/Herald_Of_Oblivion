// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/EntityClass.h"
#include "EnemyClass.generated.h"

class UEnemyDataAsset;

UCLASS()
class HERALD_OF_OBLIVION_API AEnemyClass : public AEntityClass
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// O data asset com os dados estáticos desta entidade
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Properties")
	const TObjectPtr<UEnemyDataAsset> DataAsset;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	float CalculateXPReturn(AEntityClass* Killer);
};
