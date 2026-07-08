// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EffectInstance.h"
#include "Core/EntityClass.h"
#include "Subsystems/GameInstanceClass.h"
#include "EffectFeatures/Activation/ActivationEffectFeature.h"
#include "EffectFeatures/Execution/ExecutionEffectFeature.h"
#include "Subsystems/PoolingManager.h"

void UEffectInstance::ApplyOnTargetEntity(FHitOverlapResult& HitOverlapResult)
{
	if (IsValid(ActivationFeature))
	{
		ActivationFeature->Init(HitOverlapResult);
	}
}

void UEffectInstance::Initialize(UEffectDataAsset* InDataAsset,
	AActor* InOriginatingEntity, AEntityClass* InTargetEntity, UActivationEffectFeature* InActivationFeature,
	UExecutionEffectFeature* InExecutionFeature)
{
	DataAsset = InDataAsset;
	OriginatingActor = InOriginatingEntity;
	TargetEntity = InTargetEntity;
	ActivationFeature = InActivationFeature;
	ExecutionFeature = InExecutionFeature;
}

void UEffectInstance::SaveToPool()
{
	DataAsset = nullptr;
	OriginatingActor = nullptr;
	TargetEntity = nullptr;
	ActivationFeature = nullptr;
	ExecutionFeature = nullptr;
	
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		UPoolingManager* PoolingManager = Cast<UPoolingManager>(World->GetGameInstance()->GetSubsystem<UPoolingManager>());
		PoolingManager->SaveObjectInPool(this->GetClass(),this);
	}
}
