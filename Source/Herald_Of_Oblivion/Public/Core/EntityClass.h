// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enumerators/EntityEnums.h"
#include "Enumerators/ItemEnums.h"
#include "Structs/EntityStructs.h"
#include "GameFramework/Character.h"
#include "Structs/SkillStructs.h"
#include "EntityClass.generated.h"

/**
 * AEntityClass
 * Classe pai de todas as entidades.
 */

class UAnimationDataAsset;
// Forward declarations para reduzir includes no header.
class UEquipmentInstance;
class USkillDataAsset;
class UEquipmentDataAsset;
class AEquipmentActor;
class USkillInstance;
class UEffectInstance;
class UEntityDataAsset;
class UAnimInstance;

// Delegate para notificar mudanças de vida
DECLARE_DELEGATE_OneParam(FOnHealthChangedSignature, float);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnimationNotify, FName);


UCLASS(Blueprintable, BlueprintType)
class HERALD_OF_OBLIVION_API AEntityClass : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEntityClass();
	
	// Delegates
	FOnHealthChangedSignature OnHealthChanged;
	FOnAnimationNotify OnAnimationNotify;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Executa a logica de morte da entidade.
	virtual void Die();
	
	// Apenas remove o efeito do array de efeitos ativos da entidade
	void RemoveEffect(FPrimaryAssetId EffectId);
	
	// Retorna o atributo simbolico.
	FAttribute* GetSimbolicAttribute(EEntitySimbolicAttributeEnum SimbolicAttribute);
	// Retorna o atributo verdadeiro.
	FAttribute* GetTrueAttribute(EEntityTrueAttributeEnum TrueAttribute);
	// Retorna a quantidade de certo tipo de efeitos ativos
	int8 GetAmountActiveEffects(FPrimaryAssetId EffectId);
	
	// Retorna o Level
	int8 GetLevel() const {return Level;};
	
	void ApplyEffect(UEffectInstance* Effect, FHitOverlapResult& HitOverlapResult);
	void ApplyEffect(FPrimaryAssetId EffectId);
	
	UFUNCTION(BlueprintCallable)
	UAnimSequence* GetRightArmAnimation(FName AnimationName);
	UFUNCTION(BlueprintCallable)
	UAnimSequence* GetLeftArmAnimation(FName AnimationName);

	// Equipa o equipamento
	void EquipEquipment(UEquipmentInstance* Equipment);

	// Busca o actor fisico de um equipamento equipado em um slot.
	AEquipmentActor* GetEquipmentActor(EEquipmentSlot Slot);

	/*
	#if WITH_EDITOR // Compila este código apenas no editor
		// Chamado quando uma propriedade é alterada no editor
		virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	#endif
	*/
	
	// Carrega os Assets na memória - Apenas os assets necessários imediatamente ao Castar a skill
	virtual void LoadSkillAssets(USkillInstance* SkillInstance, bool bAsync);

	// Recebe dano, funcao sobrescrita da classe pai
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	// Atribui uma restricao a entidade
	virtual void TakeRestriction(ERestrictionTypeEnum Restriction);
	
	FAttribute* GetEquivalentResistanceAttribute(ETypeDamage InTypeDamage);
	
	// AnimationBlueprint da entidade
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Animation")
	TObjectPtr<UAnimationDataAsset> AnimationDataAsset;
	
	// Define se a entidade está andando
	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category="Movement")
	bool bIsWalking;
	// Define se a entidade está Correndo
	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category="Movement")
	bool bIsRunning;
	// Define se a entidade está se movendo
	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category="Movement")
	bool bIsMoving; 
	
	// Velocidade de movimento para frente
	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category="Movement")
	float ForwardMoveSpeed;
	// Velocidade de movimento para os lados
	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category="Movement")
	float RightMoveSpeed;
	
	// Velocidade máxima de movimento andando
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Movement")
	float MaxWalkMoveSpeed = 200.0f;
	// Velocidade máxima de movimento correndo
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Movement")
	float MaxRunMoveSpeed = 500.0f;

protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsTurningInPlace;
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsHardTurningInPlace;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FRotator TargetRotation;
	
	// Define os atributos padrão para todas as criaturas, é sobrescrito pelo método da classe filha
	virtual void DefineAttributes();
	
	// Nível da entidade
	UPROPERTY(EditAnywhere, Category="Properties")
	uint8 Level = 1; 
	
	// Pontos de experiencia da entidade
	UPROPERTY(EditAnywhere, Category="Properties")
	uint32 XP = 1000;

	// As Bençãos Concedidas à entidade;
	// UPROPERTY(EditAnywhere, Category="Properties")
	// TArray<EBlessingEnum> Blessings;
	
	// Os efeitos afetando a entidade no momento.
	UPROPERTY(VisibleAnywhere, Category="FX")
	TMap<FPrimaryAssetId, TWeakObjectPtr<UEffectInstance>> Effects;
	
	// Array para as instâncias das habilidades.
	UPROPERTY(VisibleAnywhere, Category="FX")
	TArray<TObjectPtr<USkillInstance>> SkillsInstances;
	
	// Os atributos simbolicos da entidade
	UPROPERTY(EditAnywhere, Category="FX")
	TMap<EEntitySimbolicAttributeEnum, FAttribute> SimbolicAttributes;
	
	// Os atributos verdadeiros da entidade
	UPROPERTY(VisibleAnywhere, Category="FX")
	TMap<EEntityTrueAttributeEnum, FAttribute> TrueAttributes; 
	
	// Um tmap das instancias dos equipamentos
	UPROPERTY(VisibleAnywhere, Category="Equipments")
	TMap<EEquipmentSlot, TWeakObjectPtr<UEquipmentInstance>> EquippedEquipments;
	
	// TArray<TSoftObjectPtr<UItemInstance>> Equipment;
	// TMap<FName, TArray<FPrimaryAssetId>> Sounds;
	// UWidgetComponent StatusWidgetComponent;
	// TSoftObjectPtr<UWidgetComponent> OptionsWidgetComponent;
	// TSoftObjectPtr<UDataAsset> DialogueData;
	
	// O material da superficie onde a entidade esta pisando.
	UPROPERTY(Transient, VisibleAnywhere, Category="Properties")
	TObjectPtr<UPhysicalMaterial> SurfaceMaterial;
	
	// As restricoes afetando a entidade no momento
	UPROPERTY(Transient, VisibleAnywhere, Category="FX")
	TArray<ERestrictionTypeEnum> Restrictions;
};
