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
	virtual void CleanNiagara() override;
	
	// ==================== Cast Properties ====================
	// Skill castada na mao esquerda ou direita, ou ambas
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	bool bCastOnLeftHand = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	bool bCastOnRightHand = false;

	// Tempo de cast
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	float MaxCastTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	float MinCastTime = 0.0f;

	// Se o efeito de cast deve estar orientado para o alvo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	bool bCastOnTarget = false;

	// Define se a habilidade possui aura ao ser castada
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	bool bAura = false;

	// O VFX da aura
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura", meta = (AssetBundles = "SkeletalMeshAuraVFX"))
	TSoftObjectPtr<UNiagaraSystem> SkeletalMeshAuraEffect;
	
	// Quantidade de párticulas
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	float SpawnRateAura = 0.0f;
	// Localização relativa da aura
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	float NormalOffsetAura = 0.0f;
	// lifetime minimo e maximo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	float MinLifeTimeAura = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	float MaxLifeTimeAura = 0.0f;

	
	// Efeito de cast
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast", meta = (AssetBundles = "CastVFX"))
	TSoftObjectPtr<UNiagaraSystem> CastEffect;

	// Som de cast
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast", meta = (AssetBundles = "CastSFX"))
	TSoftObjectPtr<USoundCue> CastSound;

};
