// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EntityAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class HERALD_OF_OBLIVION_API UEntityAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Animation")
	float AnimationMontageBlendWeight = 1.f;
};
