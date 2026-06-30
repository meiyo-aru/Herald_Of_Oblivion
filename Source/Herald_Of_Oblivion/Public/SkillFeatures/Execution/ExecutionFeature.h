// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "SkillFeatures/SkillFeature.h"
#include "ExecutionFeature.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USoundCue;
/**
 * UExecutionFeature
 * Feature pai de todas as features do tipo Execution
 */
UCLASS(Abstract)
class HERALD_OF_OBLIVION_API UExecutionFeature : public USkillFeature
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Execution", meta = (AssetBundles = "ExecutionVFX"))
	TSoftObjectPtr<UNiagaraSystem> ExecutionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Execution", meta = (AssetBundles = "ExecutionSFX"))
	TSoftObjectPtr<USoundCue> ExecutionSound;
	
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Realiza operações essenciais e comuns às classes filhas
	virtual void Execute(FSkillContext& InSkillContext);

	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>> SpawnedNiagaraComponents) override;

	// Executa alguma lógica nas particulas do niagara
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext);
	UPROPERTY()
	float ParticlesProcessed;
};
