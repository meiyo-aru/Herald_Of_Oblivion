#include "Utility/GeneralUtilities.h"
#include "Enumerators/EntityEnums.h"

EEntityTrueAttributeEnum GeneralUtilities::ConvertTypeDamageEnumToTrueAttributeEnum(ETypeDamage TypeDamage)
{
	switch (TypeDamage)
	{
		case ETypeDamage::SlashingDamage:
			return EEntityTrueAttributeEnum::SlashingDamage;
		default:
			return EEntityTrueAttributeEnum::None;
	}
}
