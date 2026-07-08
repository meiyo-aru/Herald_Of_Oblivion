// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillFeatures/SkillFeature.h"
#include "ActivationFeature.generated.h"

/**
 * UActivationFeature
 * Feature pai de todas as features do tipo activation 
 */

class UNiagaraComponent;
class UNiagaraSystem;
class USoundCue;

UCLASS(Abstract)
class HERALD_OF_OBLIVION_API UActivationFeature : public USkillFeature
{
	GENERATED_BODY()
	
public:
	FTimerHandle TimerHandle;
	
	// Controla o raio da mira, quanto menor, mais preciso
	UPROPERTY(EditAnywhere, Category="Aim")
	float AimRadius = 5.0f;
	
	// Carrega os FX de forma síncrona
	virtual void LoadFXSync();

	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;

	// Retorna um HitResult baseado na visão do jogador
	FHitResult GetAimTarget(FSkillContext& InContext, float InAimRadius) const;
	// Retorna um HitResult baseado no cursor
	FHitResult GetCursorLocation(FSkillContext& InContext) const;
	
	// Limpa o Niagara
	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents) override;

	// Lógica de ativação inicial e final
	virtual void StartActivation(FSkillContext& InSkillContext);	
	virtual void CompleteActivation(FSkillContext& InSkillContext);	

	
	// Efeito de Ativação
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation FX", meta = (AssetBundles = "FX"))
	TSoftObjectPtr<UNiagaraSystem> ActivationEffect;
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedActivationEffect = nullptr;

	// Som de Ativação
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation FX", meta = (AssetBundles = "FX"))
	TSoftObjectPtr<USoundCue> ActivationSound;
	UPROPERTY()
	TObjectPtr<USoundCue> LoadedSoundCue = nullptr;

};

