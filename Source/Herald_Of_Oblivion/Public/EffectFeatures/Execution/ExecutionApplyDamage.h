// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionEffectFeature.h"
#include "Enumerators/EntityEnums.h"
#include "ExecutionApplyDamage.generated.h"

/**
 * 
 */
UCLASS()
class HERALD_OF_OBLIVION_API UExecutionApplyDamage : public UExecutionEffectFeature
{
	GENERATED_BODY()
	public:
	virtual void Execute(const FHitOverlapResult& HitOverlapResult) override;

	UPROPERTY(EditAnywhere)
	ETypeDamage TypeDamage = ETypeDamage::None;
	
	UPROPERTY(EditAnywhere)
	int8 Damage;
};
