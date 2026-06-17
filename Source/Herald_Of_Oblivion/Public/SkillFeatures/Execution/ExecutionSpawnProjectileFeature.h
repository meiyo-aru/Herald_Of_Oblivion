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

	UPROPERTY(EditAnywhere, Category="Projectile")
	float Speed;
	
	UPROPERTY(EditAnywhere, Category="Projectile")
	float LifeSpan;
	
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	virtual void Execute(FSkillContext& InSkillContext) override;
	
	virtual void OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;
	virtual void OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent) override;

	// Spawna o projetil
	void SpawnProjectile(FSkillContext& InSkillContext, FName ShootingSocketName);
	// Executa alguma lógica nas particulas do niagara
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path", meta = (AssetBundles = "PathVFX"))
	TSoftObjectPtr<UNiagaraSystem> PathEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path", meta = (AssetBundles = "PathSFX"))
	TSoftObjectPtr<USoundCue> PathSound;
	
	// Define se o projétil sai pelas mãos, caso False ele sai pelo SkillContext.StartLocation
	UPROPERTY(EditAnywhere, Category="Projectile")
	bool bThrowByTheHands = true;
	
	// Define se o lançamento do projétil será paralelo ao terreno
	UPROPERTY(EditAnywhere, Category="Projectile")
	bool bParallelToTheTerrain = false;
	
	// Define as propriedades de colisao da skill
	UPROPERTY(EditAnywhere, Category="Projectile")
	float RadiusCollision = 0.0f;
	UPROPERTY(EditAnywhere, Category="Projectile")
	bool bGenerateOverlapEvents = false;
	UPROPERTY(EditAnywhere, Category="Projectile")
	bool bGenerateHitEvents = false;
	UPROPERTY(EditAnywhere, Category="Projectile")
	bool bNotifyRigidBodyCollision = false;
	UPROPERTY(EditAnywhere, Category="Projectile")
	TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled = ECollisionEnabled::NoCollision;
	UPROPERTY(EditAnywhere, Category="Projectile")
	TEnumAsByte<ECollisionResponse> PawnCollision = ECR_Ignore;
	UPROPERTY(EditAnywhere, Category="Projectile")
	TEnumAsByte<ECollisionResponse> WorldStaticCollision = ECR_Ignore;
	UPROPERTY(EditAnywhere, Category="Projectile")
	TEnumAsByte<ECollisionResponse> WorldDynamicCollision = ECR_Ignore;

};
