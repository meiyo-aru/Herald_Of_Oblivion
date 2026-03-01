#include "Structs/SkillStructs.h"
#include "Core/SkillInstance.h"   
#include "Core/EntityClass.h" 

// Implementação do construtor padrão
FSkillContext::FSkillContext() 
	: SkillInstance(nullptr)
	, EntityOwner(nullptr)
	, HitOverlapResultType(EHitOverlapResultType::None)
{}

// Implementação do construtor com parâmetros
FSkillContext::FSkillContext(USkillInstance* InSkillInstance, AEntityClass* InOwner)
{
	this->SkillInstance = InSkillInstance;
	this->EntityOwner = InOwner;
	this->HitOverlapResultType = EHitOverlapResultType::None;
}

void FSkillContext::Reset()
{
	SkillActor = nullptr;
	Direction = FVector::ZeroVector;
	TargetEntity = nullptr;
	HitOverlapResultType = EHitOverlapResultType::None;
	HitResult = FHitResult();
	OverlapResult = FOverlapResult();
	bActivated = false;
	StartLocation = FVector::ZeroVector;
	EndLocation = FVector::ZeroVector;
	CastTime = 0.0f;
	ReleasedTime = 0.0f;
	HoldDuration = 0.0f;
	ChargeRatio = 0.0f;
}

