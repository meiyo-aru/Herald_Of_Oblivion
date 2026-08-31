// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enumerators/ItemEnums.h"
#include "Enumerators/SkillEnums.h"
#include "Structs/SkillStructs.h"
#include "Engine/EngineTypes.h"
#include "Structs/EntityStructs.h"
#include "SkillDataAsset.generated.h"

/**
 * USkillDataAsset
 * DataAsset que guarda todos os dados das habilidades.
 * Uma nova habilidade pode ser facilmente criada usando um data asset.
 * As instâncias utilizam os dados do DataAsset para trabalhar
 */

class UEffectDataAsset;
class UOnHitFeature;
class USpecializationDataAsset;
class ASkillActor;
class UNiagaraComponent;
class USkillInstance;
class USoundCue;
class UNiagaraSystem;
class UActivationFeature;
class UExecutionFeature;

UCLASS(Blueprintable, BlueprintType)
class HERALD_OF_OBLIVION_API USkillDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	USkillDataAsset();
	// void PostEditChangeProperty(FPropertyChangedEvent& e);
	// void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

/*#if WITH_EDITOR // Compila este código apenas no editor
		// Chamado quando uma propriedade é alterada no editor
		virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& e) override;
	#endif*/

	FPrimaryAssetId GetPrimaryAssetId() const {
		return FPrimaryAssetId("Skill", GetFName());
	}
	
	// Recebe uma entidade e cria uma instancia da habilidade para ela
	USkillInstance* CreateInstance(AEntityClass* Owner);

	// Nome da habilidade
	UPROPERTY(EditDefaultsOnly, Category="UI")
	FText Name;
	
	// Raridade da habilidade
	UPROPERTY(EditDefaultsOnly, Category="UI", AssetRegistrySearchable)
	ESkillRarityEnum Rarity = ESkillRarityEnum::None;
	
	// Categoria da habilidade
	UPROPERTY(EditDefaultsOnly, Category="UI", AssetRegistrySearchable)
	ESkillCategoryEnum Category = ESkillCategoryEnum::None;
	
	// Tipo da habilidade
	UPROPERTY(EditDefaultsOnly, Category="UI", AssetRegistrySearchable)
	ESkillTypeEnum Type = ESkillTypeEnum::None;
	
	// Nível mínimo da entidade
	UPROPERTY(EditDefaultsOnly, Category="Properties", AssetRegistrySearchable)
	uint8 MinimumLevel = 0;
	
	// ESkillTagsEnum Tags;
	
	// A especialização(classe) da habilidade, exemplo: Guerreiro, Mago.
	UPROPERTY(EditDefaultsOnly, Category="Properties", AssetRegistrySearchable, meta = (AssetBundle = "UI"))
	ESpecializationEnum Specialization;
	
	// Descrição da habilidade
	UPROPERTY(EditDefaultsOnly, Category="UI")
	FText Description;
	
	// O custo de uso da habilidade.
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	FResourceCost Cost;
	
	// O tempo de recarga da habilidade
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	float Cooldown = 0.0f;
	
	// Tempo de lançamento, as habilidades precisam ser preparadas antes de lançadas (mesmo que não sejam de carga), 
	// cada habilidade pode usar um atributo diferente para reduzir seu tempo de cast
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	float MaxCastTime = 0.0f;

	// Multiplicador do redutor de tempo de lançamento, de 0.0 a 1.0 indica quanto o atributo redutor vai influenciar
	// no casttime. 1.0 significa que 50 de força resultará em uma redução de 50%
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	float MultiplierReduceCastTime = 0.0f;
	
	// Atributo que reduz o tempo de cast da habilidade
	UPROPERTY(EditDefaultsOnly, Category="Properties")
	EEntitySimbolicAttributeEnum ReduceCastTimeAttribute = EEntitySimbolicAttributeEnum::None;
	
	UPROPERTY(EditDefaultsOnly, Category="Properties", AssetRegistrySearchable)
	EWeaponType WeaponType = EWeaponType::None;
	
	// O icone da habilidade
	UPROPERTY(EditDefaultsOnly, Category="UI", meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> Icon;
	
	// Features da skill
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Features")
	TObjectPtr<UActivationFeature> ActivationFeature;
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Features")
	TObjectPtr<UExecutionFeature> ExecutionFeature;
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Features")
	TArray<TObjectPtr<UOnHitFeature>> OnHitFeature;
};
