#pragma once

#include "CoreMinimal.h" 
#include "Engine/DamageEvents.h"
#include "Enumerators/EntityEnums.h"
#include "EntityStructs.generated.h"
// Especialização da entidade
UENUM(BlueprintType)
enum class ESpecializationEnum : uint8
{
	Mage		UMETA(DisplayName = "Mage"),
	None        UMETA(DisplayName = "None")
};

// Recursos que podem ser recuperados por um efeito.
UENUM(BlueprintType)
enum class ERecoverableAttributeEnum : uint8
{
	Health		UMETA(DisplayName = "Health"),
	Mana		UMETA(DisplayName = "Mana"),
	Energy		UMETA(DisplayName = "Energy"),
	None        UMETA(DisplayName = "None")
};

// Dados de recuperacao aplicados por efeitos de cura ou restauracao.
USTRUCT(BlueprintType)
struct FRecoverStruct
{
	GENERATED_BODY()
	
public:
	// Atributo recuperado
	UPROPERTY(EditAnywhere)
	ERecoverableAttributeEnum RecoveredAttribute = ERecoverableAttributeEnum::None;
	
	// Valor recuperado
	UPROPERTY(EditAnywhere)
	float RecoveredValue = 0.0f;
	
	// Decide se recupera quem foi atingido pelo efeito
	UPROPERTY(EditAnywhere)
	bool RecoverTarget = false;

	// Decide se recupera quem lançou causou o efeito
	UPROPERTY(EditAnywhere)
	bool RecoverOwner = false;
};


// Dados de dano usados por efeitos e habilidades.
struct FDamageStruct : FPointDamageEvent
{
public:
	// Tipo do dano (Cortante, Perfurante, etc.).
	ETypeDamage TypeDamage = ETypeDamage::None;
	
	FDamageStruct(){};

	explicit FDamageStruct(ETypeDamage InTypeDamage) : TypeDamage(InTypeDamage) {}
	
	/*static bool IsValidTypeDamage(EEntityTrueAttributeEnum InTypeDamage)
	{
		if (InTypeDamage == EEntityTrueAttributeEnum::SlashingDamage ||
			InTypeDamage == EEntityTrueAttributeEnum::PiercingDamage ||
			InTypeDamage == EEntityTrueAttributeEnum::BludgeoningDamage ||
			InTypeDamage == EEntityTrueAttributeEnum::FireDamage ||      
			InTypeDamage == EEntityTrueAttributeEnum::IceDamage ||       
			InTypeDamage == EEntityTrueAttributeEnum::LightningDamage || 
			InTypeDamage == EEntityTrueAttributeEnum::EarthDamage ||     
			InTypeDamage == EEntityTrueAttributeEnum::PoisonDamage ||    
			InTypeDamage == EEntityTrueAttributeEnum::AcidDamage ||      
			InTypeDamage == EEntityTrueAttributeEnum::HolyDamage ||      
			InTypeDamage == EEntityTrueAttributeEnum::ProfaneDamage ||   
			InTypeDamage == EEntityTrueAttributeEnum::LightDamage ||     
			InTypeDamage == EEntityTrueAttributeEnum::ShadowDamage) return true;
		return false;
	}*/
	
	// ID único para identificação
	inline static const int32 ClassID = FCrc::StrCrc32(TEXT("FDamageStruct_Unique_ID"));
	virtual int32 GetTypeID() const override { return ClassID; }
};

// Define como um efeito altera atributos verdadeiros ou simbolicos por valor fixo ou percentual.
USTRUCT(BlueprintType)
struct FAffectAttributeStruct
{
	GENERATED_BODY()
	
public:
	// O atributo verdadeiro afetado
	UPROPERTY(EditAnywhere, meta = (EditCondition = "AffectedSimbolicAttribute == EEntitySimbolicAttributeEnum::None", EditConditionHides))
	EEntityTrueAttributeEnum AffectedTrueAttribute = EEntityTrueAttributeEnum::None;
	
	// O atributo simbólico afetado
	UPROPERTY(EditAnywhere, meta = (EditCondition = "AffectedTrueAttribute == EEntityTrueAttributeEnum::None", EditConditionHides))
	EEntitySimbolicAttributeEnum AffectedSimbolicAttribute = EEntitySimbolicAttributeEnum::None;
	
	// Valor fixo aplicado quando o efeito nao usa percentual.
	UPROPERTY(EditAnywhere,  meta = (EditCondition = "PercentValue == 0", EditConditionHides))
	float FlatValue = 0.0f;

	// Percentual aplicado quando o efeito nao usa valor fixo.
	UPROPERTY(EditAnywhere,  meta = (EditCondition = "FlatValue == 0.0f", EditConditionHides))
	uint8 PercentValue = 0;
	
	// Decide se será um ou não
	UPROPERTY(EditAnywhere)
	bool bIsBuff = false;
	
	// Duração do efeito, 0.0f significa permanente
	UPROPERTY(EditAnywhere)
	float Duration = 0.0;
};

// Guarda uma conquista de abate e o multiplicador aplicado quando ela e alcancada.
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
	
	// Recalcula o multiplicador quando a conquista e alterada no editor.
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


// Guarda a raridade da entidade e o multiplicador usado em recompensas/dificuldade.
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
	// Recalcula o multiplicador quando a raridade e alterada no editor.
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


// Usado para definir os atributos verdadeiros que sao aumentados por um simbolico especifico.
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

// Struct para o atributo, contem propriedades e funcoes relevantes.
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
	UPROPERTY(EditAnywhere, meta=(EditCondition = "AttributeType == EAttributeTypeEnum::SimbolicAttribute", EditConditionHides))
	TArray<FAffectedAttributeStruct> AffectedAttributes; 
	
	// Construtor Padrao
	FAttribute() 
		: Value(0.0f),
		AttributeType(EAttributeTypeEnum::None),
		TrueAttribute(EEntityTrueAttributeEnum::None),
		SimbolicAttribute(EEntitySimbolicAttributeEnum::None){}
	
	// Construtor para iniciliazacao dos atributos com atributo simbolico
	explicit FAttribute(float InValue,
		EAttributeTypeEnum InAttributeType = EAttributeTypeEnum::None,
		EEntityTrueAttributeEnum InTrueAttribute = EEntityTrueAttributeEnum::None)
		: Value(InValue),
		AttributeType(InAttributeType),
		TrueAttribute(InTrueAttribute),
		SimbolicAttribute(EEntitySimbolicAttributeEnum::None),
		AffectedAttributes((TArray<FAffectedAttributeStruct>())){}
	
	// Construtor para iniciliazacao dos atributos com atributo simbolico
	explicit FAttribute(const float InValue,
		const EAttributeTypeEnum InAttributeType = EAttributeTypeEnum::None,
		const EEntitySimbolicAttributeEnum InSimbolicAttribute = EEntitySimbolicAttributeEnum::None,
		const TArray<FAffectedAttributeStruct>& InAffectedAttributes = TArray<FAffectedAttributeStruct>())
		: Value(InValue),
		AttributeType(InAttributeType),
		TrueAttribute(EEntityTrueAttributeEnum::None),
		SimbolicAttribute(InSimbolicAttribute),
		AffectedAttributes(InAffectedAttributes){}
	
	/*// Construtor para iniciliazacao dos atributos com atributo verdadeiro
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
	SimbolicAttribute(EEntitySimbolicAttributeEnum::None) {}*/
	
	// Retorna o valor do atributo
	FORCEINLINE float GetAttributeValue() const
	{
		return this->Value;
	}
	
	// Soma um modificador ao valor atual do atributo.
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
