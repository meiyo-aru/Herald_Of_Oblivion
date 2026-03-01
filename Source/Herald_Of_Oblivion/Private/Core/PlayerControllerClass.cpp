// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PlayerControllerClass.h"

APlayerControllerClass::APlayerControllerClass()
{
	// Permite que o mouse apareça na tela
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}

void APlayerControllerClass::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("Menu", IE_Pressed, this, &APlayerControllerClass::Menu);
}

void APlayerControllerClass::Menu()
{
}

