#pragma once

#include "CoreMinimal.h" 
#include "Enumerators/EntityEnums.h"
#include "EntityStructs.generated.h"

USTRUCT(BlueprintType)
struct FEntitySlaughterAchievementsStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EEntitySlaughterAchievementsEnum Achievements = EEntitySlaughterAchievementsEnum::None;
	UPROPERTY(EditAnywhere)
	float AchievementsModifier = 0.0f;
	FEntitySlaughterAchievementsStruct() = default;
	explicit FEntitySlaughterAchievementsStruct(EEntitySlaughterAchievementsEnum InAchievements)
	{
		Achievements = InAchievements;
		switch (this->Achievements)
		{
		case EEntitySlaughterAchievementsEnum::NoDamage:
			this->AchievementsModifier = 2.0f;
			break;
		case EEntitySlaughterAchievementsEnum::FastKill:
			this->AchievementsModifier = 1.5f;
			break;
		case EEntitySlaughterAchievementsEnum::NumericalDisadvantage:
			this->AchievementsModifier = 3.0f;
			break;
		default:
			this->AchievementsModifier = 0.0f;
			break;
		}
	}
	
	void UpdateModifier()
	{
		switch (this->Achievements)
		{
		case EEntitySlaughterAchievementsEnum::NoDamage:
			this->AchievementsModifier = 2.0f;
			break;
		case EEntitySlaughterAchievementsEnum::FastKill:
			this->AchievementsModifier = 1.5f;
			break;
		case EEntitySlaughterAchievementsEnum::NumericalDisadvantage:
			this->AchievementsModifier = 3.0f;
			break;
		default:
			this->AchievementsModifier = 0.0f;
			break;
		}
	}
};


USTRUCT(BlueprintType)
struct FEntityRarityStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EEntityRarityEnum Rarity = EEntityRarityEnum::None;
	
	UPROPERTY(EditAnywhere)
	float RarityModifier = 0.0f;
	FEntityRarityStruct() = default;
	explicit FEntityRarityStruct(EEntityRarityEnum InRarity)
	{
		Rarity = InRarity;
		switch (Rarity)
		{
		case EEntityRarityEnum::Minion:
			this->RarityModifier = 1.0f;
			break;
		case EEntityRarityEnum::Common:
			this->RarityModifier = 1.05f;
			break;
		case EEntityRarityEnum::Rare:
			this->RarityModifier = 1.2f;
			break;
		case EEntityRarityEnum::Unique:
			this->RarityModifier = 1.5f;
			break;
		case EEntityRarityEnum::MiniBoss:
			this->RarityModifier = 2.0f;
			break;
		case EEntityRarityEnum::Boss:
			this->RarityModifier = 3.5f;
			break;
		default:
			this->RarityModifier = 0.0f;
			break;
		}
	}
	void UpdateModifier()
	{
		switch (Rarity)
		{
		case EEntityRarityEnum::Minion:
			this->RarityModifier = 1.0f;
			break;
		case EEntityRarityEnum::Common:
			this->RarityModifier = 1.05f;
			break;
		case EEntityRarityEnum::Rare:
			this->RarityModifier = 1.2f;
			break;
		case EEntityRarityEnum::Unique:
			this->RarityModifier = 1.5f;
			break;
		case EEntityRarityEnum::MiniBoss:
			this->RarityModifier = 2.0f;
			break;
		case EEntityRarityEnum::Boss:
			this->RarityModifier = 3.5f;
			break;
		default:
			this->RarityModifier = 0.0f;
			break;
		}	}

};


// Usado para definir os atributos verdadeiros que são aumentados por um simbólico específico
USTRUCT(BlueprintType)
struct FAffectedAttributeStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EEntityTrueAttributeEnum AffectedAttribute;

	UPROPERTY(EditAnywhere)
	float Multiplier;

	FAffectedAttributeStruct() : AffectedAttribute(EEntityTrueAttributeEnum::None), Multiplier(0.0f) {}
	FAffectedAttributeStruct(EEntityTrueAttributeEnum InC, float InM) : AffectedAttribute(InC), Multiplier(InM) {}
	
	float GetMultiplier() const { return Multiplier; }
	
	EEntityTrueAttributeEnum GetAffectedAttribute() const { return AffectedAttribute; }
};

