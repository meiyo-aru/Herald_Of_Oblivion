#pragma once

#include "CoreMinimal.h" 
#include "Enumerators/EntityEnums.h"
#include "AffectAttributeStruct.generated.h"

USTRUCT(BlueprintType, Blueprintable)
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
	
	UPROPERTY(EditAnywhere,  meta = (EditCondition = "PercentValue == 0", EditConditionHides))
	float FlatValue = 0.0f;

	UPROPERTY(EditAnywhere,  meta = (EditCondition = "FlatValue == 0.0f", EditConditionHides))
	uint8 PercentValue = 0;
	
	// Decide se será um ou não
	UPROPERTY(EditAnywhere)
	bool bIsBuff = false;
	
	// Duração do efeito, 0.0f significa permanente
	UPROPERTY(EditAnywhere)
	float Duration = 0.0;
};
