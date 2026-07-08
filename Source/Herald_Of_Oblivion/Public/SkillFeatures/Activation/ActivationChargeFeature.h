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
	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents) override;
	
	// Carrega os FX de forma assincrona ou não
	virtual void LoadFXSync() override;
	
	// ==================== Charge Properties ====================
	// Skill carregada na mao esquerda
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	bool bChargeOnLeftHand = false;
	// Skill carregada na mao direita
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	bool bChargeOnRightHand = false;

	// Tempo de carga máximo da habilidade
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float MaxChargeTime = 0.0f;
	// Tempo de carga mínimo da habilidade
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	float MinChargeTime = 0.0f;

	// Se o efeito de Charge deve estar orientado para o alvo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge")
	bool bChargeOnTarget = false;

	// Se o efeito de carga deve seguir a mira
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge", meta=(EditCondition="bChargeOnTarget", EditConditionHides))
	bool bChargeFollowAim = false;
	// Frequencia de atualizacao do efeito seguindo a mira
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge", meta=(EditCondition="bChargeFollowAim && bChargeOnTarget", EditConditionHides))
	float RateToFollow = 0.5f;
	
	FTimerHandle TimerHandleChargeFollowAim;

	
	// Define se a habilidade possui aura ao ser Carregada
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX")
	bool bAura = false;
	// Define se a entidade terá efeito de aura
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	bool bAuraInEntityOwner = false;
	// Define se a arma terá efeito de aura
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	bool bAuraInWeapon = false;
	
	// O VFX da aura da entidade
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta = (AssetBundles = "FX", EditCondition="bAura", EditConditionHides))
	TSoftObjectPtr<UNiagaraSystem> EntityOwnerAuraEffect;
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedEntityOwnerAuraEffect = nullptr;
	
	// O VFX da aura da arma
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta = (AssetBundles = "FX", EditCondition="bAura", EditConditionHides))
	TSoftObjectPtr<UNiagaraSystem> WeaponAuraEffect;
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedWeaponAuraEffect = nullptr;
	
	// Efeito de Ativação que segue a mira
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation FX", meta = (AssetBundles = "FX", EditCondition="bChargeOnTarget", EditConditionHides))
	TSoftObjectPtr<UNiagaraSystem> ActivationFollowingAimEffect;
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedActivationFollowingAimEffect = nullptr;

	// Som de Ativação que segue a mira
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation FX", meta = (AssetBundles = "FX", EditCondition="bChargeOnTarget", EditConditionHides))
	TSoftObjectPtr<USoundCue> ActivationFollowingAimSound;
	UPROPERTY()
	TObjectPtr<USoundCue> LoadedActivationFollowingAimSound = nullptr;
	
	// Quantidade de párticulas
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	float SpawnRateAura = 0.0f;
	// Localização relativa da aura
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	float NormalOffsetAura = 0.0f;
	// lifetime mínimo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	float MinLifeTimeAura = 0.0f;
	// lifetime máximo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura FX", meta=(EditCondition="bAura", EditConditionHides))
	float MaxLifeTimeAura = 0.0f;
	
};
