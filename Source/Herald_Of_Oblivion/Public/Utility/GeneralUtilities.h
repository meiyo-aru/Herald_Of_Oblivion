#pragma once

enum class ETypeDamage : uint8;
enum class EEntityTrueAttributeEnum : uint8;

class GeneralUtilities
{
public:
	
	static EEntityTrueAttributeEnum ConvertTypeDamageEnumToTrueAttributeEnum(ETypeDamage TypeDamage);
};
