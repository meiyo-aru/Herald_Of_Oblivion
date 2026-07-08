// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnHitFeature.h"
#include "OnHitApplyEffect.generated.h"

class UEffectDataAsset;

/**
 * UOnHitApplyEffect
 * Aplica efeitos ao atingir
 */
UCLASS()
class HERALD_OF_OBLIVION_API UOnHitApplyEffect : public UOnHitFeature
{
	GENERATED_BODY()
public:
	// Carrega os FX de forma assincrona ou não
	virtual void LoadFXSync() override;

	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Realiza operações essenciais e comuns às classes filhas
	virtual void Execute(FSkillContext& InSkillContext) override;
	
	UPROPERTY(EditAnywhere, Category="Properties")
	TArray<TObjectPtr<UEffectDataAsset>> OnHitEffects;
};
