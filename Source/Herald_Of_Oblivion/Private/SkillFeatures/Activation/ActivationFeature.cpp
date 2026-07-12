// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFeatures/Activation/ActivationFeature.h"

#include "Character/PlayerClass.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"

void UActivationFeature::LoadFXSync()
{
	LoadedActivationEffect = ActivationEffect.LoadSynchronous();
	WarmupNiagara(LoadedActivationEffect);
	LoadedSoundCue = ActivationSound.LoadSynchronous();
}

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
	// Define o estágio atual da skill como Casting
	InSkillContext.SkillStage = ESkillStage::Casting;
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->GetTimerManager().ClearTimer(this->CleanNiagaraTimerHandle);
		World->GetTimerManager().ClearTimer(this->TimerHandle);

		// Captura as informacoes do cursor do mouse, como a localizacao e o SurfaceNormal
		FHitResult HitCursor = GetAimTarget(InSkillContext, AimRadius);
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
		
		InSkillContext.CastTime = World->GetTimeSeconds();
	}
}

void UActivationFeature::CompleteActivation(FSkillContext& InSkillContext)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		// 1. Se o timer ainda existir (porque o jogador soltou o botão antes do tempo), limpe-o
		if (World->GetTimerManager().IsTimerActive(this->TimerHandle))
			World->GetTimerManager().ClearTimer(this->TimerHandle);
		
		if (InSkillContext.bActivated || !InSkillContext.SkillInstance.IsValid()) return;
				
		// Captura as informacoes do cursor do mouse, como a localizacao e o SurfaceNormal
		
		FHitResult HitCursor = GetAimTarget(InSkillContext, AimRadius);
		if (AActor* HittedActor = HitCursor.GetActor())
		{
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
		InSkillContext.ReleasedTime = World->GetTimeSeconds();
		InSkillContext.HoldDuration = InSkillContext.ReleasedTime - InSkillContext.CastTime;
	}
}




FHitResult UActivationFeature::GetAimTarget(FSkillContext& InContext, float InAimRadius) const
{
	FHitResult HitResult;
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (APlayerClass* Char = Cast<APlayerClass>(InContext.EntityOwner.Get()))
		{
			FVector Start = Char->GetCameraComponent()->GetComponentLocation();
			FVector End = Start + Char->GetCameraComponent()->GetForwardVector() * 50000;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(Char);
			
			World->SweepSingleByChannel(HitResult, Start, End, FQuat::MakeFromRotator(FRotator::ZeroRotator),ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(InAimRadius), CollisionParams);
		}
	}
	return HitResult;
}

FHitResult UActivationFeature::GetCursorLocation(FSkillContext& InContext) const
{
	FHitResult HitCursor;
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		APlayerController* PC = Cast<APlayerController>(World->GetFirstPlayerController());
		
		
		// Captura a posição do mouse no mundo
		if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitCursor))
		{
			HitCursor.Location.Z += 10;
		}
	}
	return HitCursor;
}

void UActivationFeature::CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents)
{
	Super::CleanNiagara(SpawnedNiagaraComponents);
}
