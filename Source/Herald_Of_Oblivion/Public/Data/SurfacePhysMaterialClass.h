// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/EntityClass.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "SurfacePhysMaterialClass.generated.h"

/**
 * USurfacePhysMaterialClass
 * Um PhysicalMaterial que serve para afetar uma entidade que está em contato com a superfície
 */
UCLASS()
class HERALD_OF_OBLIVION_API USurfacePhysMaterialClass : public UPhysicalMaterial
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SurfaceSpeedModifier = 0.5f;
	
	UPROPERTY(EditAnywhere, Category = "Attribute")
	EEntityTrueAttributeEnum Attribute;
};
