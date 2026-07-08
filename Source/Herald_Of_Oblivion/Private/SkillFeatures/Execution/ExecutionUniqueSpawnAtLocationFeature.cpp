// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Execution/ExecutionUniqueSpawnAtLocationFeature.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Core/EntityClass.h"
#include "Core/SkillActor.h"
#include "Core/SkillInstance.h"
#include "SkillFeatures/Execution/ExecutionSpawnProjectileFeature.h"
#include "Kismet/KismetMathLibrary.h"

void UExecutionUniqueSpawnAtLocationFeature::LoadFXSync()
{
	Super::LoadFXSync();
}

void UExecutionUniqueSpawnAtLocationFeature::CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents)
{
	Super::CleanNiagara(SpawnedNiagaraComponents);
}

void UExecutionUniqueSpawnAtLocationFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UExecutionUniqueSpawnAtLocationFeature::Execute(FSkillContext& InSkillContext)
{
	Super::Execute(InSkillContext);
	
	InSkillContext.SkillStage = ESkillStage::InExecution;
	
	if (!InSkillContext.bActivated) return;
	if (InSkillContext.EndLocation.IsZero()) return;
	
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::Execute - SkillInstance invalido."));
		return;
	}
	
	
	this->SpawnAtLocation(InSkillContext);
	
}

void UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation(FSkillContext& InSkillContext)
{
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation - SkillInstance invalido."));
		return;
	}
	
	UNiagaraSystem* VFX;
	if (LoadedExecutionEffect) 
		VFX = LoadedExecutionEffect;
	else 
		VFX = ExecutionEffect.Get();
	
	if (!VFX)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation - VFX ExecutionEffect invalido para ExecutionFeature '%s'."), *GetNameSafe(this));
		return;
	}
	
	AEntityClass* EntityOwner = Cast<AEntityClass>(InSkillContext.EntityOwner.Get());
	if (!EntityOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation - EntityOwner invalido."));
		return;
	}
	
	const FVector SpawnLocation = bSpawnOnEndLocation ? InSkillContext.EndLocation : InSkillContext.StartLocation;
	
	const FVector SurfaceNormal = InSkillContext.StartSurfaceNormal;
	
	FRotator SpawnRotation;
	
	if (bMakeRotFromZ)
		SpawnRotation = !SurfaceNormal.IsZero()
			? UKismetMathLibrary::MakeRotFromZ(SurfaceNormal)
			: FRotator::ZeroRotator;
	else SpawnRotation = FRotator::ZeroRotator;

	UNiagaraComponent* NiagaraComp = SpawnVFXAtLocation(VFX, SpawnRotation, SpawnLocation, InSkillContext);

	if (!NiagaraComp)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation - Falha ao spawnar Niagara."));
		return;
	}
		
	NiagaraComp->SetFloatParameter(FName("MinLifeSpan"), this->MinLifeSpan);
	NiagaraComp->SetFloatParameter(FName("MaxLifeSpan"), this->MaxLifeSpan);
	NiagaraComp->SetFloatParameter(FName("Radius"), this->Intensity);

	float HitSphereRadius = this->CollisionRadius * FMath::Clamp(InSkillContext.ChargeRatio, 0.2f, 1.0f);

	TArray<FOverlapResult> OutOverlaps = MakeOverlapSphere(HitSphereRadius, InSkillContext, SpawnLocation);
	
	if (!OutOverlaps.IsEmpty())
	{
		InSkillContext.HitOverlapResult = FHitOverlapResult(OutOverlaps);
		SkillInstance->OnSkillHitDelegate.Broadcast(InSkillContext);
	}
}


void UExecutionUniqueSpawnAtLocationFeature::ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext)
{
}
