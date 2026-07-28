// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionFeature.h"
#include "SkillFeatures/SkillFeature.h"
#include "ExecutionSpawnProjectileFeature.generated.h"

class UNiagaraComponent;

/**
 * UExecutionSpawnProjectileFeature
 * Spawna um projetíl em uma direção.
 */
UCLASS()
class HERALD_OF_OBLIVION_API UExecutionSpawnProjectileFeature : public UExecutionFeature
{
	GENERATED_BODY()
	
public:
	// Velocidade do projétil
	UPROPERTY(EditAnywhere, Category="Projectile")
	float Speed;
	// Tempo de vida do projétil
	UPROPERTY(EditAnywhere, Category="Projectile")
	float LifeSpan;
	// Distância máxima que o projétil chega
	UPROPERTY(EditAnywhere, Category="Projectile")
	float MaximumRange = 50000;

	// Carrega os FX de forma assincrona ou não
	virtual void LoadFXSync() override;

	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Executa a lógica principal da Feature
	virtual void PrimaryExecute(FSkillContext& InSkillContext) override;
	// Execução final, geralmente ouve uma notificacao da animação
	virtual void FinallyExecute(FSkillContext& InSkillContext) override;

	// Spawna o projetil
	void SpawnProjectile(FSkillContext& InSkillContext, FName ShootingSocketName);
	
	virtual void OnPlayMontageNotifyBegin(FName NotifyName) override;
	
	// Executa alguma lógica nas particulas do niagara
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& InSkillContext) override;

	// Define se o projétil sai pelas mãos, caso False ele sai pelo SkillContext.StartLocation
	UPROPERTY(EditAnywhere, Category="Projectile")
	bool bThrowByTheHands = true;
	
	// Define se sairá da mao direita, se false sai pela esquerda
	UPROPERTY(EditAnywhere, Category="Projectile", meta=(EditCondition="bThrowByTheHands", EditConditionHides))
	bool bThrowByTheRightHand = true;
	
	// Define se usará o socket Forward da entidade.
	UPROPERTY(EditAnywhere, Category="Properties", meta=(EditCondition="!bThrowByTheHands", EditConditionHides))
	bool bThrowByTheForward = false;
};
