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
	
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;

	FHitResult GetAimTarget(FSkillContext& InContext, float Sensibility) const;
	FHitResult GetCursorLocation(FSkillContext& InContext) const;
	
	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>> SpawnedNiagaraComponents) override;

	// Lógica de ativação inicial e final
	virtual void StartActivation(FSkillContext& InSkillContext);	
	virtual void CompleteActivation(FSkillContext& InSkillContext);	
	
	// Efeito de Ativação
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation FX", meta = (AssetBundles = "ActivationVFX"))
	TSoftObjectPtr<UNiagaraSystem> ActivationEffect;

	// Som de Ativação
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation FX", meta = (AssetBundles = "ActivationSFX"))
	TSoftObjectPtr<USoundCue> ActivationSound;

};

