// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnHitFeature.h"
#include "OnHitSurfaceExplosion.generated.h"

class USoundCue;
/**
 * UOnHitSurface
 * Spawna uma explosão ao atingir
 */
UCLASS()
class HERALD_OF_OBLIVION_API UOnHitSurfaceExplosion : public UOnHitFeature
{
	GENERATED_BODY()
public:
	
	// Carrega os FX de forma assincrona ou não
	virtual void LoadFXSync() override;

	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Realiza operações essenciais e comuns às classes filhas
	virtual void Execute(FSkillContext& InSkillContext) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OnHit FX", meta = (AssetBundles = "FX"))
	TSoftObjectPtr<UNiagaraSystem> OnHitEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OnHit FX", meta = (AssetBundles = "FX"))
	TSoftObjectPtr<USoundCue> OnHitSound;
	
	UPROPERTY(EditAnywhere, Category = "Explosion")
	float ExplosionRadius = 30.0f;
	
	UPROPERTY(EditAnywhere, Category = "Explosion")
	FVector ExplosionIntensityScale = FVector(1.0f,1.0f,1.0f);
	
	UPROPERTY(EditAnywhere, Category = "Explosion")
	float ExplosionTemperature = 1.0f;
};
