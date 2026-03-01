// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionFeature.h"
#include "SkillFeatures/SkillFeature.h"
#include "ExecutionSpawnProjectileFeature.generated.h"

class UNiagaraComponent;

/**
 * UExecutionSpawnProjectileFeature
 * Spawna um projetíl em uma direção.
 */
UCLASS()
class HERALD_OF_OBLIVION_API UExecutionSpawnProjectileFeature : public UExecutionFeature
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	float Speed;
	
	UPROPERTY(EditAnywhere)
	float LifeSpan;
	
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	virtual void Execute(FSkillContext& InSkillContext) override;
	
	virtual void OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;
	virtual void OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;

	// Spawna o projetil
	void SpawnProjectile(FSkillContext& InSkillContext, FName ShootingSocketName);
	// Executa alguma lógica nas particulas do niagara
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext) override;
};
