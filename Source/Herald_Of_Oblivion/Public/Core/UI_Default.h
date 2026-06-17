// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_Default.generated.h"

/**
 * 
 */
UCLASS()
class HERALD_OF_OBLIVION_API UUI_Default : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;
	
	UFUNCTION()
	void UpdateHealthBar(float NewHealth);
	
};
