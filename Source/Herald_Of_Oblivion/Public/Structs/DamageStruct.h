#pragma once

#include "CoreMinimal.h" 
#include "Enumerators/EntityEnums.h"
#include "DamageStruct.generated.h"

UENUM(BlueprintType)
enum class ETypeDamage : uint8
{
	Elemental		UMETA(DisplayName = "Elemental"),
	Physical		UMETA(DisplayName = "Physical"),
	Magical			UMETA(DisplayName = "Magical"),
	None            UMETA(DisplayName = "None")
};

USTRUCT(BlueprintType, Blueprintable)
struct FDamageStruct
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	ETypeDamage TypeEnum = ETypeDamage::None;
	UPROPERTY(EditAnywhere)
	EEntityTrueAttributeEnum AttributeEnum = EEntityTrueAttributeEnum::None;
	UPROPERTY(EditAnywhere)
	uint8 ScalingDamage = 0;
	UPROPERTY(EditAnywhere)
	uint16 TotalDamage = 0;
};
