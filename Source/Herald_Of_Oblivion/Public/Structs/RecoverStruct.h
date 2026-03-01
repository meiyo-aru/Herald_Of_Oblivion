#pragma once

#include "CoreMinimal.h" 
#include "RecoverStruct.generated.h"

UENUM(BlueprintType)
enum class ERecoverableAttributeEnum : uint8
{
	Health		UMETA(DisplayName = "Health"),
	Mana		UMETA(DisplayName = "Mana"),
	Energy		UMETA(DisplayName = "Energy"),
	None        UMETA(DisplayName = "None")
};

USTRUCT(BlueprintType, Blueprintable)
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
