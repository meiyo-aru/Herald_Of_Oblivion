// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnHitFeature.h"
#include "OnHitBleedEntity.generated.h"

/**
 * UOnHitBleedEntity
 * Spawna um VFX de sangue na entidade hittada
 */
UCLASS()
class HERALD_OF_OBLIVION_API UOnHitBleedEntity : public UOnHitFeature
{
	GENERATED_BODY()
	
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Realiza operações essenciais e comuns às classes filhas
	virtual void Execute(FSkillContext& InSkillContext, FVector TargetLocation) override;

	void ExecuteOnHitEntity(FSkillContext& InSkillContext, AEntityClass* TargetEntity);

};
