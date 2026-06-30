// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/OnHit/OnHitSurfaceExplosion.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

void UOnHitSurfaceExplosion::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UOnHitSurfaceExplosion::Execute(FSkillContext& InSkillContext)
{
	Super::Execute(InSkillContext);
	
	if (UNiagaraSystem* VFX = OnHitEffect.Get())
	{
		if (InSkillContext.HitResult.bBlockingHit)
		{
			UNiagaraComponent* Niagara = SpawnVFXAtLocation(VFX, UKismetMathLibrary::MakeRotFromZ(InSkillContext.HitResult.ImpactNormal), InSkillContext.HitResult.ImpactPoint, InSkillContext);
			Niagara->SetFloatParameter(FName("ExplosionRadius"), ExplosionRadius);
			Niagara->SetFloatParameter(FName("ExplosionTemperature"), ExplosionTemperature);
			Niagara->SetVectorParameter(FName("ExplosionIntensityScale"), ExplosionIntensityScale);
		}
	}
	
}
