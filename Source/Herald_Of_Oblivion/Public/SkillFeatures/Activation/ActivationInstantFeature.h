// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActivationFeature.h"
#include "ActivationInstantFeature.generated.h"

/**
 * UInstantActivationFeature
 * Feature responsável por ativar a lógica da habilidade imediatamente assim que o cast é iniciado.
 * Ideal para habilidades que não possuem tempo de carga (channeled).
 * @see USkillFeature, USkillInstance
 */

class UNiagaraComponent;

UCLASS()
class HERALD_OF_OBLIVION_API UActivationInstantFeature : public UActivationFeature
{
	GENERATED_BODY()
	
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Lógica de ativação inicial e final
	virtual void StartActivation(FSkillContext& InSkillContext) override;
	
	FTimerHandle CastTimerHandle;
};
