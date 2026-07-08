// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/EffectInstance.h"
#include "UObject/Object.h"
#include "EffectDataAsset.generated.h"

class UExecutionEffectFeature;
class UActivationEffectFeature;
class UNiagaraSystem;
class USoundCue;

UENUM(BlueprintType)
enum class EEffectType : uint8
{
	Damage		UMETA(DisplayName = "Damage"),
	Recover		UMETA(DisplayName = "Recover"),
	Shield		UMETA(DisplayName = "Shield"),
	Buff		UMETA(DisplayName = "Buff"),
	Debuff		UMETA(DisplayName = "Debuff"),
	CrowdControll		UMETA(DisplayName = "CrowdControll"),
	None            UMETA(DisplayName = "None")
};

/**
 * UEffectDataAsset
 * O DataAsset que define o efeito. Define caracteristicas fundamentais.
 */
UCLASS(Blueprintable, BlueprintType)
class HERALD_OF_OBLIVION_API UEffectDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UEffectDataAsset(){};
	FPrimaryAssetId GetPrimaryAssetId() const {
		return FPrimaryAssetId("Effect", GetFName());
	}

	UEffectInstance* GetInstance(AActor* InOriginatingEntity = nullptr, AEntityClass* InTargetEntity = nullptr);

	// O Tipo do efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	EEffectType EffectType;
	
	// Define se o efeito pode acumular
	UPROPERTY(EditAnywhere, Category="Properties")
	bool bAcumulateEffect = false;
	
	// Define a quantidade máxima de acúmulos de um efeito na mesma entidade
	UPROPERTY(EditAnywhere, Category="Properties", meta=(EditCondition="bAcumulateEffect", EditConditionHides))
	int8 MaxEffectStacks = 5;
	
	// Feature de ativação do efeito, implementa a lógica de ativação
	UPROPERTY(EditAnywhere, Instanced, Category = "Features")
	TObjectPtr<UActivationEffectFeature> ActivationFeature;
	
	// Feature de execução do efeito, implementa a lógica de ativação
	UPROPERTY(EditAnywhere, Instanced, Category = "Features")
	TObjectPtr<UExecutionEffectFeature> ExecutionFeature;
		
	/*
	// O Atributo afetado pelo efeito, positavemente ou negativamente
	UPROPERTY(EditAnywhere, Category = "Properties")
	FAffectAttributeStruct AffectAttribute;
	
	// O recurso recuperado pelo efeito
	UPROPERTY(EditAnywhere, Category = "Properties")
	FRecoverStruct RecoverStruct;
	
	// O dano causado pelo efeito
	UPROPERTY(EditAnywhere, Category = "Properties")
	FDamageStruct DamageStruct;
	*/
	
	/*
	// As restrições causadas pelo efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	TArray<ERestrictionTypeEnum> RestrictionTypes;
	*/
};
