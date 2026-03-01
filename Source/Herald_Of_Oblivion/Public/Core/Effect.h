// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Effect.generated.h"

class UNiagaraSystem;
class AEntityClass;
/**
 * UEffect
 * Respresenta um efeito causado por uma habiliidade ou item
 */
UCLASS()
class HERALD_OF_OBLIVION_API UEffect : public UObject
{
	GENERATED_BODY()
	
	// A entidade alvo do efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	TWeakObjectPtr<AEntityClass> TargetEntity;
	
	// A entidade que causou o efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	TWeakObjectPtr<AEntityClass> OwnerEntity;
	
	// O nível do efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	uint8 Level;

	
	// Ponteiro para o actor que soltou o efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	AActor* Owner;

	// // Quanto maior o nível melhor  o efeito
	// UPROPERTY(EditAnywhere, Category="Effect Properties")
	// uint8_t Level;
	
	// O FX do efeito
	// UPROPERTY(EditAnywhere, Category="Effect Properties")
	UNiagaraSystem* FX;
	
};
