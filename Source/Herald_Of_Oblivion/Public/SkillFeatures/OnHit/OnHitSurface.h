// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnHitFeature.h"
#include "OnHitSurface.generated.h"

/**
 * UOnHitSurface
 * Feature para spawnar efeito ao atingir uma superfície
 */
UCLASS()
class HERALD_OF_OBLIVION_API UOnHitSurface : public UOnHitFeature
{
	GENERATED_BODY()
	
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Realiza operações essenciais e comuns às classes filhas
	virtual void Execute(FSkillContext& InSkillContext, FVector TargetLocation) override;

};
