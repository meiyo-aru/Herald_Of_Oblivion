// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Engine/EngineTypes.h"
#include "SkillFeatures/SkillFeature.h"
#include "ExecutionFeature.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USoundCue;
class UAnimSequenceBase;
struct FSkillContext;

/**
 * UExecutionFeature
 * Feature pai de todas as features do tipo Execution
 */
UCLASS(Abstract)
class HERALD_OF_OBLIVION_API UExecutionFeature : public USkillFeature
{
	GENERATED_BODY()
	
public:
	FTimerHandle RotateManuallyTimerHandle;
	
	// Animação de execucao
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AssetBundles = "FX"))
	TSoftObjectPtr<UAnimSequenceBase> ExecutionAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float PrimaryExecutionAnimBlendInTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float PrimaryExecutionAnimBlendOutTime = 0.0f;
	// O Slot usado pela animação no AnimGraph
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FName PrimaryExecutionAnimSlotName = FName("FullBodySlot");

	FSkillContext* SkillContext;
	
	// Define se o personagem vai rotacionar para a direção da visão (Geralmente utilizado quando o personagem 
	// precisa estar virado para uma direcao e a própria animação não consegue fazer isso)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bRotateManually;
	
	// Efeito de execução da skill, deve ser compatível com a ExecutionFeature
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara System", meta = (AssetBundles = "FX"))
	TSoftObjectPtr<UNiagaraSystem> ExecutionEffect;
	// Efeito de som
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AssetBundles = "FX"))
	TSoftObjectPtr<USoundCue> ExecutionSound;

	// Carrega os FX de forma síncrona
	virtual void LoadFXSync();

	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) override;
	
	// Execução primária, geralmente contém apenas a lógica da animação de execução
	virtual void PrimaryExecute(FSkillContext& InSkillContext);
	// Execução final, geralmente ouve uma notificacao da animação
	virtual void FinallyExecute(FSkillContext& InSkillContext);
	
	virtual void OnPlayMontageNotifyBegin(FName NotifyName);

	// LImpa o Niagara
	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents) override;

	// Executa alguma lógica nas particulas do niagara
	virtual void ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& InSkillContext);
	UPROPERTY()
	float ParticlesProcessed;
};
