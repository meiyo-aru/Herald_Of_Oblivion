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
	UPROPERTY(EditAnywhere, Category="Thorn")
	float MaxLifeSpan = 7.0f;
	// Tempo de vida Minimo de um espinho
	UPROPERTY(EditAnywhere, Category="Thorn")
	float MinLifeSpan = 5.0f;
	// Escala minima do espinho
	UPROPERTY(EditAnywhere, Category="Thorn")
	FVector MinScale = FVector(0.8,0.8,1.5);
	// Escala maxima do espinho
	UPROPERTY(EditAnywhere, Category="Thorn")
	FVector MaxScale = FVector(1.2,1.2,2.0);
	// Quantidade máxima de espinhos
	UPROPERTY(EditAnywhere, Category="Thorn")
	int8 MaxThornsAmount = 5;
	// Espaçamento entre os espinhos
	UPROPERTY(EditAnywhere, Category="Thorn")
	float MaxSpacingBetweenThorns = 60.0f;
	
	TMap<int32, FEntityArrayWrapper> ParticlesIDCollided;
	
	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>> SpawnedNiagaraComponents) override;
	virtual void Initialize(USkillInstance* Owner) override;
	virtual void Execute(FSkillContext& InSkillContext) override;
	void SpawnThorn(FSkillContext& InSkillContext);
	void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext);
	// Executa alguma lógica nas particulas do niagara
	/*
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext) override;
	*/
	
	TArray<FHitResult> CheckSurfaceInAim(FSkillContext& InSkillContext, TArray<ESurfaceType>& InValidSurfaces);
	TArray<FHitResult> LineTraceAroundLocation(FVector StartLocation, FVector EndLocation,
											   FCollisionQueryParams CollisionParams,
											   TArray<ESurfaceType>& InValidSurfaces);

};
