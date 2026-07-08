// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core/EffectInstance.h"
#include "CoreMinimal.h"
#include "Structs/SkillStructs.h"
#include "UObject/Object.h"
#include "EffectFeature.generated.h"

class UEffectInstance;
class AEntityClass;
/**
 * UEffectFeature
 * Classe pai de todas as features de efeito. É um UObject editável no editor e instanciável.
 * Uma feature são objetos de lógica e comportamentos essenciais para o funcionamento dos efeitos
 * As features se comunicam entre si através de Delegates disparados pela instância de skill da qual pertencem.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class HERALD_OF_OBLIVION_API UEffectFeature : public UObject
{
	GENERATED_BODY()
	
	public:
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Init(FHitOverlapResult& HitOverlapResult);
};
