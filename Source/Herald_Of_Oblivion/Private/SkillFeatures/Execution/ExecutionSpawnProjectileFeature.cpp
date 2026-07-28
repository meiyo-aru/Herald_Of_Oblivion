// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Execution/ExecutionSpawnProjectileFeature.h"

#include "Core/EntityClass.h"
#include "Core/SkillActor.h"
#include "Data/SkillDataAsset.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Character/PlayerClass.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Core/EquipmentActor.h"
#include "Core/SkillInstance.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Subsystems/PoolingManager.h"


void UExecutionSpawnProjectileFeature::LoadFXSync()
{
	Super::LoadFXSync();
}

void UExecutionSpawnProjectileFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
}

void UExecutionSpawnProjectileFeature::PrimaryExecute(FSkillContext& InSkillContext)
{
	Super::PrimaryExecute(InSkillContext);
}

void UExecutionSpawnProjectileFeature::FinallyExecute(FSkillContext& InSkillContext)
{
	Super::FinallyExecute(InSkillContext);
	
	USkillInstance* SkillInstance = InSkillContext.SkillInstance.Get();
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionThornFeature::Execute - SkillInstance invalido."));
		return;
	}
	
	this->SpawnProjectile(InSkillContext, FName("RightWeaponSocket"));
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
	
	UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
	if (!AssetManager)
	{
		UE_LOG(LogTemp, Error, TEXT("USkillInstance::CastSkill - AssetManager invalido."));
		return;
	}

	const USkillDataAsset* SkillDataAsset = SkillInstance->DataAsset;
		
	if (!SkillDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionSpawnProjectileFeature::SpawnProjectile - SkillDataAsset invalido."));
		return;
	}
	
	AEntityClass* EntityOwner = Cast<AEntityClass>(InSkillContext.EntityOwner.Get());
	if (!EntityOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionSpawnProjectileFeature::SpawnProjectile - EntityOwner invalido."));
		return;
	}
	
	FVector StartLocation;
	if (this->bThrowByTheForward)
	{
		StartLocation = EntityOwner->GetMesh()->GetSocketLocation("Forward");
	}
	else 
	{
		if (bThrowByTheRightHand)
		{
			if (AEquipmentActor* EquipmentActor = EntityOwner->GetEquipmentActor(EEquipmentSlot::RightWeapon))
				StartLocation = EquipmentActor->GetMesh()->GetSocketLocation("Charge");
			else StartLocation = EntityOwner->GetMesh()->GetSocketLocation("RightHand");
			
		} else
		{
			if (AEquipmentActor* EquipmentActor = EntityOwner->GetEquipmentActor(EEquipmentSlot::LeftWeapon))
				StartLocation = EquipmentActor->GetMesh()->GetSocketLocation("Charge");
			else StartLocation = EntityOwner->GetMesh()->GetSocketLocation("LeftHand");
		}
	}
	
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
	
	FVector TargetLocation;
	if (InSkillContext.EndLocation.IsZero())
	{
		if (APlayerClass* Char = Cast<APlayerClass>(EntityOwner))
		{
			TargetLocation = StartLocation + Char->GetCameraComponent()->GetForwardVector() * MaximumRange;
		}
	} else TargetLocation = InSkillContext.EndLocation;
	
	// Atualiza a direção do contexto para a nova direção calculada
	InSkillContext.Direction = (TargetLocation - StartLocation).GetSafeNormal();
	
	
	FTransform SpawnTransform(EntityOwner->GetActorRotation(), StartLocation);

	ASkillActor* SkillActor = SpawnSkillActor(EntityOwner, SpawnTransform);
	if (!IsValid(SkillActor))
	{
		UE_LOG(LogTemp, Error, TEXT("UExecutionSpawnProjectileFeature::SpawnProjectile - Falha ao spawnar SkillActor."));
		return;
	}
	
	UNiagaraSystem* VFX;
	TObjectPtr<UNiagaraSystem>* LoadedExecutionEffect = CachedEffects.Find(FName("ExecutionEffect"));

	if (LoadedExecutionEffect) 
		VFX = *LoadedExecutionEffect;
	else 
		VFX = ExecutionEffect.Get();

	InSkillContext.SkillActor = SkillActor;
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
		if (UPoolingManager* PoolingManager = World->GetGameInstance()->GetSubsystem<UPoolingManager>())
		{
			SkillActor->NiagaraComponent = PoolingManager->GetNiagaraComponentFromPool(VFX);
		}

	SkillActor->Initialize(SkillInstance, EntityOwner, InSkillContext);
	
	if (bHaveCollisionComponent)
		SkillActor->ConfigureCollisionComponent(this, EntityOwner);
	
	SkillActor->ProjectileMovementComponent->SetUpdatedComponent(SkillActor->CollisionComponent);
	
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
}

void UExecutionSpawnProjectileFeature::OnPlayMontageNotifyBegin(FName NotifyName)
{
	Super::OnPlayMontageNotifyBegin(NotifyName);
}

void UExecutionSpawnProjectileFeature::ProccessParticles(const TArray<FBasicParticleData>& Data, FSkillContext& InSkillContext)
{
	if (Data.Num() == 0 || !InSkillContext.SkillInstance.IsValid())
	{
		return;
	}

	ASkillActor* SkillActor = InSkillContext.SkillActor.Get();
	AEntityClass* EntityOwner = Cast<AEntityClass>(InSkillContext.EntityOwner.Get());
	
	if (!IsValid(SkillActor) || !IsValid(EntityOwner)) return;
		
	UE_LOG(LogTemp, Warning, TEXT("UExecutionSpawnProjectileFeature::ProccessParticles     ssssssssssss"));
	TArray<FOverlapResult> OutOverlaps = MakeOverlapSphere(this->RadiusCollision, InSkillContext, Data[0].Position);

	if (OutOverlaps.Num() == 0) return;
	for (FOverlapResult OutOverlap : OutOverlaps)
	{
		UE_LOG(LogTemp, Warning, TEXT("Colidiu com %s"), *OutOverlap.GetActor()->GetName());
	}
}
