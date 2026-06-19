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
	// Verificamos se o mundo ainda existe e limpamos o timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(this->TimerHandle);
	}

	Super::BeginDestroy();
}

void USkillInstance::CastSkill()
{
	this->bIsCasting = true;
	this->CurrentContext.Reset();
	
	UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
	if (!AssetManager)
	{
		UE_LOG(LogTemp, Error, TEXT("USkillInstance::CastSkill - AssetManager invalido."));
		return;
	}

	TArray<FName> ExecutionBundles = {
		FName("ExecutionVFX"), FName("ExecutionSFX"),
		FName("PersistentVFX"), FName("PersistentSFX")
	};
	
	const USkillDataAsset* Data = this->GetDataAsset();
	
	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("USkillInstance::CastSkill - SkillDataAsset invalido."));
		return;
	}
	

	if (!this->EndingHandle || !this->EndingHandle.IsValid())
	{
		this->EndingHandle = AssetManager->LoadPrimaryAsset(
			Data->GetPrimaryAssetId(),
			ExecutionBundles,
			FStreamableDelegate::CreateLambda([Data]
			{
				UE_LOG(LogTemp, Log, TEXT("Execution Asset carregado: %s"), *Data->GetName());	
			})
		);
	}	
	

	this->OnSkillCastDelegate.Broadcast(this->CurrentContext);

	/*
	if (!this->AuraHandle || !this->AuraHandle.IsValid())
	{
		if (Data->ExecutionFeature)
		{
			TArray<FName> AuraBundles;
			if (Data->ExecutionFeature->bAura)
				AuraBundles.Add("AuraVFX");
			
			this->AuraHandle = AssetManager->LoadPrimaryAsset(
				Data->GetPrimaryAssetId(),
				AuraBundles,
				FStreamableDelegate::CreateLambda([Data]
				{
					UE_LOG(LogTemp, Log, TEXT("Aura Assets carregados: %s"), *Data->GetName());	
				})
			);
		}
	}*/
}

void USkillInstance::GoOnCooldown()
{
	// 1. Limpa o timer se ele já estiver rodando (evita bugs de múltiplos disparos)
	GetWorld()->GetTimerManager().ClearTimer(this->TimerHandle);
	
	UE_LOG(LogTemp,Warning, TEXT("GoOnCooldown!!!"));
	this->bInCooldown = true;
	this->bIsCasting = false;

	float CDTime = this->DataAsset->Cooldown;
	if (this->CooldownReduce > 0.0f)
		CDTime *= this->CooldownReduce/100;

	TWeakObjectPtr<USkillInstance> WeakThis(this);
		
	GetWorld()->GetTimerManager().SetTimer(
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
void USkillInstance::FinishSkill()
{
	this->ReleaseEndingHandle();
}
void USkillInstance::Initialize(AEntityClass* InOwner, USkillDataAsset* InDataAsset, UActivationFeature* InActivationFeature, UExecutionFeature* InExecutionFeature, UOnHitFeature* InOnHitFeature)
{
	this->DataAsset = InDataAsset;
	this->ActivationFeature = InActivationFeature;
	this->ExecutionFeature = InExecutionFeature;
	this->OnHitFeature = InOnHitFeature;
	this->Owner = InOwner;
	this->CurrentContext = FSkillContext(this, InOwner);
}

void USkillInstance::InitializeFeatures()
{
	if (!IsValid(this->ActivationFeature))
	{
		UE_LOG(LogTemp, Error, TEXT("USkillInstance::InitializeFeatures - ActivationFeature invalida para SkillDataAsset '%s'."), *GetNameSafe(this->DataAsset));
		return;
	}

	if (!IsValid(this->ExecutionFeature))
	{
		UE_LOG(LogTemp, Error, TEXT("USkillInstance::InitializeFeatures - ExecutionFeature invalida para SkillDataAsset '%s'."), *GetNameSafe(this->DataAsset));
		return;
	}
	
	this->ActivationFeature->Initialize(this);
	this->ExecutionFeature->Initialize(this);

	if (IsValid(this->OnHitFeature))
	{
		this->OnHitFeature->Initialize(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("USkillInstance::InitializeFeatures - OnHitFeature nao configurada para SkillDataAsset '%s'."), *GetNameSafe(this->DataAsset));
	}
}

void USkillInstance::PrepareForPooling()
{
	// Limpa os delegates
	this->OnSkillCastDelegate.Clear();
	this->OnSkillReleasedDelegate.Clear();
	this->OnSkillActivateDelegate.Clear();
	this->OnSkillHitDelegate.Clear();
	
	this->ReleaseCastingHandle();
	this->ReleaseEndingHandle();
	
	// Reseta variaveis
	this->Owner = nullptr;
	this->Level = 1;
	this->CooldownReduce = 0;
	this->bInCooldown = false;
	this->bIsCasting = false;
	this->ForceMultiplier = 1.0f;
}

void USkillInstance::ReleaseCastingHandle()
{
	if (!CastingHandle || !this->CastingHandle.IsValid()) return; // Early return, nada a fazer

	UE_LOG(LogTemp, Log, TEXT("Casting Assets Liberados da memória"))
	
	this->CastingHandle->ReleaseHandle();
	this->CastingHandle.Reset(); 
}

void USkillInstance::ReleaseEndingHandle()
{
	if (!EndingHandle || !this->EndingHandle.IsValid()) return; // Early return, nada a fazer
	
	UE_LOG(LogTemp, Log, TEXT("Ending Assets Liberados da memória"))
	
	this->EndingHandle->ReleaseHandle();
	this->EndingHandle.Reset(); 
}

void USkillInstance::ReleaseAuraHandle()
{
	if (!AuraHandle || !this->AuraHandle.IsValid()) return; // Early return, nada a fazer
	
	UE_LOG(LogTemp, Log, TEXT("Aura Assets Liberados da memória"))
	
	this->AuraHandle->ReleaseHandle();
	this->AuraHandle.Reset(); 
}

