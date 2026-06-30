#pragma once

#include "CoreMinimal.h" 
#include "ItemEnums.generated.h"

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	None UMETA(DisplayName = "None"),
	Weapon UMETA(DisplayName = "Weapon"),
	Shield UMETA(DisplayName = "Shield"),
	Helmet UMETA(DisplayName = "Helmet"),
	Chest UMETA(DisplayName = "Chest"),
	Leggings UMETA(DisplayName = "Leggings"),
	Boots UMETA(DisplayName = "Boots"),
	Gloves UMETA(DisplayName = "Gloves"),
	Ring UMETA(DisplayName = "Ring"),
	Amulet UMETA(DisplayName = "Amulet")	
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None UMETA(DisplayName = "None"),
	LeftWeapon UMETA(DisplayName = "Left Weapon"),
	RightWeapon UMETA(DisplayName = "Right Weapon"),
	Shield UMETA(DisplayName = "Shield"),
	Helmet UMETA(DisplayName = "Helmet"),
	Chest UMETA(DisplayName = "Chest"),
	Leggings UMETA(DisplayName = "Leggings"),
	Boots UMETA(DisplayName = "Boots"),
	Gloves UMETA(DisplayName = "Gloves"),
	Ring UMETA(DisplayName = "Ring"),
	Amulet UMETA(DisplayName = "Amulet")	
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None UMETA(DisplayName = "None"),
	Staff UMETA(DisplayName = "Staff"),
	Wand UMETA(DisplayName = "Wand"),
	Sword UMETA(DisplayName = "Sword"),
	Axe UMETA(DisplayName = "Axe"),
	Dagger UMETA(DisplayName = "Dagger"),
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None UMETA(DisplayName = "None"),
	Potion UMETA(DisplayName = "Potion"),
	Equipment UMETA(DisplayName = "Equipment"),
};

UENUM(BlueprintType)
enum class EItemRarityEnum : uint8
{
	None		UMETA(DisplayName = "None"),
	Normal		UMETA(DisplayName = "Normal"),
	Magic		UMETA(DisplayName = "Magic"),
	Rare		UMETA(DisplayName = "Rare"),
	Unique		UMETA(DisplayName = "Unique"),
	Legendary		UMETA(DisplayName = "Legendary"),
};