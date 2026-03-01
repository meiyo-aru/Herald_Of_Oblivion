#pragma once

#include "CoreMinimal.h" 
#include "EntityEnums.generated.h"

UENUM(BlueprintType)
enum class EEntitySpecializationEnum : uint8
{
	Mage				UMETA(DisplayName = "Mage"),
	Warrior				UMETA(DisplayName = "Warrior"),
	None				UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class EEntitySlaughterAchievementsEnum : uint8
{
	NoDamage				UMETA(DisplayName = "No Damage"),
	FastKill				UMETA(DisplayName = "Fast Kill"),
	NumericalDisadvantage   UMETA(DisplayName = "Numerical Disadvantage"),
	None					UMETA(DisplayName = "None")
};

// UENUM(BlueprintType)
// enum class ECombatFeatEnum : uint8
// {
// 	None					UMETA(DisplayName = "None")
// };

UENUM(BlueprintType)
enum class EEntityRaceEnum : uint8
{
	Elf			UMETA(DisplayName = "Elf"),
	Human		UMETA(DisplayName = "Human"),
	None		UMETA(DisplayName = "None")
};

// Enumerator para a raridade da entidade
UENUM(BlueprintType)
enum class EEntityRarityEnum : uint8
{
	Minion		UMETA(DisplayName = "Minion"),
	Common		UMETA(DisplayName = "Common"),
	Rare		UMETA(DisplayName = "Rare"),
	Unique		UMETA(DisplayName = "Unique"),
	MiniBoss	UMETA(DisplayName = "Mini-Boss"),
	Boss		UMETA(DisplayName = "Boss"),
	None		UMETA(DisplayName = "None")
};


// Enumerator para a divindade adorada pela entidade
UENUM(BlueprintType)
enum class EEntityDivinityEnum : uint8
{
	Sophia  UMETA(DisplayName = "Sophia"),
	None	UMETA(DisplayName = "None")
};

// Enumerator para as restrições afetando a entidade
UENUM(BlueprintType)
enum class ERestrictionTypeEnum : uint8
{
	Movement    UMETA(DisplayName = "Movement"),
	Magic       UMETA(DisplayName = "Magic"),
	Technique   UMETA(DisplayName = "Technique"),
	Heal        UMETA(DisplayName = "Heal"),
	Shield      UMETA(DisplayName = "Shield"),
	Inventory   UMETA(DisplayName = "Inventory"),
	Map         UMETA(DisplayName = "Map"),
	None		UMETA(DisplayName = "None")
};


// Enumerator para o tipo do atributo, simbolico ou verdadeiro
UENUM(BlueprintType)
enum class EAttributeTypeEnum : uint8
{
	SimbolicAttribute		UMETA(DisplayName = "Simbolic Attribute"),
	TrueAttribute			UMETA(DisplayName = "True Attribute"),
	None					UMETA(DisplayName = "None")
};

// Enumerator para o atributo simbolico em si
UENUM(BlueprintType)
enum class EEntitySimbolicAttributeEnum : uint8
{
	// --- Simbolic Attributes ---
	Strength		UMETA(DisplayName = "Strength"),
	Dexterity       UMETA(DisplayName = "Dexterity"),
	Vitality		UMETA(DisplayName = "Vitality"),
	Intelligence	UMETA(DisplayName = "Intelligence"),
	Constitution    UMETA(DisplayName = "Constitution"),
	None			UMETA(DisplayName = "None")
};

// Enumerator para o atributo verdadeiro em si
UENUM(BlueprintType)
enum class EEntityTrueAttributeEnum : uint8
{
	// --- Offensive Stats ---
    SlashingDamage      UMETA(DisplayName = "Slashing Damage"),
    PiercingDamage      UMETA(DisplayName = "Piercing Damage"),
    BludgeoningDamage   UMETA(DisplayName = "Bludgeoning Damage"),
    FireDamage          UMETA(DisplayName = "Fire Damage"),
    IceDamage           UMETA(DisplayName = "Ice Damage"),
    LightningDamage     UMETA(DisplayName = "Lightning Damage"),
    EarthDamage         UMETA(DisplayName = "Earth Damage"),
    PoisonDamage        UMETA(DisplayName = "Poison Damage"),
    AcidDamage          UMETA(DisplayName = "Acid Damage"),
    HolyDamage          UMETA(DisplayName = "Holy Damage"),
    ProfaneDamage       UMETA(DisplayName = "Profane Damage"),
    LightDamage         UMETA(DisplayName = "Light Damage"),
    ShadowDamage        UMETA(DisplayName = "Shadow Damage"),
    CriticalDamage      UMETA(DisplayName = "Critical Damage"),
    CriticalChance      UMETA(DisplayName = "Critical Chance"),
    LifeSteal           UMETA(DisplayName = "Life Steal"),

    // --- Defensive Stats ---
    SlashingResist      UMETA(DisplayName = "Slashing Resistance"),
    PiercingResist      UMETA(DisplayName = "Piercing Resistance"),
    BludgeoningResist   UMETA(DisplayName = "Bludgeoning Resistance"),
    FireResist          UMETA(DisplayName = "Fire Resistance"),
    IceResist           UMETA(DisplayName = "Ice Resistance"),
    LightningResist     UMETA(DisplayName = "Lightning Resistance"),
    EarthResist         UMETA(DisplayName = "Earth Resistance"),
    PoisonResist        UMETA(DisplayName = "Poison Resistance"),
    AcidResist          UMETA(DisplayName = "Acid Resistance"),
    HolyResist          UMETA(DisplayName = "Holy Resistance"),
    ProfaneResist       UMETA(DisplayName = "Profane Resistance"),
    ShadowResist        UMETA(DisplayName = "Shadow Resistance"),
    LightResist         UMETA(DisplayName = "Light Resistance"),
    Tenacity            UMETA(DisplayName = "Tenacity"),

    // --- Vital Stats ---
    Health              UMETA(DisplayName = "Health"),
    MaxHealth           UMETA(DisplayName = "Max Health"),
    Mana                UMETA(DisplayName = "Mana"),
    MaxMana             UMETA(DisplayName = "Max Mana"),
    Energy              UMETA(DisplayName = "Energy"),
    MaxEnergy           UMETA(DisplayName = "Max Energy"),
	FatigueReduction	UMETA(DisplayName = "Fatigue Reduction"),
	
    // --- Utility & Speed ---
    MovementSpeed       UMETA(DisplayName = "Movement Speed"),
    AttackSpeed         UMETA(DisplayName = "Attack Speed"),
    AbilitySpeed        UMETA(DisplayName = "Ability Speed"),
	
	None			UMETA(DisplayName = "None")
};

