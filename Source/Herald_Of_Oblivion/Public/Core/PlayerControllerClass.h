// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerClass.generated.h"

/**
 * 
 */
UCLASS()
class HERALD_OF_OBLIVION_API APlayerControllerClass : public APlayerController
{
	GENERATED_BODY()
	
public:
	APlayerControllerClass();
	
private:
	// Configuração de inputs que pertencem ao Jogador (ex: abrir menus)
	virtual void SetupInputComponent() override;
	void Menu();
	
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUI_Default> HUDClass;
	
	UPROPERTY() 
	UUI_Default* HUDWidget;
};
