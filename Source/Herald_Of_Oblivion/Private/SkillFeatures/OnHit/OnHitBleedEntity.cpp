// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/OnHit/OnHitBleedEntity.h"

#include "Core/EntityClass.h"
#include "Core/SkillInstance.h"

void UOnHitBleedEntity::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
	if (Owner) 
	{
		// Delegate disparado ao feature de target terminar
		Owner->OnHitEntity.AddUObject(this, &UOnHitBleedEntity::ExecuteOnHitEntity);
	}; 
}

void UOnHitBleedEntity::Execute(FSkillContext& InSkillContext, FVector TargetLocation)
{
	Super::Execute(InSkillContext, TargetLocation);
	
}

void UOnHitBleedEntity::ExecuteOnHitEntity(FSkillContext& InSkillContext, AEntityClass* TargetEntity)
{
	const FVector TargetLocation = IsValid(TargetEntity) ? TargetEntity->GetActorLocation() : FVector::ZeroVector;
	this->Execute(InSkillContext, TargetLocation);
}
