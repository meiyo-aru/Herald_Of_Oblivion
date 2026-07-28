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
	// Mínimo de tempo de vida do niagara
	UPROPERTY(EditAnywhere, Category="Properties")
	float MinLifeSpan = 0.0f;
	// Máximo de tempo de vida do niagara
	UPROPERTY(EditAnywhere, Category="Properties")
	float MaxLifeSpan = 1.0f;
	
	// Intensidade do efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	float Intensity = 0.0f;
	// Raio de colisão do efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	float CollisionRadius = 0.0f;
	
	// Define se spawna ao castar a skill ou ao soltar
	UPROPERTY(EditAnywhere, Category="Properties")
	bool bSpawnOnEndLocation = true;

	// Carrega os FX de forma assincrona ou não
	virtual void LoadFXSync() override;

	// Limpa o Niagara
	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents) override;
	// Inicializa com os valores
	virtual void Initialize(USkillInstance* Owner) override;
	// Executa a lógica principal
	virtual void PrimaryExecute(FSkillContext& InSkillContext) override;
	// Execução final, geralmente ouve uma notificacao da animação
	virtual void FinallyExecute(FSkillContext& InSkillContext) override;

	virtual void OnPlayMontageNotifyBegin(FName NotifyName) override;

	// Spawna o efeito em um local
	void SpawnAtLocation(FSkillContext& InSkillContext);
	// Executa alguma lógica nas particulas do niagara
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& InSkillContext) override;
};
