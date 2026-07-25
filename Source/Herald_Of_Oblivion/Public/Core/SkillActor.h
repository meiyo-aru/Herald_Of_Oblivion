// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Structs/SkillStructs.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraDataInterfaceExport.h"
#include "SkillFeatures/SkillFeature.h"
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
class UExecutionFeature;

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

public:
	virtual void BeginDestroy() override;
	
	// Chamado quando o CollisionComponent Hitta um actor
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse,
	           const FHitResult& Hit);
	
	// Chamado quando o CollisionComponent faz um Overlap em um Actor
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult);
	
	// Configura o CollisionComponent baseado na Feature e na EntityOwner
	void ConfigureCollisionComponent(USkillFeature* SkillFeature, AEntityClass* EntityOwner);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	
	// Quando o Niagara termina, destrói o Skill Actor
	void OnNiagaraSystemFinished(UNiagaraComponent* NC);

	// Inicializa com os valores
	void Initialize(USkillInstance* InInstance, AEntityClass* InEntity, FSkillContext& InSkillContext);
	
	// Utilizado para contar as párticulas do Niagara Processadas
	UPROPERTY(Transient)
	int16 ParticlesProcessed = 0;
	
	// Niagaras spawnados
	UPROPERTY(Transient, VisibleAnywhere)
	TArray<TObjectPtr<UNiagaraComponent>> SpawnedNiagaraComponents;

	// A instancia da skill que deu origem a esta SkillActor
	UPROPERTY(Transient, VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<USkillInstance> Instance;
	
	// A entidade que lançou a habilidade
	UPROPERTY(Transient, VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AEntityClass> OwnerEntity;

	// A capsula de colisao
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> CollisionComponent = nullptr;
	
	// A Mesh da habilidade
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent = nullptr;

	// 3. Declare a função obrigatória da interface. É aqui que os dados chegam!
	UFUNCTION(BlueprintCallable)
	virtual void ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data, UNiagaraSystem* NiagaraSystem, const FVector& SimulationPositionOffset) override;	
	
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> NiagaraComponent = nullptr;
	
	// de 0 a 1, é uma medida de carregamento, 1 sendo o maximo
	UPROPERTY(Transient)
	float ChargeRatio = 0.0f;
	
	// Componente de movimento
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent = nullptr;
};
