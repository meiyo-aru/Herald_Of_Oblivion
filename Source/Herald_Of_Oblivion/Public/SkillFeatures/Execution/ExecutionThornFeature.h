// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "ExecutionThornFeature.generated.h"

/**
 * UExecutionThornFeature
 * Spawna espinhos na através de um vetor normalizado
 */

USTRUCT(BlueprintType)
struct FEntityArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FName> Entities;
};

UCLASS()
class HERALD_OF_OBLIVION_API UExecutionThornFeature : public UExecutionFeature
{
	GENERATED_BODY()
public:
	// Tempo de vida Maximo de um espinho
	UPROPERTY(EditAnywhere)
	float MaxLifeSpan = 7.0f;
	// Tempo de vida Minimo de um espinho
	UPROPERTY(EditAnywhere)
	float MinLifeSpan = 5.0f;
	
	
	float ModifierTarget = 450;
	float ModifierOffsetThorns = -80;
	
	UPROPERTY()
	TMap<int32, FEntityArrayWrapper> ParticlesIDCollided;
	
	// Escala minima do espinho
	UPROPERTY(EditAnywhere)
	FVector MinScale = FVector(0.8,0.8,1.5);
	// Escala maxima do espinho
	UPROPERTY(EditAnywhere)
	FVector MaxScale = FVector(1.2,1.2,2.0);
	
	virtual void Initialize(USkillInstance* Owner) override;
	virtual void Execute(FSkillContext& InSkillContext) override;
	void SpawnThorn(FSkillContext& InSkillContext);
	virtual void OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;
	virtual void OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;
	// Executa alguma lógica nas particulas do niagara
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext) override;
};
