// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SkillInstance.h"

#include "NiagaraDataInterfaceExport.h"
#include "Core/EntityClass.h"
#include "Data/SkillDataAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "SkillFeatures/Activation/ActivationFeature.h"
#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "SkillFeatures/OnHit/OnHitFeature.h"

USkillInstance::USkillInstance()
{
}

void USkillInstance::BeginDestroy()
{
	Super::BeginDestroy();
}

void USkillInstance::CastSkill()
{
	this->bIsCasting = true;
	this->CurrentContext.Reset();
	OnSkillCastDelegate.RemoveAll(this); 
	OnSkillCastDelegate.Broadcast(this->CurrentContext);
}

void USkillInstance::GoOnCooldown()
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		// 1. Limpa o timer se ele já estiver rodando (evita bugs de múltiplos disparos)
		World->GetTimerManager().ClearTimer(this->TimerHandle);
		
		this->bInCooldown = true;
		this->bIsCasting = false;
		
		UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
		if (!AssetManager)
		{
			UE_LOG(LogTemp, Error, TEXT("USkillInstance::CastSkill - AssetManager invalido."));
			return;
		}
		

		float CDTime = DataAsset->Cooldown;
		if (this->CooldownReduce > 0.0f)
			CDTime *= this->CooldownReduce/100;

		TWeakObjectPtr WeakThis(this);
		
		World->GetTimerManager().SetTimer(
			this->TimerHandle, 
			[WeakThis]() mutable
			{
				if (!WeakThis.IsValid())
				{
					return;
				}

				WeakThis->bInCooldown = false;
			},
			 CDTime,
			false
			);
		
		
	}
}

void USkillInstance::Initialize(AEntityClass* InOwner, USkillDataAsset* InDataAsset, UActivationFeature* InActivationFeature, UExecutionFeature* InExecutionFeature, TArray<UOnHitFeature*> InOnHitFeature)
{
	this->DataAsset = InDataAsset;
	this->ActivationFeature = InActivationFeature;
	this->ExecutionFeature = InExecutionFeature;
	this->OnHitFeature = InOnHitFeature;
	this->Owner = InOwner;
	this->CurrentContext = FSkillContext(this, InOwner);
}

void USkillInstance::Prepare()
{
	if (!IsValid(this->ActivationFeature))
	{
		UE_LOG(LogTemp, Error, TEXT("USkillInstance::InitializeFeatures - ActivationFeature invalida para SkillDataAsset."));
		return;
	}

	if (!IsValid(this->ExecutionFeature))
	{
		UE_LOG(LogTemp, Error, TEXT("USkillInstance::InitializeFeatures - ExecutionFeature invalida para SkillDataAsset."));
		return;
	}
	
	this->ActivationFeature->Initialize(this);
	this->ExecutionFeature->Initialize(this);

	if (!this->OnHitFeature.IsEmpty())
	{
		for (UOnHitFeature* Feature : this->OnHitFeature)
		{
			Feature->Initialize(this);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("USkillInstance::InitializeFeatures - OnHitFeature nao configurada para SkillDataAsset."));
	}
	
	UE_LOG(LogTemp, Error, TEXT("Features de %s preparada"), *this->GetName());
}

void USkillInstance::PrepareForPooling()
{
	// Limpa os delegates
	this->OnSkillCastDelegate.Clear();
	this->OnSkillReleasedDelegate.Clear();
	this->OnSkillActivateDelegate.Clear();
	this->OnSkillHitDelegate.Clear();
	
	// Reseta variaveis
	this->Owner = nullptr;
	this->Level = 1;
	this->CooldownReduce = 0;
	this->bInCooldown = false;
	this->bIsCasting = false;
	this->ForceMultiplier = 1.0f;
}