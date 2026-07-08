// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/SkillStructs.h"
#include "UObject/Object.h"
#include "EffectInstance.generated.h"

class UExecutionEffectFeature;
class UActivationEffectFeature;
class UEffectDataAsset;
class UNiagaraSystem;
class AEntityClass;
/**
 * UEffect
 * Respresenta um efeito causado por uma habilidade ou item
 * Uma entidade só pode ser afetada por uma instância do mesmo efeito, ao invés de acumular instâncias do mesmo efeito, o Stack
 * da instância original aumenta e a duração do efeito reseta
 * 
 */
UCLASS()
class HERALD_OF_OBLIVION_API UEffectInstance : public UObject
{
	GENERATED_BODY()
public:
	void ApplyOnTargetEntity(FHitOverlapResult& HitOverlapResult);
	
	void Initialize(UEffectDataAsset* InDataAsset, AActor* InOriginatingEntity,
	                AEntityClass* InTargetEntity, UActivationEffectFeature* InActivationFeature, UExecutionEffectFeature*
	                InExecutionFeature);
	void SaveToPool();

	// Quando um efeito é aplicado mais de uma vez ao mesmo alvo, o Stack do efeito aplicado aumenta
	UPROPERTY(VisibleAnywhere, Category="Properties")
	int8 Stacks = 0;
	
	// Quando um efeito é aplicado em um alvo que já possui o mesmo efeito, a duração deste é resetada
	UPROPERTY(VisibleAnywhere, Category="Properties")
	bool Reset = false;
	
	// O DataAsset do efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	TObjectPtr<UEffectDataAsset> DataAsset;	
	
	// A entidade alvo do efeito
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AEntityClass> TargetEntity;
	
	// A entidade que causou o efeito
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AActor> OriginatingActor;
	
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TObjectPtr<UActivationEffectFeature> ActivationFeature; 
	
	UPROPERTY(VisibleAnywhere, Category="Properties")
	TObjectPtr<UExecutionEffectFeature> ExecutionFeature;
};
