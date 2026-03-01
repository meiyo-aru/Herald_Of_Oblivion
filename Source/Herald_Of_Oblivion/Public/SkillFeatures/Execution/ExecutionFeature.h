// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillFeatures/SkillFeature.h"
#include "ExecutionFeature.generated.h"

class UNiagaraComponent;
/**
 * UExecutionFeature
 * Feature pai de todas as features do tipo Execution
 */
UCLASS(Abstract)
class HERALD_OF_OBLIVION_API UExecutionFeature : public USkillFeature
{
	GENERATED_BODY()
	
public:
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Realiza operações essenciais e comuns às classes filhas
	virtual void Execute(FSkillContext& InSkillContext);

	virtual void OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;
	virtual void OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;
	
	// Executa alguma lógica nas particulas do niagara
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext);
	UPROPERTY()
	float ParticlesProcessed;
};
