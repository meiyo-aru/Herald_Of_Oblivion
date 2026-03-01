// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Execution/ExecutionSpawnProjectileFeature.h"

#include "Core/EntityClass.h"
#include "Core/SkillActor.h"
#include "Data/SkillDataAsset.h"
#include "NiagaraComponent.h"
#include "Core/SkillInstance.h"
#include "GameFramework/ProjectileMovementComponent.h"


void UExecutionSpawnProjectileFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UExecutionSpawnProjectileFeature::Execute(FSkillContext& InSkillContext)
{
	Super::Execute(InSkillContext);
	
	UE_LOG(LogTemp, Warning, TEXT("Execute"));
	if (!InSkillContext.bActivated) return;

	if (InSkillContext.EndLocation.IsZero()) return;
	
	this->SpawnProjectile(InSkillContext, FName("RightHandSocket"));	
}

void UExecutionSpawnProjectileFeature::SpawnProjectile(FSkillContext& InSkillContext, FName ShootingSocketName)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnProjectile"));
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionSpawnProjectileFeature::SpawnProjectile - SkillInstance invalido."));
		return;
	}
	
	const USkillDataAsset* SkillDataAsset = SkillInstance->GetDataAsset();
	if (!SkillDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionSpawnProjectileFeature::SpawnProjectile - SkillDataAsset invalido."));
		return;
	}
	
	AEntityClass* EntityOwner = InSkillContext.EntityOwner.Get();
	if (!EntityOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionSpawnProjectileFeature::SpawnProjectile - EntityOwner invalido."));
		return;
	}
	
	FVector SocketLocation = EntityOwner->GetMesh()->GetSocketLocation(ShootingSocketName);
	FVector TargetLocation = InSkillContext.EndLocation;
	
	// Atualiza a direção do contexto para a nova direção calculada
	InSkillContext.Direction = (TargetLocation - SocketLocation).GetSafeNormal();
	
	FTransform SpawnTransform(EntityOwner->GetActorRotation(), SocketLocation);

	ASkillActor* SkillActor = SpawnSkillActor(EntityOwner, SpawnTransform);
	InSkillContext.SkillActor = SkillActor;
	if (!IsValid(SkillActor))
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionSpawnProjectileFeature::SpawnProjectile - Falha ao spawnar SkillActor."));
		return;
	}

	SkillActor->Initialize(SkillInstance, EntityOwner, InSkillContext);
	SkillActor->SkillContext = InSkillContext;

	// Timer de vida máxima (fallback)
	if (this->LifeSpan > 0.0f)
	{
		SkillActor->SetLifeSpan(this->LifeSpan);
	}
	
	if (IsValid(SkillActor->ProjectileMovementComponent))
	{
		SkillActor->ProjectileMovementComponent->InitialSpeed = this->Speed;
		SkillActor->ProjectileMovementComponent->MaxSpeed = this->Speed;
		SkillActor->ProjectileMovementComponent->bRotationFollowsVelocity = true; 
		SkillActor->ProjectileMovementComponent->bShouldBounce = false;
		SkillActor->ProjectileMovementComponent->ProjectileGravityScale = 0.0f; // 0 = reta, 1 = arco

		// Define a velocidade DIRETAMENTE
		SkillActor->ProjectileMovementComponent->Velocity = InSkillContext.Direction * this->Speed;
      
		// Força o componente a entender que a velocidade mudou
		SkillActor->ProjectileMovementComponent->UpdateComponentVelocity();
		SkillActor->ProjectileMovementComponent->Activate(true);
	}
	
	if (IsValid(SkillActor->NiagaraComponent))
	{
		if (!SkillActor->NiagaraComponent->OnSystemFinished.IsAlreadyBound(this, &UExecutionSpawnProjectileFeature::OnNiagaraSystemFinished))
		{
			SkillActor->NiagaraComponent->OnSystemFinished.AddDynamic(this, &UExecutionSpawnProjectileFeature::OnNiagaraSystemFinished);
		}
		SkillActor->NiagaraComponent->Activate();
	}
}

void UExecutionSpawnProjectileFeature::OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	Super::OnNiagaraSystemFinished(FinishedComponent);
}

void UExecutionSpawnProjectileFeature::OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	Super::OnAuraNiagaraSystemFinished(FinishedComponent);
}

void UExecutionSpawnProjectileFeature::ProccessParticles(const TArray<FBasicParticleData>& Data, FSkillContext& SkillContext)
{
	if (Data.Num() == 0 || !SkillContext.SkillInstance.IsValid())
	{
		return;
	}

	ASkillActor* SkillActor = SkillContext.SkillActor.Get();
	AEntityClass* EntityOwner = SkillContext.EntityOwner.Get();
	
	if (!IsValid(SkillActor) || !IsValid(EntityOwner)) return;
		
	TArray<FOverlapResult> OutOverlaps = MakeHitSphere(SkillContext.SkillInstance->GetDataAsset()->RadiusCollision, SkillContext, Data[0].Position);

	if (OutOverlaps.Num() == 0) return;
	for (FOverlapResult OutOverlap : OutOverlaps)
	{
		UE_LOG(LogTemp, Warning, TEXT("Colidiu com %s"), *OutOverlap.GetActor()->GetName());
	}
}
