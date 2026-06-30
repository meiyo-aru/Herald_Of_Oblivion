// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionFeature.h"
#include "ExecutionBetweenTwoLocations.generated.h"

/**
 * UExecutionEffectBetweenTwoLocations
 * Habilidades que percurrem de um ponto ao outro, mas não são projéteis
 */
UCLASS()
class HERALD_OF_OBLIVION_API UExecutionBetweenTwoLocations : public UExecutionFeature
{
	GENERATED_BODY()
	
public:
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Realiza operações essenciais e comuns às classes filhas
	virtual void Execute(FSkillContext& InSkillContext) override;
	
	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>> SpawnedNiagaraComponents) override;

	// Executa alguma lógica nas particulas do niagara
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext) override;
	
	// Caso True, o efeito usara o StartLocation, caso false usará o EndLocation mesmo que ambos os bUseStartLocation e bUseEndLocation sejam falsos
	UPROPERTY(EditAnywhere, Category="Properties")
	bool bUseStartLocation = true;
	
	UPROPERTY(EditAnywhere, Category="Properties", meta=(EditCondition="!bUseStartLocation", EditConditionHides))
	bool bUseRight = true;

	UPROPERTY(EditAnywhere, Category="Properties")
	bool bUseEndLocation = true;
	
	UPROPERTY(EditAnywhere, Category="Properties")
	bool bSpawnOnEndLocation = false;
	
	// Define a distância máxima do efeito, caso não use start e endlocation
	UPROPERTY(EditAnywhere, Category="Properties")
	float MaxLenght;	
	
	// Se True, a colisão é feita em cada partícula
	UPROPERTY(EditAnywhere, Category="Properties")
	bool bCollisionPerParticle = false;
};
