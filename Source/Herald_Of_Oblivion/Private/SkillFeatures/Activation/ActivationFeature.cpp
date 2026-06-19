// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Activation/ActivationFeature.h"

#include "Character/PlayerClass.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"

void UActivationFeature::Initialize(USkillInstance* Owner)
{
	Super::Initialize(Owner);
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("UActivationFeature::Initialize - Owner (USkillInstance) invalido."));
		return;
	}
	Owner->OnSkillCastDelegate.AddUObject(this, &UActivationFeature::StartActivation);
	Owner->OnSkillReleasedDelegate.AddUObject(this, &UActivationFeature::CompleteActivation);
}

void UActivationFeature::StartActivation(FSkillContext& InSkillContext)
{
	this->Context = InSkillContext;
	
	// Define o estágio atual da skill como Casting
	InSkillContext.SkillStage = ESkillStage::Casting;
	
	// Captura as informacoes do cursor do mouse, como a localizacao e o SurfaceNormal
	FHitResult HitCursor = GetAimTarget(InSkillContext, 30.0f);
	if (AActor* HittedActor = HitCursor.GetActor())
	{
		if (AEntityClass* Entity = Cast<AEntityClass>(HittedActor))
		{
			InSkillContext.EntityOnStartLocation = Entity;
		} else
		{
			InSkillContext.EntityOnEndLocation = nullptr;
		}
	}
	
	InSkillContext.StartSurfaceNormal = HitCursor.ImpactNormal;
	InSkillContext.StartLocation = HitCursor.ImpactPoint;
	
	InSkillContext.CastTime = GetWorld()->GetTimeSeconds();
}

void UActivationFeature::CompleteActivation(FSkillContext& InSkillContext)
{
	// 1. Se o timer ainda existir (porque o jogador soltou o botão antes do tempo), limpe-o
	if (GetWorld()->GetTimerManager().IsTimerActive(this->TimerHandle))
		GetWorld()->GetTimerManager().ClearTimer(this->TimerHandle);
	
	if (InSkillContext.bActivated || !InSkillContext.SkillInstance.IsValid()) return;
			
	// Captura as informacoes do cursor do mouse, como a localizacao e o SurfaceNormal
	
	FHitResult HitCursor = GetAimTarget(InSkillContext, 30.0f);
	if (AActor* HittedActor = HitCursor.GetActor())
	{
			UE_LOG(LogTemp, Warning, TEXT("bateu no ator: %s"), *HittedActor->GetName());
		if (AEntityClass* Entity = Cast<AEntityClass>(HittedActor))
		{
			InSkillContext.EntityOnEndLocation = Entity;
		} else
		{
			InSkillContext.EntityOnEndLocation = nullptr;
		}
	}
	
	InSkillContext.EndSurfaceNormal = HitCursor.ImpactNormal;
	InSkillContext.EndLocation = HitCursor.ImpactPoint;
	
	InSkillContext.Direction = (InSkillContext.EndLocation - InSkillContext.StartLocation).GetSafeNormal();
	InSkillContext.ReleasedTime = GetWorld()->GetTimeSeconds();
	InSkillContext.HoldDuration = InSkillContext.ReleasedTime - InSkillContext.CastTime;
}

void UActivationFeature::OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	Super::OnNiagaraSystemFinished(FinishedComponent);
}

void UActivationFeature::OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent)
{
	Super::OnAuraNiagaraSystemFinished(FinishedComponent);
}


FHitResult UActivationFeature::GetAimTarget(FSkillContext& InContext, float Sensibility) const
{
	FHitResult HitResult;
	
	if (APlayerClass* Char = Cast<APlayerClass>(InContext.EntityOwner))
	{
		FVector Start = Char->GetCameraComponent()->GetComponentLocation();
		FVector End = Start + Char->GetCameraComponent()->GetForwardVector() * 10000;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Char);
		GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::MakeFromRotator(FRotator::ZeroRotator),ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(Sensibility), CollisionParams);
	}
	return HitResult;
}

FHitResult UActivationFeature::GetCursorLocation(FSkillContext& InContext) const
{
	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	
	FHitResult HitCursor;
	
	// Captura a posição do mouse no mundo
	if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitCursor))
	{
		HitCursor.Location.Z += 10;
	}
	return HitCursor;
}

void UActivationFeature::CleanNiagara()
{
	Super::CleanNiagara();
}
