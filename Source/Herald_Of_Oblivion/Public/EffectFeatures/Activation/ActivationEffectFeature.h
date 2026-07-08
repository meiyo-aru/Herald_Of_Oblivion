// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectFeatures/EffectFeature.h"
#include "Structs/SkillStructs.h"
#include "UObject/Object.h"
#include "ActivationEffectFeature.generated.h"

/**
 * UActivationEffectFeature
 * Feature de efeitos. Pai de todas as features com lógica de ativação
 */
UCLASS(Abstract)
class HERALD_OF_OBLIVION_API  UActivationEffectFeature : public UEffectFeature
{
	GENERATED_BODY()
public:
	virtual void Init(FHitOverlapResult& HitOverlapResult) override;
	
	// Executa a lógica principal do efeito
	virtual void Execute(FHitOverlapResult& HitOverlapResult);
};
