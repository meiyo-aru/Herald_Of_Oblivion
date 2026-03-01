// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Structs/SkillStructs.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraDataInterfaceExport.h"
#include "NiagaraSystem.h"
#include "SkillActor.generated.h"

/**
 * ASkillActor
 * Actor que representa a habilidade no mundo, possui mesh e FX
 */

class USphereComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;
class USkillInstance;
class AEntityClass;
class UNiagaraSystem;
class USkillDataAsset;

UCLASS(Blueprintable, BlueprintType)
class HERALD_OF_OBLIVION_API ASkillActor : public AActor, public INiagaraParticleCallbackHandler
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkillActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginDestroy() override;

public:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse,
	           const FHitResult& Hit);
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void Initialize(USkillInstance* InInstance, AEntityClass* InEntity, FSkillContext& InSkillContext);
	
	FSkillContext SkillContext;
	
	int16 ParticlesProcessed = 0;
	UPROPERTY(VisibleAnywhere)
	TArray<UNiagaraComponent*> SpawnedNiagaraComponents;

	// A instancia da skill que deu origem a esta SkillActor
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<USkillInstance> Instance;
	
	// A entidade que lançou a habilidade
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AEntityClass> OwnerEntity;

	// A capsula de colisao
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* CollisionComponent = nullptr;
	
	// Root genérico (não tem collision)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* SceneRoot = nullptr;

	// A Mesh da habilidade
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	// 3. Declare a função obrigatória da interface. É aqui que os dados chegam!
	UFUNCTION(BlueprintCallable)
	virtual void ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data, UNiagaraSystem* NiagaraSystem, const FVector& SimulationPositionOffset) override;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UNiagaraComponent* NiagaraComponent = nullptr;
	
	// de 0 a 1, é uma medida de carregamento, 1 sendo o maximo
	float ChargeRatio = 0.0f;
	
	// Componente de movimento
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

private:
	void BindCollisionDelegatesFromSkillData(const USkillDataAsset* SkillData);
	bool bCollisionDelegatesBound = false;
};
