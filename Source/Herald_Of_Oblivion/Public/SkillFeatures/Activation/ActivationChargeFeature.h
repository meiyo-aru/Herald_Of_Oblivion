// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillFeatures/Activation/ActivationFeature.h"
#include "ActivationChargeFeature.generated.h"

/**
 * UActivationChargeFeature
 * Reponsável pela lógica das habilidades que podem ser carregadas
 */
UCLASS()
class HERALD_OF_OBLIVION_API UActivationChargeFeature : public UActivationFeature
{
	GENERATED_BODY()
	
public:
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Lógica de ativação inicial e final
	virtual void StartActivation(FSkillContext& InSkillContext) override;
	virtual void BeginDestroy() override;
	virtual void CompleteActivation(FSkillContext& InSkillContext) override;
	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>> SpawnedNiagaraComponents) override;
	
	// ==================== Charge Properties ====================
	// Skill carregada na mao esquerda ou direita, ou ambas
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	bool bChargeOnLeftHand = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	bool bChargeOnRightHand = false;

	// Tempo de Charge
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float MaxChargeTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float MinChargeTime = 0.0f;

	// Se o efeito de Charge deve estar orientado para o alvo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	bool bChargeOnTarget = false;

	// Se o efeito de carga deve seguir a mira
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge", meta=(EditCondition="bChargeOnTarget", EditConditionHides))
	bool bChargeFollowAim = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge", meta=(EditCondition="bChargeFollowAim && bChargeOnTarget", EditConditionHides))
	float RateToFollow = 0.5f;
	
	FTimerHandle TimerHandleChargeFollowAim;

	
	// Define se a habilidade possui aura ao ser Carregada
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX")
	bool bAura = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	bool bAuraInEntityOwner = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	bool bAuraInWeapon = false;
	
	// Os VFX da aura
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta = (AssetBundles = "EntityOwnerAuraVFX", EditCondition="bAura", EditConditionHides))
	TSoftObjectPtr<UNiagaraSystem> EntityOwnerAuraEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta = (AssetBundles = "WeaponAuraVFX", EditCondition="bAura", EditConditionHides))
	TSoftObjectPtr<UNiagaraSystem> WeaponAuraEffect;
	
	// Efeito de Ativação que segue a mira
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation FX", meta = (AssetBundles = "ActivationVFX", EditCondition="bChargeOnTarget", EditConditionHides))
	TSoftObjectPtr<UNiagaraSystem> ActivationFollowingAimEffect;

	// Som de Ativação que segue a mira
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation FX", meta = (AssetBundles = "ActivationSFX", EditCondition="bChargeOnTarget", EditConditionHides))
	TSoftObjectPtr<USoundCue> ActivationFollowingAimSound;

	
	// Quantidade de párticulas
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	float SpawnRateAura = 0.0f;
	// Localização relativa da aura
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	float NormalOffsetAura = 0.0f;
	// lifetime minimo e maximo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	float MinLifeTimeAura = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	float MaxLifeTimeAura = 0.0f;
	
};
