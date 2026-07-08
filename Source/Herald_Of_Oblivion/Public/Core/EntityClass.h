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

// Forward declarations para reduzir includes no header.
class UEquipmentInstance;
class USkillDataAsset;
class UEquipmentDataAsset;
class AEquipmentActor;
class USpecializationDataAsset;
class USkillInstance;
class UEffectInstance;

// Delegate para notificar mudanças de vida
DECLARE_DELEGATE_OneParam(FOnHealthChangedSignature, float);

UCLASS(Blueprintable, BlueprintType)
class HERALD_OF_OBLIVION_API AEntityClass : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEntityClass();
	
	FOnHealthChangedSignature OnHealthChanged;

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

	FPrimaryAssetId GetSpecializationId() const {return Specialization;};
	
	int8 GetAmountActiveEffects(FPrimaryAssetId EffectId);
	void ApplyEffect(UEffectInstance* Effect, FHitOverlapResult& HitOverlapResult);
	void ApplyEffect(FPrimaryAssetId EffectId);
	
	// Equipa o equipamento
	void EquipEquipment(UEquipmentInstance* Equipment);

	// Busca o actor fisico de um equipamento equipado em um slot.
	AEquipmentActor* GetEquipmentActor(EEquipmentSlot Slot);

	#if WITH_EDITOR // Compila este código apenas no editor
		// Chamado quando uma propriedade é alterada no editor
		virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	#endif
	
	// Carrega os Assets na memória - Apenas os assets necessários imediatamente ao Castar a skill
	virtual void LoadSkillAssets(USkillInstance* SkillInstance, bool bAsync);

	// Recebe dano, funcao sobrescrita da classe pai
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	// Atribui uma restricao a entidade
	virtual void TakeRestriction(ERestrictionTypeEnum Restriction);
	
	FAttribute* GetEquivalentResistanceAttribute(ETypeDamage InTypeDamage);
	
protected:
	// Calcula o retorno de XP esperado ao abater a entidade em questão. O calculo é feito utilizando a raridade da entidade e a diferenca de nivel
	virtual float CalculateXPReturn(AEntityClass* Killer);
	
	// Define os atributos padrão para todas as criaturas, é sobrescrito pelo método da classe filha
	virtual void DefineAttributes();
	
	// Nome da entidade
	UPROPERTY(EditAnywhere, Category="Properties")
	FText Name; // Nome da entidade
	
	// FName da especializacao, usado para filtrar entidades pela especializacao
	UPROPERTY(AssetRegistrySearchable)
	FName SpecializationName;
	
	// A especialização (classe) da entidade 
	UPROPERTY(EditAnywhere, Category="Properties", meta=(AllowedTypes="Specialization"))
	FPrimaryAssetId Specialization;
	
	// Conquistas de abate da entidade. Exemplo: Fast Kill é uma conquista que pode ser adquirida ao matar a entidade muito rápido
	UPROPERTY(EditAnywhere, Category="FX")
    TArray<FEntitySlaughterAchievementsStruct> SlaughterAchievements;
	
	// As Conquistas de abate alcançadas pelo jogador
	UPROPERTY(EditAnywhere, Category="FX")
	TArray<EEntitySlaughterAchievementsEnum> SlaughterAchievementsReached;
	
	// Raca da entidade, Ex: Elfo, Humano
	UPROPERTY(EditAnywhere, Category="Properties")
	EEntityRaceEnum Race = EEntityRaceEnum::None;
	
	// Nível da entidade
	UPROPERTY(EditAnywhere, Category="Properties")
	uint8 Level = 1; 
	
	// Pontos de experiencia da entidade
	UPROPERTY(EditAnywhere, Category="Properties")
	uint32 XP = 1000;
	
	// Raridade da entidade
	UPROPERTY(EditAnywhere, Category="Properties")
	FEntityRarityStruct Rarity = FEntityRarityStruct(EEntityRarityEnum::None);
	
	// A divindade adorada pela entidade
	UPROPERTY(EditAnywhere, Category="Properties")
	EEntityDivinityEnum Divinity = EEntityDivinityEnum::None; 
	
	// As Bençãos Concedidas à entidade;
	// UPROPERTY(EditAnywhere, Category="Properties")
	// TArray<EBlessingEnum> Blessings;
	
	// Os efeitos afetando a entidade no momento.
	UPROPERTY(EditAnywhere, Category="FX")
	TMap<FPrimaryAssetId, TWeakObjectPtr<UEffectInstance>> Effects;
	
	// Array para as instâncias das habilidades.
	UPROPERTY(EditAnywhere, Category="FX")
	TArray<TObjectPtr<USkillInstance>> SkillsInstances;
	
	// Os atributos simbolicos da entidade
	UPROPERTY(EditAnywhere, Category="FX")
	TMap<EEntitySimbolicAttributeEnum, FAttribute> SimbolicAttributes;
	
	// Os atributos verdadeiros da entidade
	UPROPERTY(EditAnywhere, Category="FX")
	TMap<EEntityTrueAttributeEnum, FAttribute> TrueAttributes; 
	
	// Um tmap das instancias dos equipamentos
	UPROPERTY(EditAnywhere, Category="Equipments")
	TMap<EEquipmentSlot, TWeakObjectPtr<UEquipmentInstance>> EquippedEquipments;
	
	// TArray<TSoftObjectPtr<UItemInstance>> Equipment;
	// TMap<FName, TArray<FPrimaryAssetId>> Sounds;
	// UWidgetComponent StatusWidgetComponent;
	// TSoftObjectPtr<UWidgetComponent> OptionsWidgetComponent;
	// TSoftObjectPtr<UDataAsset> DialogueData;
	
	// O material da superficie onde a entidade esta pisando.
	UPROPERTY(EditAnywhere, Category="Properties")
	UPhysicalMaterial* SurfaceMaterial;
	
	// As restricoes afetando a entidade no momento
	UPROPERTY(EditAnywhere, Category="FX")
	TArray<ERestrictionTypeEnum> Restrictions;
};
