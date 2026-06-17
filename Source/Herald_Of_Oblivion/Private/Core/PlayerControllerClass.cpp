// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PlayerControllerClass.h"
#include "Core/UI_Default.h"

APlayerControllerClass::APlayerControllerClass()
{
	// Permite que o mouse apareça na tela
	bShowMouseCursor = false;
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

void APlayerControllerClass::BeginPlay()
{
	Super::BeginPlay();
	
	if (HUDClass)
	{
		HUDWidget = CreateWidget<UUI_Default>(this, HUDClass);
		if (HUDWidget) HUDWidget->AddToViewport();
	}
}

