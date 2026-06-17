// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enumerators/SkillEnums.h"
#include "Structs/SkillStructs.h"
#include "Engine/EngineTypes.h"
#include "SkillDataAsset.generated.h"

/**
 * USkillDataAsset
 * DataAsset que guarda todos os dados das habilidades.
 * Uma nova habilidade pode ser facilmente criada usando um data asset.
 * As instâncias utilizam os dados do DataAsset para trabalhar
 */

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

#if WITH_EDITOR // Compila este código apenas no editor
		// Chamado quando uma propriedade é alterada no editor
		virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& e) override;
	#endif

	FPrimaryAssetId GetPrimaryAssetId() const {
		return FPrimaryAssetId("Skill", GetFName());
	}
	
	UNiagaraComponent* SpawnVFXAtLocation(UWorld* World, UNiagaraSystem* System, const FVector& Location, const FRotator& Rotation) const;

	// Recebe uma entidade e cria uma instancia da habilidade para ela
	USkillInstance* CreateInstance(AEntityClass* Owner);

	// Nome da habilidade
	UPROPERTY(EditAnywhere, Category="UI")
	FText Name;
	
	// Raridade da habilidade
	UPROPERTY(EditAnywhere, Category="UI", AssetRegistrySearchable)
	ESkillRarityEnum Rarity = ESkillRarityEnum::None;
	
	// Categoria da habilidade
	UPROPERTY(EditAnywhere, Category="UI", AssetRegistrySearchable)
	ESkillCategoryEnum Category = ESkillCategoryEnum::None;
	
	// Tipo da habilidade
	UPROPERTY(EditAnywhere, Category="UI", AssetRegistrySearchable)
	ESkillTypeEnum Type =ESkillTypeEnum::None;
	
	// Nível mínimo da entidade
	UPROPERTY(EditAnywhere, Category="Properties", AssetRegistrySearchable)
	uint8 MinimumLevel = 0;
	
	// Booleano se a skill é ou não uma skill inicial, isto é, se o jogador começa com ela
	UPROPERTY(EditAnywhere, Category="Properties", AssetRegistrySearchable)
	bool bIsInitial = false;
	
	// ESkillTagsEnum Tags;
	
	// FName da especializacao, usado para filtrar skills pela especializacao
	UPROPERTY(EditAnywhere, Category="Properties", AssetRegistrySearchable)
	FName SpecializationName;
	
	// A especialização(classe) da habilidade, exemplo: Guerreiro, Mago.
	UPROPERTY(EditAnywhere, Category="Properties", meta = (AssetBundle = "UI"))
	TSoftObjectPtr<USpecializationDataAsset> Specialization;
	
	// Descrição da habilidade
	UPROPERTY(EditAnywhere, Category="UI")
	FText Description;
	
	// Um Array contendo referências para as classes de todos os efeitos causados pela habilidade,
	// por exemplo: dano físico, dano de fogo, queimadura, congelamento, etc.
	// UPROPERTY(EditAnywhere, Category="Skill Properties")
	// TArray<TSoftObjectPtr<UEffect>> Effects;
	
	// O custo de uso da habilidade.
	UPROPERTY(EditAnywhere, Category="Properties")
	FResourceCost Cost;
	
	
	// O tempo de recarga da habilidade
	UPROPERTY(EditAnywhere, Category="Properties")
	float Cooldown;
	
	// O icone da habilidade
	UPROPERTY(EditAnywhere, Category="UI", meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> Icon;

	// Features da skill
	UPROPERTY(EditAnywhere, Instanced, Category = "Features")
	UActivationFeature* ActivationFeature;
	UPROPERTY(EditAnywhere, Instanced, Category = "Features")
	UExecutionFeature* ExecutionFeature;
	UPROPERTY(EditAnywhere, Instanced, Category = "Features")
	UOnHitFeature* OnHitFeature;

};
