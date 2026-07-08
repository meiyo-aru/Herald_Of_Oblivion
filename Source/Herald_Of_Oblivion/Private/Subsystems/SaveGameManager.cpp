// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/SaveGameManager.h"

#include "Core/SaveGameClass.h"
#include "Kismet/GameplayStatics.h"

void USaveGameManager::SaveGame()
{
	USaveGameClass* SaveObj = Cast<USaveGameClass>(UGameplayStatics::CreateSaveGameObject(USaveGameClass::StaticClass()));
	UGameplayStatics::SaveGameToSlot(SaveObj, "Slot1", 0);
}
