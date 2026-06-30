// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enumerators/EntityEnums.h"
#include "Enumerators/ItemEnums.h"
#include "Structs/EntityStructs.h"
#include "GameFramework/Character.h"
#include "EntityClass.generated.h"

/**
 * AEntityClass
 * Classe pai de todas as entidades.
 */

class UEquipmentInstance;
class USkillDataAsset;
class UEquipmentDataAsset;
class AEquipmentActor;
class USpecializationDataAsset;
class USkillInstance;
class UEffect;

UCLASS(Blueprintable, BlueprintType)
class HERALD_OF_OBLIVION_API AEntityClass : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEntityClass();

// Métodos
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Die();
	FAttribute GetSimbolicAttribute(EEntitySimbolicAttributeEnum SimbolicAttribute) const {return SimbolicAttributes[SimbolicAttribute];};
	FAttribute GetTrueAttribute(EEntityTrueAttributeEnum TrueAttribute) const {return TrueAttributes[TrueAttribute];};

	AEquipmentActor* GetEquipmentActor(EEquipmentSlot Slot);
	
	void TakeItem();
	
	#if WITH_EDITOR // Compila este código apenas no editor
		// Chamado quando uma propriedade é alterada no editor
		virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	#endif

	
protected:
	// Calcula o retorno de XP esperado ao abater a entidade em questão. O calculo é feito utilizando a raridade da entidade e a diferenca de nivel
	virtual float CalculateXPReturn(AEntityClass* Killer);
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Atribui uma restricao a entidade
	virtual void TakeRestriction(ERestrictionTypeEnum Restriction);
	
	// Define os atributos padrão para todas as criaturas, é sobrescrito pelo método da classe filha
	virtual void DefineAttributes();

	// Inicializam as skills
	void InitializeSkills(TArray<USkillInstance*> SkillInstances);
	void InitializeSkills(USkillInstance* SkillInstance);
	virtual void LoadActivationSkillAssets(USkillInstance* SkillInstance);

	// Define as Skills base para todas as criaturas
	virtual void DefineSkills();

// Propriedades
	
	// Nome da entidade
	UPROPERTY(EditAnywhere, Category="Properties")
	FText Name; // Nome da entidade
	
	// FName da especializacao, usado para filtrar entidades pela especializacao
	UPROPERTY(AssetRegistrySearchable)
	FName SpecializationName;
	
	// A especialização (classe) da entidade 
	UPROPERTY(EditAnywhere, Category="Combat")
	USpecializationDataAsset* Specialization;
	
	// Conquistas de abate da entidade. Exemplo: Fast Kill é uma conquista que pode ser adquirida ao matar a entidade muito rápido
	UPROPERTY(EditAnywhere, Category="Combat")
    TArray<FEntitySlaughterAchievementsStruct> SlaughterAchievements;
	
	// As Conquistas de abate alcançadas pelo jogador
	UPROPERTY(EditAnywhere, Category="Combat")
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
	
	// Os Efeitos afetando a entidade no momento
	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<UEffect*> Effects;
	
	// Um array com os assets das habilidades iniciais
	UPROPERTY(EditAnywhere,  Category="Combat", meta=(AllowedTypes = "Skill"))
	TArray<FPrimaryAssetId> InitialSkillsAssets;
	
	// Array para as instâncias das habilidades.
	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<USkillInstance*> SkillsInstances;
	
	// Os atributos simbolicos da entidade
	UPROPERTY(EditAnywhere, Category="Combat")
	TMap<EEntitySimbolicAttributeEnum, FAttribute> SimbolicAttributes;
	
	// Os atributos verdadeiros da entidade
	UPROPERTY(EditAnywhere, Category="Combat")
	TMap<EEntityTrueAttributeEnum, FAttribute> TrueAttributes; 
	
	// Um tarray dos equipamentos iniciais
	UPROPERTY(EditAnywhere, Category="Equipments", meta=(AllowedTypes = "Equipment"))
	TArray<FPrimaryAssetId> InitialEquipmentsAssets;
	
	// Um tmap das instancias dos equipamentos
	UPROPERTY(EditAnywhere, Category="Equipments")
	TMap<EEquipmentSlot, TObjectPtr<UEquipmentInstance>> EquippedEquipments;
	
	// TArray<TSoftObjectPtr<UItemInstance>> Equipment;
	// TMap<FName, TArray<FPrimaryAssetId>> Sounds;
	// UWidgetComponent StatusWidgetComponent;
	// TSoftObjectPtr<UWidgetComponent> OptionsWidgetComponent;
	// TSoftObjectPtr<UDataAsset> DialogueData;
	
	//O material da superfície onde a entidade está pisando
	UPROPERTY(EditAnywhere, Category="Properties")
	UPhysicalMaterial* SurfaceMaterial;
	
	// As restricoes afetando a entidade no momento
	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<ERestrictionTypeEnum> Restrictions;
};
