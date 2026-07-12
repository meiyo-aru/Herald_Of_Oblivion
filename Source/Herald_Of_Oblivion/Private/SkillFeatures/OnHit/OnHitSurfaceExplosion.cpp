// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/OnHit/OnHitSurfaceExplosion.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Data/SkillDataAsset.h"
#include "Sound/SoundCue.h"
#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

void UOnHitSurfaceExplosion::LoadFXSync()
{
	Super::LoadFXSync();
	
	LoadedOnHitEffect = OnHitEffect.LoadSynchronous();
	WarmupNiagara(LoadedOnHitEffect);
	LoadedOnHitSound = OnHitSound.LoadSynchronous();
}

void UOnHitSurfaceExplosion::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UOnHitSurfaceExplosion::Execute(FSkillContext& InSkillContext)
{
	Super::Execute(InSkillContext);
	
	UNiagaraSystem* VFX;
	if (LoadedOnHitEffect) 
		VFX = LoadedOnHitEffect;
	else 
		VFX = OnHitEffect.Get();

	if (VFX)
	{
		if (InSkillContext.HitOverlapResult.HitResult.bBlockingHit)
		{
			UNiagaraComponent* Niagara = SpawnVFXAtLocation(VFX, UKismetMathLibrary::MakeRotFromZ(InSkillContext.HitOverlapResult.HitResult.ImpactNormal), InSkillContext.HitOverlapResult.HitResult.ImpactPoint, InSkillContext);
			if (!Niagara)
			{
				UE_LOG(LogTemp, Error, TEXT("UOnHitSurfaceExplosion - Falha ao spawnar Niagara."));
				return;
			}
			Niagara->SetFloatParameter(FName("ExplosionRadius"), ExplosionRadius);
			Niagara->SetFloatParameter(FName("ExplosionTemperature"), ExplosionTemperature);
			Niagara->SetVectorParameter(FName("ExplosionIntensityScale"), ExplosionIntensityScale);
		}
	}
	
}
