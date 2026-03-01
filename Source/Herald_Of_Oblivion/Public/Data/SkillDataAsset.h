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
	UPROPERTY(EditAnywhere, Category="UI", AssetRegistrySearchable)
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
	
	// // O tempo de espera para a habilidade ser lançada em segundos, logo depois de castada
	// UPROPERTY(EditAnywhere, Category="Skill Properties")
	// float CastTime = 1.0f;
	
	// O tempo de recarga da habilidade
	UPROPERTY(EditAnywhere, Category="Properties")
	float Cooldown;
	
	// O icone da habilidade
	UPROPERTY(EditAnywhere, Category="UI", meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> Icon;

	// Define as propriedades de colisão
	UPROPERTY(EditAnywhere, Category="Collision")
	float RadiusCollision = 0.0f;
	UPROPERTY(EditAnywhere, Category="Collision")
	bool bGenerateOverlapEvents = false;
	UPROPERTY(EditAnywhere, Category="Collision")
	bool bGenerateHitEvents = false;
	UPROPERTY(EditAnywhere, Category="Collision")
	bool bNotifyRigidBodyCollision = false;
	UPROPERTY(EditAnywhere, Category="Collision")
	TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled = ECollisionEnabled::NoCollision;
	UPROPERTY(EditAnywhere, Category="Collision")
	TEnumAsByte<ECollisionResponse> PawnCollision = ECR_Ignore;
	UPROPERTY(EditAnywhere, Category="Collision")
	TEnumAsByte<ECollisionResponse> WorldStaticCollision = ECR_Ignore;
	UPROPERTY(EditAnywhere, Category="Collision")
	TEnumAsByte<ECollisionResponse> WorldDynamicCollision = ECR_Ignore;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX", meta = (AssetBundles = "SkeletalMeshAuraVFX"))
	TSoftObjectPtr<UNiagaraSystem> SkeletalMeshAuraEffect;      // Efeito de aura do skeletalmesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX", meta = (AssetBundles = "StaticMeshAuraVFX"))
	TSoftObjectPtr<UNiagaraSystem> StaticMeshAuraEffect;      // Efeito de aura do staticmesh

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	bool bAuraInStaticMesh = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	bool bAuraInSkeletalMesh = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	bool bAuraInCast = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	bool bAuraWhenFinish = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
    float SpawnRateAura = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	float NormalOffsetAura = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	float MinLifeTimeAura = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	float MaxLifeTimeAura = 0.0f;
	
	// Skill castada na mão esquerda ou direita, ou ambas
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	bool bLeftHand = false; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	bool bRightHand = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX", meta = (AssetBundles = "CastVFX"))
	TSoftObjectPtr<UNiagaraSystem> CastEffect;  // Efeito de cast
	UPROPERTY(EditAnywhere, Category = "VFX")
	float MaxCastTime = 0.0f;
	UPROPERTY(EditAnywhere, Category = "VFX")
	float MinCastTime = 0.0f;
	UPROPERTY(EditAnywhere, Category = "VFX")
	bool bCastOnTarget = false;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX", meta = (AssetBundles = "PathVFX"))
	TSoftObjectPtr<UNiagaraSystem> PathEffect; // Efeito de trajeto 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX", meta = (AssetBundles = "ExecutionVFX"))
	TSoftObjectPtr<UNiagaraSystem> ExecutionEffect;    // Efeito de execução
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX", meta = (AssetBundles = "PersistentVFX"))
	TSoftObjectPtr<UNiagaraSystem> PersistentEffect; // Efeito contínuo (DOT, buff)
	
	// Os SoundEffect da habilidades
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX", meta = (AssetBundles = "CastSFX"))
	TSoftObjectPtr<USoundCue> CastSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX", meta = (AssetBundles = "PathSFX"))
	TSoftObjectPtr<USoundCue> PathSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX", meta = (AssetBundles = "ExecutionSFX"))
	TSoftObjectPtr<USoundCue> ExecutionSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX", meta = (AssetBundles = "PersistentSFX"))
	TSoftObjectPtr<USoundCue> PersistentSound;
	
	// Features da skill
	UPROPERTY(EditAnywhere, Instanced, Category = "Logic")
	UExecutionFeature* ExecutionFeature;
	UPROPERTY(EditAnywhere, Instanced, Category = "Logic")
	UActivationFeature* ActivationFeature;
	UPROPERTY(EditAnywhere, Instanced, Category = "Logic")
	UOnHitFeature* OnHitFeature;

};
