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

	TArray InitialFXBundles = {
		FName("ExecutionVFX"), FName("ExecutionSFX"),
		FName("OnHitVFX"), FName("OnHitSFX")
	};
	
	TWeakObjectPtr WeakThis(this);
	AssetManager->LoadPrimaryAsset(
		GetAssetId(),
		InitialFXBundles,
		FStreamableDelegate::CreateLambda([WeakThis]
		{
			if (USkillInstance* StrongThis = WeakThis.Get())
			{
				// Remove qualquer binding fantasma/antigo que perdeu a referência antes de rodar o broadcast
				StrongThis->OnSkillCastDelegate.RemoveAll(StrongThis); 
				StrongThis->OnSkillCastDelegate.Broadcast(StrongThis->CurrentContext);
				UE_LOG(LogTemp, Log, TEXT("Execution e OnHit Assets carregados")); 
			}
		})
	);
	
}

void USkillInstance::GoOnCooldown()
{
	// 1. Limpa o timer se ele já estiver rodando (evita bugs de múltiplos disparos)
	GetWorld()->GetTimerManager().ClearTimer(this->TimerHandle);
	
	this->bInCooldown = true;
	this->bIsCasting = false;
	
	UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
	if (!AssetManager)
	{
		UE_LOG(LogTemp, Error, TEXT("USkillInstance::CastSkill - AssetManager invalido."));
		return;
	}
	
	// Não precisa carregar pois podemos assumir que o Cast da skill ja carregou o asset na memoria
	if (USkillDataAsset* DataAsset = AssetManager->GetPrimaryAssetObject<USkillDataAsset>(AssetId))
	{
		float CDTime = DataAsset->Cooldown;
		if (this->CooldownReduce > 0.0f)
			CDTime *= this->CooldownReduce/100;

		TWeakObjectPtr WeakThis(this);
			
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
}

void USkillInstance::Initialize(AEntityClass* InOwner, FPrimaryAssetId InAssetId, UActivationFeature* InActivationFeature, UExecutionFeature* InExecutionFeature, TArray<UOnHitFeature*> InOnHitFeature)
{
	this->AssetId = InAssetId;
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
}

void USkillInstance::PrepareForPooling()
{
	// Limpa os delegates
	this->OnSkillCastDelegate.Clear();
	this->OnSkillReleasedDelegate.Clear();
	this->OnSkillActivateDelegate.Clear();
	this->OnSkillHitDelegate.Clear();
	
	this->ReleaseActivationHandle();
	/*
	this->ReleaseOnHitHandle();
	*/
	
	// Reseta variaveis
	this->Owner = nullptr;
	this->Level = 1;
	this->CooldownReduce = 0;
	this->bInCooldown = false;
	this->bIsCasting = false;
	this->ForceMultiplier = 1.0f;
}

void USkillInstance::ReleaseActivationHandle()
{
	if (!ActivationHandle || !this->ActivationHandle.IsValid()) return; // Early return, nada a fazer

	UE_LOG(LogTemp, Log, TEXT("Casting Assets Liberados da memória"))
	
	this->ActivationHandle->ReleaseHandle();
	this->ActivationHandle.Reset(); 
}

/*void USkillInstance::ReleaseOnHitHandle()
{
	if (!OnHitHandle || !this->OnHitHandle.IsValid()) return; // Early return, nada a fazer
	
	UE_LOG(LogTemp, Log, TEXT("Ending Assets Liberados da memória"))
	
	this->OnHitHandle->ReleaseHandle();
	this->OnHitHandle.Reset(); 
}*/

void USkillInstance::ReleaseAuraHandle()
{
	if (!EntityOwnerAuraHandle || !this->EntityOwnerAuraHandle.IsValid()) return; // Early return, nada a fazer
	
	UE_LOG(LogTemp, Log, TEXT("Aura Assets Liberados da memória"))
	
	this->EntityOwnerAuraHandle->ReleaseHandle();
	this->EntityOwnerAuraHandle.Reset(); 
}

