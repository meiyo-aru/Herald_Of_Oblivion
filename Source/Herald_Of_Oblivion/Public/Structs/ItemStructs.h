#pragma once

#include "CoreMinimal.h" 
#include "Enumerators/ItemEnums.h"
#include "ItemStructs.generated.h"

USTRUCT(BlueprintType)
struct FItemRarityStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EItemRarityEnum Rarity = EItemRarityEnum::None;
	
	UPROPERTY(EditAnywhere)
	float RarityModifier = 0.0f;
	
	FItemRarityStruct(EItemRarityEnum InRarity = EItemRarityEnum::None)
	{
		Rarity = InRarity;
		switch (Rarity)
		{
		case EItemRarityEnum::Normal:
			this->RarityModifier = 1.0f;
			break;
		case EItemRarityEnum::Magic:
			this->RarityModifier = 1.25f;
			break;
		case EItemRarityEnum::Rare:
			this->RarityModifier = 1.5f;
			break;
		case EItemRarityEnum::Unique:
			this->RarityModifier = 2.5f;
			break;
		case EItemRarityEnum::Legendary:
			this->RarityModifier = 4.0f;
			break;
		default:
			this->RarityModifier = 0.0f;
			break;
		}
	};
};