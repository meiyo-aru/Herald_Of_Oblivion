#include "Structs/SkillStructs.h"

#include "NiagaraComponent.h"
#include "Core/SkillInstance.h"   
#include "Core/EntityClass.h" 

// Implementação do construtor padrão
FSkillContext::FSkillContext() 
	: SkillInstance(nullptr)
	, EntityOwner(nullptr){}

// Implementação do construtor com parâmetros
FSkillContext::FSkillContext(USkillInstance* InSkillInstance, AEntityClass* InOwner)
{
	this->SkillInstance = InSkillInstance;
	this->EntityOwner = InOwner;
}

void FSkillContext::Reset()
{
	SkillActor = nullptr;
	SkillStage = ESkillStage::None;
	Direction = FVector::ZeroVector;
	EntityOnStartLocation = nullptr;
	EntityOnEndLocation = nullptr;
	StartLocation = FVector::ZeroVector;
	EndLocation = FVector::ZeroVector;
	StartSurfaceNormal = FVector::ZeroVector;
	EndSurfaceNormal = FVector::ZeroVector;
	InitChargeTime = 0.0f;
	EndChargeTime = 0.0f;
	HoldDuration = 0.0f;
	ChargeRatio = 0.0f;
	
	for (TWeakObjectPtr<UNiagaraComponent> NC : SpawnedNiagaraComponents)
	{
		if (UNiagaraComponent* Niagara = NC.Get())
		{
			Niagara->SetAutoDestroy(true);
			Niagara->Deactivate();
		}	
	}
	SpawnedNiagaraComponents.Empty();
}

