// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyClass.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/AnimationDataAsset.h"
#include "Data/EnemyDataAsset.h"


// Sets default values
AEnemyClass::AEnemyClass()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemyClass::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemyClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Calcula o Retorno de XP ao abater a entidade
float AEnemyClass::CalculateXPReturn(AEntityClass* Killer)
{
	// Diferença de nivel entre o assassino e a entidade morta
	float LevelDifference = this->Level  - Killer->GetLevel();
	
	float LevelDifferenceModifier = 1.0f;
	
	if (LevelDifference > 20) LevelDifferenceModifier = 5.0f;       // Inimigo perigoso
	else if (LevelDifference > 10) LevelDifferenceModifier = 1.5f;       // Inimigo perigoso
	else if (LevelDifference > 5) LevelDifferenceModifier = 1.2f;  // Inimigo um pouco mais forte
	else if (LevelDifference < -15) LevelDifferenceModifier = 0.0f; // Inimigo trivial
	else if (LevelDifference < -10) LevelDifferenceModifier = 0.25f; // Inimigo muito fraco
	else if (LevelDifference < -5) LevelDifferenceModifier = 0.6f;  // Inimigo um pouco mais fraco
	
	float XPReturn = this->XP * LevelDifferenceModifier * this->DataAsset->Rarity.RarityModifier;
	
	return XPReturn;
};