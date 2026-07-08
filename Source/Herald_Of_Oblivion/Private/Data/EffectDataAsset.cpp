// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/EffectDataAsset.h"
#include "Core/EntityClass.h"
#include "Core/EffectInstance.h"
#include "Subsystems/GameInstanceClass.h"
#include "EffectFeatures/Activation/ActivationEffectFeature.h"
#include "EffectFeatures/Execution/ExecutionEffectFeature.h"
#include "Subsystems/PoolingManager.h"


UEffectInstance* UEffectDataAsset::GetInstance(AActor* InOriginatingEntity, AEntityClass* InTargetEntity)
{
	if (!IsValid(InTargetEntity)) return nullptr;
	
	if (UWorld* World = InTargetEntity->GetWorld())
	{
		UPoolingManager* PoolingManager = Cast<UPoolingManager>(World->GetGameInstance()->GetSubsystem<UPoolingManager>());
		
		if (UEffectInstance* Instance = Cast<UEffectInstance>(PoolingManager->GetObjectFromPool(UEffectInstance::StaticClass())))
		{
			Instance->Initialize(this, InOriginatingEntity, InTargetEntity, 
			                     DuplicateObject<UActivationEffectFeature>(ActivationFeature, Instance), DuplicateObject<UExecutionEffectFeature>(ExecutionFeature, Instance));
			UE_LOG(LogTemp, Log, TEXT("UEquipmentDataAsset::GetInstance Instância de Effect inicializada."));
			return Instance;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("UEquipmentDataAsset::GetInstance Error"));
	return nullptr;
}
