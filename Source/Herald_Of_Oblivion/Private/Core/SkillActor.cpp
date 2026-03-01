// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SkillActor.h"

#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "NiagaraDataInterfaceExport.h"
#include "Components/SphereComponent.h"
#include "Core/EntityClass.h"
#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ASkillActor::ASkillActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ✅ Cria SceneComponent como Root (sem collision)
	this->CollisionComponent = CreateDefaultSubobject<USphereComponent>(FName("Collision Component"));
	SetRootComponent(this->CollisionComponent);

	// Criar o movimento já no construtor
	this->ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement Component"));
	this->ProjectileMovementComponent->bAutoActivate = false; // Só ativa quando dermos a velocidade}
	
	this->NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(FName("Niagara Component"));
	this->NiagaraComponent->SetupAttachment(GetRootComponent());
}

// Esta função será chamada automaticamente pelo Niagara!
void ASkillActor::ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data, UNiagaraSystem* NiagaraSystem, const FVector& SimulationPositionOffset)
{
	if (Data.Num() == 0) return;
	if (IsActorBeingDestroyed() || !IsValid(this)) return;

	USkillInstance* SkillInstance = this->Instance.Get();
	if (!IsValid(SkillInstance) || !IsValid(SkillInstance->ExecutionFeature))
	{
		return;
	}

	this->SkillContext.SkillActor = this;
	SkillInstance->ExecutionFeature->ProccessParticles(Data, this->SkillContext);
	this->ParticlesProcessed++;
}

void ASkillActor::BindCollisionDelegatesFromSkillData(const USkillDataAsset* SkillData)
{
	if (!IsValid(this->CollisionComponent) || !SkillData)
	{
		return;
	}

	// Evita binds duplicados quando BeginPlay e Initialize chamam este método.
	this->CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ASkillActor::OnBeginOverlap);
	this->CollisionComponent->OnComponentHit.RemoveDynamic(this, &ASkillActor::OnHit);

	if (SkillData->bGenerateOverlapEvents)
	{
		this->CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASkillActor::OnBeginOverlap);
	}
	if (SkillData->bGenerateHitEvents)
	{
		this->CollisionComponent->OnComponentHit.AddDynamic(this, &ASkillActor::OnHit);
	}

	this->bCollisionDelegatesBound = SkillData->bGenerateOverlapEvents || SkillData->bGenerateHitEvents;
}

// Called when the game starts or when spawned
void ASkillActor::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(this->CollisionComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("ASkillActor::BeginPlay - CollisionComponent invalido para '%s'."), *GetName());
		return;
	}

	const USkillInstance* SkillInstance = this->Instance.Get();
	const USkillDataAsset* SkillData = SkillInstance ? SkillInstance->GetDataAsset() : nullptr;
	BindCollisionDelegatesFromSkillData(SkillData);
}

void ASkillActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (this->bCollisionDelegatesBound && IsValid(this->CollisionComponent))
	{
		this->CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ASkillActor::OnBeginOverlap);
		this->CollisionComponent->OnComponentHit.RemoveDynamic(this, &ASkillActor::OnHit);
		this->bCollisionDelegatesBound = false;
	}

	if (IsValid(this->NiagaraComponent))
	{
		this->NiagaraComponent->SetVariableObject(FName("CallbackObject"), nullptr);
		this->NiagaraComponent->Deactivate();
	}

	Super::EndPlay(EndPlayReason);
}

void ASkillActor::BeginDestroy()
{
	if (this->NiagaraComponent)
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

void ASkillActor::Initialize(USkillInstance* InInstance, AEntityClass* InEntity, FSkillContext& InSkillContext)
{
	if (!IsValid(InInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("ASkillActor::Initialize - SkillInstance invalido para '%s'."), *GetName());
		return;
	}

	if (!IsValid(this->CollisionComponent) || !IsValid(this->ProjectileMovementComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("ASkillActor::Initialize - Componentes obrigatorios invalidos para '%s'."), *GetName());
		return;
	}

	this->Instance = InInstance;
	this->OwnerEntity = InEntity;
	const USkillDataAsset* SkillData = InInstance->GetDataAsset();
	if (!SkillData)
	{
		UE_LOG(LogTemp, Error, TEXT("ASkillActor::Initialize - SkillDataAsset invalido para SkillInstance '%s'."), *GetNameSafe(InInstance));
		return;
	}
	
	if (InEntity)
	{
		this->CollisionComponent->IgnoreActorWhenMoving(InEntity, true);
		this->CollisionComponent->MoveIgnoreActors.Add(InEntity);
	}
	
	this->CollisionComponent->SetGenerateOverlapEvents(SkillData->bGenerateOverlapEvents);
	this->CollisionComponent->SetNotifyRigidBodyCollision(SkillData->bNotifyRigidBodyCollision);
	this->CollisionComponent->SetCollisionEnabled(SkillData->CollisionEnabled);
	this->CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, SkillData->PawnCollision);
	this->CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, SkillData->WorldStaticCollision);
	this->CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, SkillData->WorldDynamicCollision);
	
	this->CollisionComponent->SetSphereRadius(SkillData->RadiusCollision);
	
	this->ProjectileMovementComponent->SetUpdatedComponent(this->CollisionComponent);
	this->BindCollisionDelegatesFromSkillData(SkillData);
	
	if (this->NiagaraComponent)
	{
		UNiagaraSystem* VFX = SkillData->PathEffect.LoadSynchronous();
		if (!VFX)
		{
			UE_LOG(LogTemp, Error, TEXT("ASkillActor::Initialize - VFX PathEffect invalido para SkillDataAsset '%s'."), *GetNameSafe(SkillData));
			return;
		}

		// Cria o NiagaraComponent e o registra no SkillActor
		
		this->NiagaraComponent->SetAsset(VFX);
		this->NiagaraComponent->SetAutoDestroy(false); // Nós controlamos, não o componente
		this->NiagaraComponent->SetComponentTickEnabled(true);
		
		this->NiagaraComponent->SetFloatParameter(FName("ChargeRatio"), InSkillContext.ChargeRatio);
		// Seta o objeto callback
		this->NiagaraComponent->SetVariableObject(FName("CallbackObject"), this);
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
		this->NiagaraComponent->SetVariableObject(FName("CallbackObject"), nullptr);
		this->NiagaraComponent->Deactivate();
	}
	
	this->SkillContext.HitOverlapResultType = EHitOverlapResultType::Hit;
	this->SkillContext.HitResult = Hit;
	SkillInstance->OnHitSurface.Broadcast(this->SkillContext, Hit.ImpactPoint);
}

void ASkillActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
};
