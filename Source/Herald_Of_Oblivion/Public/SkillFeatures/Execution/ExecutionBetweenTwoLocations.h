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
	// Carrega os FX de forma assincrona ou não
	virtual void LoadFXSync() override;

	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Realiza operações essenciais e comuns às classes filhas
	virtual void Execute(FSkillContext& InSkillContext) override;
	
	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents) override;

	// Executa alguma lógica nas particulas do niagara
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext) override;
	
	// Caso True, o efeito usara o StartLocation do contexto da skill como início do efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	bool bUseStartLocation = true;
	
	// Define se usará o socket Forward da entidade.
	// É ignorado caso bUseStartLocation == true ou !bUseStartLocation && !bUseStartLocation
	UPROPERTY(EditAnywhere, Category="Properties", meta=(EditCondition="!bUseStartLocation || (!bUseStartLocation && !bUseEndLocation)", EditConditionHides))
	bool bUseForward = false;
	
	// Define se usará a arma ou mão direita como início do efeito, caso false usará a mão esquerda. É
	// ignorado caso bUseStartLocation == true ou !bUseStartLocation && !bUseStartLocation
	UPROPERTY(EditAnywhere, Category="Properties", meta=(EditCondition="!bUseStartLocation || (!bUseStartLocation && !bUseEndLocation)", EditConditionHides))
	bool bUseRight = true;

	// Define se usará o EndLocation do contexto da skill como fim do efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	bool bUseEndLocation = true;
	
	// Define se o spawn será no Start ou End location
	UPROPERTY(EditAnywhere, Category="Properties")
	bool bSpawnOnEndLocation = false;
	
	// Define o comprimento máximo do efeito, caso não use start e endlocation
	UPROPERTY(EditAnywhere, Category="Properties", meta=(EditCondition="!bUseStartLocation && !bUseEndLocation", EditConditionHides))
	float MaxLenght;	
	
	// Altura da CollisionBox
	UPROPERTY(EditAnywhere, Category="Properties")
	float CollisionHeight = 20.0f;
	// Profundidade da CollisionBox, eixo Y
	UPROPERTY(EditAnywhere, Category="Properties")
	float CollisionDepth = 20.0f;
	
	// Se True, a colisão é feita em cada partícula
	UPROPERTY(EditAnywhere, Category="Properties")
	bool bCollisionPerParticle = false;
};
