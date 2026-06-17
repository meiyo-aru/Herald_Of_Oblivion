// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/UI_Default.h"

#include "Character/PlayerClass.h"
#include "Components/ProgressBar.h"

void UUI_Default::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (APlayerClass* Char = Cast<APlayerClass>(PC->GetCharacter()))
		{
			Char->OnHealthChanged.BindUObject(this, &UUI_Default::UpdateHealthBar);
		}
	}
}

void UUI_Default::UpdateHealthBar(float NewHealth)
{
	HealthBar->SetPercent(NewHealth);
}
