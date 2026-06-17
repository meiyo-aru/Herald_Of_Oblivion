// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Execution/ExecutionUniqueSpawnAtLocationFeature.h"

#include "NiagaraComponent.h"
#include "Core/EntityClass.h"
#include "Core/SkillActor.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "SkillFeatures/Execution/ExecutionSpawnProjectileFeature.h"
#include "Kismet/KismetMathLibrary.h"

void UExecutionUniqueSpawnAtLocationFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UExecutionUniqueSpawnAtLocationFeature::Execute(FSkillContext& InSkillContext)
{
	Super::Execute(InSkillContext);
	
	InSkillContext.SkillStage = ESkillStage::Hitted;
	
	if (!InSkillContext.bActivated) return;
	if (InSkillContext.EndLocation.IsZero()) return;
	
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::Execute - SkillInstance invalido."));
		return;
	}
	
	
	this->SpawnAtLocation(InSkillContext);
	
	SkillInstance->FinishSkill();		
	SkillInstance->GoOnCooldown();
}

void UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation(FSkillContext& InSkillContext)
{
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation - SkillInstance invalido."));
		return;
	}
	
	const USkillDataAsset* SkillDataAsset = SkillInstance->GetDataAsset();
	if (!SkillDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation - SkillDataAsset invalido."));
		return;
	}
	
	UNiagaraSystem* VFX = this->ExecutionEffect.Get();
	
	if (!VFX)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation - VFX ExecutionEffect invalido para ExecutionFeature '%s'."), *GetNameSafe(this));
		return;
	}
	
	AEntityClass* EntityOwner = InSkillContext.EntityOwner.Get();
	if (!EntityOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation - EntityOwner invalido."));
		return;
	}
	
	const FVector SpawnLocation = InSkillContext.StartLocation;
	if (SpawnLocation.IsZero()) {UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation - SpawnLocation inválido.")); return;};

	const FVector SurfaceNormal = InSkillContext.StartSurfaceNormal;
	
	const FRotator SpawnRotation = !SurfaceNormal.IsZero()
		? UKismetMathLibrary::MakeRotFromZ(SurfaceNormal)
		: FRotator::ZeroRotator;

	UNiagaraComponent* NiagaraComp = SpawnVFXAtLocation(VFX, SpawnRotation, SpawnLocation);

	if (!NiagaraComp)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionUniqueSpawnAtLocationFeature::SpawnAtLocation - Falha ao spawnar Niagara."));
		return;
	}
		
	NiagaraComp->SetFloatParameter(FName("ChargeRatio"), InSkillContext.ChargeRatio);
	NiagaraComp->SetFloatParameter(FName("MinLifeSpan"), this->MinLifeSpan);
	NiagaraComp->SetFloatParameter(FName("MaxLifeSpan"), this->MaxLifeSpan);
	NiagaraComp->SetFloatParameter(FName("Radius"), this->Intensity);

	float HitSphereRadius = this->CollisionRadius * FMath::Clamp(InSkillContext.ChargeRatio, 0.2f, 1.0f);

	TArray<FOverlapResult> OutOverlaps = MakeHitSphere(HitSphereRadius, InSkillContext, SpawnLocation);
	
	for (FOverlapResult OutOverlap : OutOverlaps)
	{
		UE_LOG(LogTemp, Warning, TEXT("Colidiu com %s"), *OutOverlap.GetActor()->GetName());
		InSkillContext.HitOverlapResultType = EHitOverlapResultType::Overlap;
		InSkillContext.OverlapResult = OutOverlap;
		
		if (OutOverlap.GetActor()->GetClass() == AEntityClass::StaticClass())
			InSkillContext.SkillInstance->OnHitEntity.Broadcast(InSkillContext, Cast<AEntityClass>(OutOverlap.GetActor()));
		else
			InSkillContext.SkillInstance->OnHitSurface.Broadcast(InSkillContext, InSkillContext.StartLocation);
	}	
}

void UExecutionUniqueSpawnAtLocationFeature::OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	Super::OnNiagaraSystemFinished(FinishedComponent);
}

void UExecutionUniqueSpawnAtLocationFeature::OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	Super::OnAuraNiagaraSystemFinished(FinishedComponent);
}

void UExecutionUniqueSpawnAtLocationFeature::ProccessParticles(const TArray<struct FBasicParticleData>& Data, FSkillContext& SkillContext)
{
}
