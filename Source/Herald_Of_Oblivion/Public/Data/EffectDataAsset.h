// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/AffectAttributeStruct.h"
#include "Structs/DamageStruct.h"
#include "Structs/RecoverStruct.h"
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
	FPrimaryAssetId GetPrimaryAssetId() const {
		return FPrimaryAssetId("Skill", GetFName());
	}
	// O Atributo afetado pelo efeito, positavemente ou negativamente
	UPROPERTY(EditAnywhere, Category = "Properties")
	FAffectAttributeStruct AffectAttribute;
	
	// O recurso recuperado pelo efeito
	UPROPERTY(EditAnywhere, Category = "Properties")
	FRecoverStruct RecoverStruct;
	
	// O dano causado pelo efeito
	UPROPERTY(EditAnywhere, Category = "Properties")
	FDamageStruct DamageStruct;
	
	// Feature de ativação da habilidade, implementa a lógica de ativação
	UPROPERTY(EditAnywhere, Instanced, Category = "Features")
	UExecutionEffectFeature* ExecutionFeature;
	
	// Feature de ativação da habilidade, implementa a lógica de ativação
	UPROPERTY(EditAnywhere, Instanced, Category = "Features")
	UActivationEffectFeature* ActivationFeature;
	
	// O FX do efeito
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX", meta = (AssetBundles = "VFX"))
	TSoftObjectPtr<UNiagaraSystem> VFX;

	// O SoundEffect do efeito
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX", meta = (AssetBundles = "SFX"))
	TSoftObjectPtr<USoundCue> SFX;
	
	// O Tipo do efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	EEffectType EffetType;
	
	// As restrições causadas pelo efeito
	UPROPERTY(EditAnywhere, Category="Properties")
	TArray<ERestrictionTypeEnum> RestrictionTypes;
};
