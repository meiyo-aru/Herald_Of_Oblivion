#pragma once

#include "CoreMinimal.h" 
#include "SkillEnums.generated.h"

// Raridade da habilidade, usada para organizar e balancear skills.
UENUM(BlueprintType)
enum class ESkillRarityEnum : uint8
{
	Unique		UMETA(DisplayName = "Unique"),
	Normal		UMETA(DisplayName = "Normal"),
	None            UMETA(DisplayName = "None")
};

// Tipo de habilidade, indicando se ela e ativa ou passiva.
UENUM(BlueprintType)
enum class ESkillTypeEnum : uint8
{
	Passive		UMETA(DisplayName = "Passive"),
	Active		UMETA(DisplayName = "Active"),
	None            UMETA(DisplayName = "None")
};

// Categoria funcional da habilidade, usada para filtros e regras de gameplay.
UENUM(BlueprintType)
enum class ESkillCategoryEnum : uint8
{
	Damage          UMETA(DisplayName = "Damage"),
	Heal            UMETA(DisplayName = "Heal"),
	Shield          UMETA(DisplayName = "Shield"),
	Buff            UMETA(DisplayName = "Buff"),
	Debuff          UMETA(DisplayName = "Debuff"),
	CrowdControl    UMETA(DisplayName = "Crowd Control"),
	Invocation      UMETA(DisplayName = "Invocation"),
	Mobility        UMETA(DisplayName = "Mobility"),
	None            UMETA(DisplayName = "None")
};

// UENUM(BlueprintType)
// enum class ESkillTagsEnum : uint8
// {
// 	Necromancy		UMETA(DisplayName = "Necromancy"),
// 	FireDamage		UMETA(DisplayName = "Fire Damage"),
// 	None            UMETA(DisplayName = "None")
// };
