// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillFeatures/SkillFeature.h"
#include "OnHitFeature.generated.h"

/**
 * UOnHitFeature
 * Classe pai de todas as features OnHit. São features que contem a lógica do que acontece com o Actor que tomou o Hit
 */
UCLASS(Abstract)
class HERALD_OF_OBLIVION_API UOnHitFeature : public USkillFeature
{
	GENERATED_BODY()
	
public:
	// Carrega os FX de forma assincrona ou não
	virtual void LoadFXSync();

	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Realiza operações essenciais e comuns às classes filhas
	virtual void Execute(FSkillContext& InSkillContext);

	
	// Executa alguma lógica nas particulas do niagara
	// virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext);
	// UPROPERTY()
	// float ParticlesProcessed;
	
	// virtual void OnHitEntity(AEntityClass* InEntityHitted, FSkillContext& InSkillContext) override;

};
