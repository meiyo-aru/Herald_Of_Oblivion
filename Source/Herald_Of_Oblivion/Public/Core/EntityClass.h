// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enumerators/EntityEnums.h"
#include "Structs/EntityStructs.h"
#include "GameFramework/Character.h"
#include "EntityClass.generated.h"

/**
 * AEntityClass
 * Classe pai de todas as entidades.
 */

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
	UStaticMeshComponent* GetRightEquippedWeapon() const {return RightEquippedWeapon;};
	UStaticMeshComponent* GetLeftEquippedWeapon() const {return LeftEquippedWeapon;};

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
	
	// Método chamado quando todas as skills forem carregadas
	virtual void OnAllSkillsLoaded(TArray<FPrimaryAssetId> LoadedIds);
	
	// Inicializam as skills
	void InitializeSkills(TArray<USkillInstance*> SkillInstances);
	void InitializeSkills(USkillInstance* SkillInstance);
	
	// Define as Skills base para todas as criaturas
	virtual void DefineSkills();

// Propriedades
	
	// Nome da entidade
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Info")
	FText Name; // Nome da entidade
	
	// FName da especializacao, usado para filtrar entidades pela especializacao
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Info", AssetRegistrySearchable)
	FName SpecializationName;
	
	// A especialização (classe) da entidade 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Properties")
	USpecializationDataAsset* Specialization;
	
	// Conquistas de abate da entidade. Exemplo: Fast Kill é uma conquista que pode ser adquirida ao matar a entidade muito rápido
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Info")
    TArray<FEntitySlaughterAchievementsStruct> SlaughterAchievements;
	
	// As Conquistas de abate alcançadas pelo jogador
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Info")
	TArray<EEntitySlaughterAchievementsEnum> SlaughterAchievementsReached;
	
	// Raca da entidade, Ex: Elfo, Humano
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Info")
	EEntityRaceEnum Race = EEntityRaceEnum::None;
	
	// Nível da entidade
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Properties")
	uint8 Level = 1; 
	
	// Pontos de experiencia da entidade
	UPROPERTY(EditAnywhere, Category="Properties")
	uint32 XP = 1000;
	
	// Raridade da entidade
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Info")
	FEntityRarityStruct Rarity;
	
	// A divindade adorada pela entidade
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Info")
	EEntityDivinityEnum Divinity = EEntityDivinityEnum::None; 
	
	// As Bençãos Concedidas à entidade;
	// UPROPERTY(EditAnywhere, Category="Properties")
	// TArray<EBlessingEnum> Blessings;
	
	// Os Efeitos afetando a entidade no momento
	UPROPERTY(EditAnywhere, Category="Properties")
	TArray<UEffect*> Effects;
	
	// Array para as instâncias das habilidades.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Properties")
	TArray<USkillInstance*> SkillsInstances;
	
	// Os atributos simbolicos da entidade
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Properties")
	TMap<EEntitySimbolicAttributeEnum, FAttribute> SimbolicAttributes;
	
	// Os atributos verdadeiros da entidade
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Properties")
	TMap<EEntityTrueAttributeEnum, FAttribute> TrueAttributes; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Properties")
	UStaticMeshComponent* RightEquippedWeapon;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Properties")
	UStaticMeshComponent* LeftEquippedWeapon;
	
	// TArray<TSoftObjectPtr<UItemInstance>> Equipment;
	// TMap<FName, TArray<FPrimaryAssetId>> Sounds;
	// UWidgetComponent StatusWidgetComponent;
	// TSoftObjectPtr<UWidgetComponent> OptionsWidgetComponent;
	// TSoftObjectPtr<UDataAsset> DialogueData;
	
	//O material da superfície onde a entidade está pisando
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Properties")
	UPhysicalMaterial* SurfaceMaterial;
	
	// As restricoes afetando a entidade no momento
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Properties")
	TArray<ERestrictionTypeEnum> Restrictions;
};
