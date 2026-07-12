// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SkillActor.h"

#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "NiagaraDataInterfaceExport.h"
#include "Components/SphereComponent.h"
#include "Core/EntityClass.h"
#include "Core/EquipmentActor.h"
#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Subsystems/PoolingManager.h"


// Sets default values
ASkillActor::ASkillActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->CollisionComponent = CreateDefaultSubobject<USphereComponent>(FName("Collision Component"));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(this->CollisionComponent);

	// Criar o movimento já no construtor
	this->ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement Component"));
	this->ProjectileMovementComponent->bAutoActivate = false; // Só ativa quando dermos a velocidade}
}

// Esta função será chamada automaticamente pelo Niagara! Usada para executar lógica para cada partícula individualmente
void ASkillActor::ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data, UNiagaraSystem* NiagaraSystem, const FVector& SimulationPositionOffset)
{
	if (Data.Num() == 0) return;
	if (IsActorBeingDestroyed() || !IsValid(this)) return;

	USkillInstance* SkillInstance = this->Instance.Get();
	if (!IsValid(SkillInstance) || !IsValid(SkillInstance->ExecutionFeature))
	{
		return;
	}
	
	SkillInstance->CurrentContext.SkillActor = this;
	SkillInstance->ExecutionFeature->ProccessParticles(Data, SkillInstance->CurrentContext);
	
	this->ParticlesProcessed++;
}

// Called when the game starts or when spawned
void ASkillActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASkillActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	this->CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ASkillActor::OnBeginOverlap);
	this->CollisionComponent->OnComponentHit.RemoveDynamic(this, &ASkillActor::OnHit);

	if (IsValid(this->NiagaraComponent))
	{
		this->NiagaraComponent->SetVariableObject(FName("CallbackObject"), nullptr);
		this->NiagaraComponent->Deactivate();
	}

	Super::EndPlay(EndPlayReason);
}

void ASkillActor::BeginDestroy()
{
	if (IsValid(this->NiagaraComponent))
	{
		this->NiagaraComponent->SetVariableObject(FName("CallbackObject"), nullptr);
		this->NiagaraComponent->Deactivate();
	}
	Super::BeginDestroy();
}
// Called every frame
void ASkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASkillActor::OnNiagaraSystemFinished(UNiagaraComponent* NC)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
		if (UPoolingManager* PoolingManager = World->GetGameInstance()->GetSubsystem<UPoolingManager>())
		{
			NC->OnSystemFinished.RemoveAll(this);
			PoolingManager->SaveNiagaraInPool(NC);
			this->NiagaraComponent = nullptr;
		}
	UE_LOG(LogTemp,Warning, TEXT("Skill Actor Destroyed!"));
}

void ASkillActor::Initialize(USkillInstance* InInstance, AEntityClass* InEntity, FSkillContext& InSkillContext)
{
	if (!IsValid(InInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("ASkillActor::Initialize - SkillInstance invalido para '%s'."), *GetName());
		return;
	}

	this->Instance = InInstance;
	this->OwnerEntity = InEntity;

	// Se tiver componente de Niagara, configura-o
	if (this->NiagaraComponent)
	{
		this->NiagaraComponent->OnSystemFinished.AddDynamic(this, &ASkillActor::OnNiagaraSystemFinished);
		this->NiagaraComponent->SetVariableObject(FName("CallbackObject"), this);
		this->NiagaraComponent->SetFloatParameter(FName("ChargeRatio"), InSkillContext.ChargeRatio);
		this->NiagaraComponent->Activate(true);
		this->NiagaraComponent->SetVisibility(true);
		this->NiagaraComponent->SetAbsolute(false,false,false);
		this->NiagaraComponent->AttachToComponent(CollisionComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void ASkillActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
					   UPrimitiveComponent* OtherComp, FVector NormalImpulse,
					   const FHitResult& Hit)
{
	USkillInstance* SkillInstance = this->Instance.Get();
	if (!IsValid(SkillInstance))
	{
		return;
	}
	
	if (IsValid(this->NiagaraComponent))
	{
		this->NiagaraComponent->Deactivate();
	}
	
	if (USkillInstance* StrongInstance = this->Instance.Get())
	{
		FHitOverlapResult HitOverlapResult = FHitOverlapResult(Hit);
		StrongInstance->CurrentContext.HitOverlapResult = HitOverlapResult;
		SkillInstance->OnSkillHitDelegate.Broadcast(Instance->CurrentContext);
	}
}

void ASkillActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
};

void ASkillActor::ConfigureCollisionComponent(USkillFeature* SkillFeature, AEntityClass* EntityOwner)
{
	CollisionComponent->IgnoreActorWhenMoving(EntityOwner, true);
	CollisionComponent->MoveIgnoreActors.Add(EntityOwner);
	
	if (AEquipmentActor* Actor = EntityOwner->GetEquipmentActor(EEquipmentSlot::RightWeapon))
		CollisionComponent->MoveIgnoreActors.Add(Actor);
	if (AEquipmentActor* Actor = EntityOwner->GetEquipmentActor(EEquipmentSlot::LeftWeapon))
		CollisionComponent->MoveIgnoreActors.Add(Actor);

	CollisionComponent->SetGenerateOverlapEvents(SkillFeature->bGenerateOverlapEvents);
	CollisionComponent->SetNotifyRigidBodyCollision(SkillFeature->bNotifyRigidBodyCollision);
	CollisionComponent->SetCollisionEnabled(SkillFeature->CollisionEnabled);
	CollisionComponent->SetCollisionObjectType(SkillFeature->CollisionObjectType);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, SkillFeature->PawnCollision);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, SkillFeature->WorldStaticCollision);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, SkillFeature->WorldDynamicCollision);
	CollisionComponent->SetSphereRadius(SkillFeature->RadiusCollision);
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	
	// Evita binds duplicados quando BeginPlay e Initialize chamam este método.
	this->CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ASkillActor::OnBeginOverlap);
	this->CollisionComponent->OnComponentHit.RemoveDynamic(this, &ASkillActor::OnHit);

	if (SkillFeature->bGenerateOverlapEvents)
	{
		this->CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASkillActor::OnBeginOverlap);
	}
	if (SkillFeature->bGenerateHitEvents)
	{
		this->CollisionComponent->OnComponentHit.AddDynamic(this, &ASkillActor::OnHit);
	}
}