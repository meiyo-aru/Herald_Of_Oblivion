// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "ExecutionUniqueSpawnAtLocationFeature.generated.h"

/**
 * UExecutionUniqueSpawnAtLocationFeature
 * Spawna a habilidade no local alvo
 */
UCLASS()
class HERALD_OF_OBLIVION_API UExecutionUniqueSpawnAtLocationFeature : public UExecutionFeature
{
	GENERATED_BODY()
	
public:
	// Mínimo de tempo de vida do niagara e do actor
	UPROPERTY(EditAnywhere)
	float MinLifeSpan = 0.0f;
	// Máximo de tempo de vida do niagara e do actor
	UPROPERTY(EditAnywhere)
	float MaxLifeSpan = 1.0f;
	
	virtual void Initialize(USkillInstance* Owner) override;
	virtual void Execute(FSkillContext& InSkillContext) override;
	void SpawnAtLocation(FSkillContext& InSkillContext);
	virtual void OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;
	virtual void OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;
	// Executa alguma lógica nas particulas do niagara
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext) override;
};
