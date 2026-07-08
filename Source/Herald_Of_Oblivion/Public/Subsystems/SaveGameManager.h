// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGameManager.generated.h"

/**
 * 
 */
UCLASS()
class HERALD_OF_OBLIVION_API USaveGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	void SaveGame();
};
