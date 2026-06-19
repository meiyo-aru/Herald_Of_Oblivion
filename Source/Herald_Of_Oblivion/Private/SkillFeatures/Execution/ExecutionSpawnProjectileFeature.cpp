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
	
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionThornFeature::Execute - SkillInstance invalido."));
		return;
	}
	
	this->SpawnProjectile(InSkillContext, FName("RightHandSocket"));
	SkillInstance->FinishSkill();
	SkillInstance->GoOnCooldown();
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
	
	FVector StartLocation;
	if (this->bThrowByTheHands)
	{
		StartLocation = EntityOwner->GetMesh()->GetSocketLocation(ShootingSocketName);
	}
	else
	{
		StartLocation = InSkillContext.StartLocation;
		StartLocation.Z += 5;
	}
	
	FVector TargetLocation = InSkillContext.EndLocation;
	
	/*
	if (bParallelToTheTerrain)
	{
		// Faz um linetrace para baixo para pegar o impact normal do terreno
		FHitResult Hit;
		FVector Start = StartLocation;
		Start.Z += 50;
		
		FVector End = StartLocation;
		End.Z -= 200;
		
		// Configurações da Consulta
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(EntityOwner); // O laser não deve atingir o próprio personagem

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			Hit, 
			Start, 
			End, 
			ECC_Visibility, 
			Params
		);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("OPAAA")));

		if (bHit)
		{
			// faz a projeção da direcao do projetil usando o impactnormal (o projetil sai paralelo ao terreno)
			FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();
			InSkillContext.Direction = FVector::VectorPlaneProject(Direction, Hit.ImpactNormal);
			InSkillContext.Direction.Normalize();
		}
	} else
	{
	}*/
	
	// Atualiza a direção do contexto para a nova direção calculada
	InSkillContext.Direction = (TargetLocation - StartLocation).GetSafeNormal();
	
	
	FTransform SpawnTransform(EntityOwner->GetActorRotation(), StartLocation);

	ASkillActor* SkillActor = SpawnSkillActor(EntityOwner, SpawnTransform);
	InSkillContext.SkillActor = SkillActor;
	if (!IsValid(SkillActor))
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionSpawnProjectileFeature::SpawnProjectile - Falha ao spawnar SkillActor."));
		return;
	}

	SkillActor->Initialize(SkillInstance, EntityOwner, InSkillContext);

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
		
		if (bParallelToTheTerrain)
			if (!((StartLocation.Z > TargetLocation.Z + 160) || (StartLocation.Z < TargetLocation.Z - 160)))
				SkillActor->ProjectileMovementComponent->Velocity.Z = 0.0f;
		
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
	AActor* Actor = FinishedComponent->GetOwner();
	if (IsValid(Actor))
		Actor->Destroy();
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
		
	TArray<FOverlapResult> OutOverlaps = MakeHitSphere(this->RadiusCollision, SkillContext, Data[0].Position);

	if (OutOverlaps.Num() == 0) return;
	for (FOverlapResult OutOverlap : OutOverlaps)
	{
		UE_LOG(LogTemp, Warning, TEXT("Colidiu com %s"), *OutOverlap.GetActor()->GetName());
	}
}
