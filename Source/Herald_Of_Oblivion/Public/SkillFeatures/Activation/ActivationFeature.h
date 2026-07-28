// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enumerators/ItemEnums.h"
#include "SkillFeatures/SkillFeature.h"
#include "ActivationFeature.generated.h"

/**
 * UActivationFeature
 * Feature pai de todas as features do tipo activation 
 */

class UAnimSequenceBase;
class UNiagaraComponent;
class UNiagaraSystem;
class USoundCue;

UCLASS(Abstract)
class HERALD_OF_OBLIVION_API UActivationFeature : public USkillFeature
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Transient)
	FTimerHandle PrematureCompleteActivationTimerHandle;
	UPROPERTY(Transient)
	FTimerHandle CastTimerHandle;
	
	// Controla o raio da mira, quanto menor, mais preciso
	UPROPERTY(EditAnywhere, Category="Aim")
	float AimRadius = 5.0f;
	
	// Carrega os FX de forma síncrona
	virtual void LoadFXSync();

	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	void CastOnHands(FSkillContext& InSkillContext, USkillInstance* SkillInstance, AEntityClass* EntityOwner,
	                 TObjectPtr<UNiagaraSystem>* FX, EEquipmentSlot Hand);

	// Retorna um HitResult baseado na visão do jogador
	FHitResult GetAimTarget(FSkillContext& InContext, float InAimRadius) const;
	// Retorna um HitResult baseado no cursor
	FHitResult GetCursorLocation(FSkillContext& InContext) const;
	
	// Limpa o Niagara
	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents) override;

	// Lógica de ativação inicial e final
	virtual void StartActivation(FSkillContext& InSkillContext);	
	virtual void CompleteActivation(FSkillContext& InSkillContext);	
	
	// O Animation Montage de Cast da habilidade
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast", meta = (AssetBundles = "FX"))
	TSoftObjectPtr<UAnimSequenceBase> CastAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	float CastAnimBlendInTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	float CastAnimBlendOutTime = 0.0f;
	// O Slot usado pela animação no AnimGraph
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	FName CastAnimSlotName = FName("FullBodySlot");

	// O Niagara de Cast da habilidade
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast", meta = (AssetBundles = "FX"))
	TSoftObjectPtr<UNiagaraSystem> CastEffect;
	// O Sound de Cast da habilidade
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast", meta = (AssetBundles = "FX"))
	TSoftObjectPtr<USoundCue> CastSound;
	
	// Define se os efeitos de Cast sao anexados a entidade
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	bool bCastOnForwardFollowOwner = false;
	// Define se o cast vai ser feito no socket forward da entidade
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	bool bCastOnForward = false;
	
	
	// Define se os efeitos de Cast sao anexados a entidade
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	bool bCastOnHandsFollowOwner = false;
	// Define que o cast vai ser feito na mao direita
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	bool bCastOnRightHand = false;
	// Define que o cast vai ser feito na mao esquerda
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cast")
	bool bCastOnLeftHand = false;
};

