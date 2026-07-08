// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectFeatures/EffectFeature.h"
#include "Structs/SkillStructs.h"
#include "UObject/Object.h"
#include "ExecutionEffectFeature.generated.h"

/**
 * UExecutionEffectFeature
 * A feature pai de todas as features com lógica de execução
 */
UCLASS(Abstract)
class HERALD_OF_OBLIVION_API UExecutionEffectFeature : public UEffectFeature
{
	GENERATED_BODY()
	
public:
	virtual void Execute(const FHitOverlapResult& HitOverlapResult);
	
};
