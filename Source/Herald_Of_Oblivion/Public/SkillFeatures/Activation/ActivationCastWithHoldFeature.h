// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillFeatures/Activation/ActivationFeature.h"
#include "ActivationCastWithHoldFeature.generated.h"

/**
 * UCastWithHoldActivationFeature
 * Reponsável pela lógica das habilidades que possuem CastTime
 */
UCLASS()
class HERALD_OF_OBLIVION_API UActivationCastWithHoldFeature : public UActivationFeature
{
	GENERATED_BODY()
	
public:
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;

	// Limpa os handles utilizados
	virtual void OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;
	virtual void OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;
	
	// Lógica de ativação inicial e final
	virtual void StartActivation(FSkillContext& InSkillContext) override;
	virtual void BeginDestroy() override;
	virtual void CompleteActivation(FSkillContext& InSkillContext) override;
	void CleanNiagara();
};