// Struct para o atributo, contem propriedades e funcoes relevantes
USTRUCT(BlueprintType)
struct FAttribute
{
	GENERATED_BODY()
public:
	// O valor do atributo, por exemplo: 105 de Dano cortante
	UPROPERTY(EditAnywhere)
	float Value; 
	
	// O tipo do atributo: Simbolico ou Verdadeiro
	UPROPERTY(EditAnywhere)
	EAttributeTypeEnum AttributeType; 
	
	// O atributo verdadeiro em si
	UPROPERTY(EditAnywhere, meta = (EditCondition = "AttributeType == EAttributeTypeEnum::TrueAttribute", EditConditionHides))
	EEntityTrueAttributeEnum TrueAttribute; 
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "AttributeType == EAttributeTypeEnum::SimbolicAttribute", EditConditionHides))
	EEntitySimbolicAttributeEnum SimbolicAttribute; 
	
	// Quais atributos verdadeiros serao contribuidos.
	UPROPERTY(EditAnywhere)
	TArray<FAffectedAttributeStruct> AffectedAttributes; 
	
	// Construtor Padrao
	FAttribute() 
		: Value(0.0f),
		AttributeType(EAttributeTypeEnum::None),
		TrueAttribute(EEntityTrueAttributeEnum::None),
		SimbolicAttribute(EEntitySimbolicAttributeEnum::None){}
	
	// Construtor para iniciliazacao dos atributos com atributo simbolico
	FAttribute(float InValue, EAttributeTypeEnum InAttributeType, EEntitySimbolicAttributeEnum InSimbolicAttribute, TArray<FAffectedAttributeStruct> InAffectedAttributes)
		: Value(InValue),
		AttributeType(InAttributeType),
		TrueAttribute(EEntityTrueAttributeEnum::None),
		SimbolicAttribute(InSimbolicAttribute),
		AffectedAttributes(InAffectedAttributes){}
	
	// Construtor para iniciliazacao dos atributos com atributo verdadeiro
	FAttribute(float InValue, EAttributeTypeEnum InAttributeType, EEntityTrueAttributeEnum InTrueAttribute, TArray<FAffectedAttributeStruct> InAffectedAttributes)
		: Value(InValue),
		AttributeType(InAttributeType),
		TrueAttribute(InTrueAttribute),
		SimbolicAttribute(EEntitySimbolicAttributeEnum::None),
		AffectedAttributes(InAffectedAttributes){}
	
	// Construtor para inicializacao sem contribuidores com atributo simbolico
	FAttribute(float InValue, EAttributeTypeEnum InAttributeType, EEntitySimbolicAttributeEnum InSimbolicAttribute)
	: Value(InValue),
	AttributeType(InAttributeType),
	TrueAttribute(EEntityTrueAttributeEnum::None),
	SimbolicAttribute(InSimbolicAttribute) {}
	
	// Construtor para inicializacao sem contribuidores com atributo verdadeiro
	FAttribute(float InValue, EAttributeTypeEnum InAttributeType, EEntityTrueAttributeEnum InTrueAttribute)
	: Value(InValue),
	AttributeType(InAttributeType),
	TrueAttribute(InTrueAttribute),
	SimbolicAttribute(EEntitySimbolicAttributeEnum::None) {}
	
	// Retorna o valor do atributo
	FORCEINLINE float GetAttributeValue() const
	{
		return this->Value;
	}
	
	FORCEINLINE void UpdateAttributeValue(float InValue)
	{
		this->Value += InValue;
	}
	
	// Retorna os Atributos que são afetados
	TArray<FAffectedAttributeStruct> GetAffectedAttributes()
	{
		return this->AffectedAttributes;
	}
};