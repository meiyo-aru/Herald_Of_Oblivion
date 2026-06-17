// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerClass.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "Data/SurfacePhysMaterialClass.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SkillFeatures/Activation/ActivationCastWithHoldFeature.h"
#include "SkillFeatures/Activation/ActivationFeature.h"
#include "SkillFeatures/Execution/ExecutionFeature.h"

// Sets default values
APlayerClass::APlayerClass() : MaxZoom(1200.0f), 
	  MinZoom(300.0f), 
	  ZoomInterpSpeed(5.0f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Define o SpringArm 
	this->SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	this->SpringArm->SetupAttachment(RootComponent);
	
	// Define o Zoom base como a distancia inicial do SpringArm
	this->DesiredZoom = 0.0f;
	
	this->SpringArm->SetRelativeLocation(FVector(0.0f,0.0f,80.0f));
	
	this->SpringArm->TargetArmLength = 110.0f;
	this->SpringArm->SocketOffset = FVector(0.0f,50.0f,0.0f);
	
	this->SpringArm->bUsePawnControlRotation = true;
	
	this->bUseControllerRotationYaw = true;
	
	// Define a camera
	this->Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	this->Camera->SetupAttachment(SpringArm);
	
	this->GetCharacterMovement()->MaxWalkSpeed = 300;
	this->GetCharacterMovement()->MaxWalkSpeedCrouched = 150;
}

// Called when the game starts or when spawned
void APlayerClass::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				// Add with priority 0; higher values override lower ones
				UInputMappingContext* Mapping = this->DefaultMappingContext.LoadSynchronous();
				if (Mapping)
				{
					Subsystem->AddMappingContext(Mapping, 0);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("APlayerClass::BeginPlay - Failed to load DefaultMappingContext for %s"), *GetName());
				}
			}
		}
	}
}

void APlayerClass::LoadCastingSkillAssets(USkillInstance* SkillInstance)
{
	
	UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
	if (!AssetManager)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerClass::LoadSkillAssets - AssetManager invalido."));
		return;
	}
	
	TArray<FName> CastingBundles = {
		FName("CastVFX"), FName("CastSFX")
	};

	const USkillDataAsset* Data = SkillInstance->GetDataAsset();
	
	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerClass::LoadSkillAssets - SkillDataAsset invalido para SkillInstance '%s'."), *GetNameSafe(SkillInstance));
		return;
	}
	

	if (!SkillInstance->CastingHandle || !SkillInstance->CastingHandle.IsValid())
	{
		SkillInstance->CastingHandle = AssetManager->LoadPrimaryAsset(
			Data->GetPrimaryAssetId(),
			CastingBundles,
			FStreamableDelegate::CreateLambda([Data]()
			{
				UE_LOG(LogTemp, Log, TEXT("Casting Asset carregado: %s"), *Data->GetName());	
			})
		);
	}	
	
	
	TArray<FName> AuraBundles;
	
	if (UActivationCastWithHoldFeature* CastWithHoldFeature = Cast<UActivationCastWithHoldFeature>(Data->ActivationFeature))
	{
		if (CastWithHoldFeature->bAura)
		{
			if (!SkillInstance->AuraHandle || !SkillInstance->AuraHandle.IsValid())
			{
				AuraBundles.Add(FName("AuraVFX"));
			}
		}
	}
	


	SkillInstance->AuraHandle = AssetManager->LoadPrimaryAsset(
		Data->GetPrimaryAssetId(),
		AuraBundles,
		FStreamableDelegate::CreateLambda([Data]()
		{
			UE_LOG(LogTemp, Log, TEXT("Aura Asset carregado: %s"), *Data->GetName());	
		})
	);
}

// Called every frame
void APlayerClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->GetCharacterMovement()->IsMovingOnGround())
	{
		FHitResult Hit;
		FVector Start = this->GetActorLocation();
		FVector End = Start - FVector(0, 0, 200.0f);

		// Configurações da Consulta
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this); // O laser não deve atingir o próprio personagem
		Params.bReturnPhysicalMaterial = true; // Crucial para o seu sistema de velocidade
		
		// Execução do Trace
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			Hit, 
			Start, 
			End, 
			ECC_Visibility, 
			Params
		);
		
		if (bHit)
		{
			if (this->SurfaceMaterial != Hit.PhysMaterial.Get() && Hit.PhysMaterial.Get() != nullptr)
			{
				this->SurfaceMaterial = Hit.PhysMaterial.Get();
				USurfacePhysMaterialClass* PhysMaterial = Cast<USurfacePhysMaterialClass>(this->SurfaceMaterial);
				
				if (PhysMaterial)
				{  
					float Speed = this->GetCharacterMovement()->MaxWalkSpeed;
					this->GetCharacterMovement()->MaxWalkSpeed = PhysMaterial->SurfaceSpeedModifier * this->TrueAttributes[EEntityTrueAttributeEnum::MovementSpeed].GetAttributeValue();
				} else
				{
					this->GetCharacterMovement()->MaxWalkSpeed = this->TrueAttributes[EEntityTrueAttributeEnum::MovementSpeed].GetAttributeValue();
				}
			} 
			
		}
	}
}

// Called to bind functionality to input
void APlayerClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) return;
    
	if (UInputAction* IAMove = MoveAction.LoadSynchronous())
		EIC->BindAction(IAMove, ETriggerEvent::Triggered, this, &APlayerClass::Move);
    
	if (UInputAction* IAFirstSkill = CastSkillAction.LoadSynchronous())
	{
		EIC->BindAction(IAFirstSkill, ETriggerEvent::Started, this, &APlayerClass::CastFirstSkill);
		EIC->BindAction(IAFirstSkill, ETriggerEvent::Completed, this, &APlayerClass::ReleasedFirstSkill);
	}
	
	if (UInputAction* IAMouseLook = MouseLookAction.LoadSynchronous())
	{
		EIC->BindAction(IAMouseLook, ETriggerEvent::Triggered, this, &APlayerClass::MouseLook);
	}	
}

// void APlayerClass::HandleCastQuickAccess(int slot)
// {
// 	
// }

void APlayerClass::HandleCastSkill(USkillInstance* InSkillInstance)
{
	OnHealthChanged.Execute(50.0f);
	
	if (!InSkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerClass::HandleCastSkill - InSkillInstance invalido."));
		return;
	}

	if (InSkillInstance->GetIsCasting() || InSkillInstance->GetInCooldown())
		return;

	InSkillInstance->CastSkill();
}

void APlayerClass::HandleReleasedSkill(USkillInstance* InSkillInstance)
{
	if (!InSkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerClass::HandleReleasedSkill - InSkillInstance invalido."));
		return;
	}

	if (!InSkillInstance->GetIsCasting())
		return;

	InSkillInstance->FinishCast();
	
	if (!InSkillInstance->CurrentContext.SkillInstance.IsValid())
		return;
	
	InSkillInstance->OnSkillReleasedDelegate.Broadcast(InSkillInstance->CurrentContext);
	
}

void APlayerClass::Move(const FInputActionValue& Value)
{
	// Valor do input (1D axis ou 2D axis)
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	// Aplicar movimento baseado na câmera/controle do personagem
	if (MovementVector.Y != 0.0f)
	{
		// Movimento forward/backward (relativo à câmera)
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	}
	
	if (MovementVector.X != 0.0f)
	{
		// Movimento left/right (relativo à câmera)
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void APlayerClass::MouseLook(const FInputActionValue& Value)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		const FVector2D LookValue = Value.Get<FVector2D>();
		
		// Rotação Yaw (esquerda/direita)
		if (LookValue.X != 0.0f)
		{
			PC->AddYawInput(LookValue.X);
		}
		
		// Rotação Pitch (cima/baixo)
		if (LookValue.Y != 0.0f)
		{
			PC->AddPitchInput(-LookValue.Y);
		}
	}
}


// Define os atributos da entidade
void APlayerClass::DefineAttributes()
{
	Super::DefineAttributes();
}

void APlayerClass::UpdateSimbolicAttribute(EEntitySimbolicAttributeEnum InSimbolicAttribute)
{
	FAttribute SimbolicAttribute = this->SimbolicAttributes[InSimbolicAttribute];
	TArray<FAffectedAttributeStruct> AffectedAttributes = SimbolicAttribute.GetAffectedAttributes();

	for (auto AffectedAttribute : AffectedAttributes)
	{
		UpdateTrueAttribute(AffectedAttribute.GetAffectedAttribute(), AffectedAttribute.GetMultiplier() * SimbolicAttribute.GetAttributeValue());
	}
}

void APlayerClass::UpdateTrueAttribute(EEntityTrueAttributeEnum TargetTrueAttribute, float NewValue)
{
	FAttribute* TrueAttribute = &this->TrueAttributes[TargetTrueAttribute];
	TrueAttribute->UpdateAttributeValue(NewValue);
}

void APlayerClass::IncrementSimbolicAttribute(EEntitySimbolicAttributeEnum TargetSimbolicAttribute)
{
	FAttribute SimbolicAttribute = this->SimbolicAttributes[TargetSimbolicAttribute];
	SimbolicAttribute.UpdateAttributeValue(1);
	UpdateSimbolicAttribute(TargetSimbolicAttribute);
}

void APlayerClass::TakeSkill(FPrimaryAssetId SkillId)
{
	if (this->HasSkill(SkillId))
	{
		UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
		if (!AssetManager)
		{
			UE_LOG(LogTemp, Error, TEXT("APlayerClass::TakeSkill - AssetManager invalido."));
			return;
		}
		
		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &APlayerClass::OnSkillLoaded, SkillId);

		// Inicia o carregamento em massa
		AssetManager->LoadPrimaryAsset(SkillId, TArray<FName>({FName("VFX")}), Delegate);
	}
}

void APlayerClass::OnSkillLoaded(FPrimaryAssetId LoadedId)
{
	UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
	if (!AssetManager)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerClass::OnSkillLoaded - AssetManager invalido para '%s'."), *LoadedId.ToString());
		return;
	}

	// GetPrimaryAssetObject busca exatamente esse ID específico
	if (USkillDataAsset* SkillData = Cast<USkillDataAsset>(AssetManager->GetPrimaryAssetObject(LoadedId)))
	{
		this->SkillsInstances.Add(SkillData->CreateInstance(this));
	}
}

bool APlayerClass::HasSkill(FPrimaryAssetId SkillId) const
{
	// Procura no array de instâncias se alguma delas aponta para o DataAsset com esse ID
	return this->SkillsInstances.ContainsByPredicate([&](const USkillInstance* Instance) {
		return Instance && Instance->GetDataAsset()->GetPrimaryAssetId() == SkillId;
	});
}

void APlayerClass::ToggleEquipSkill(USkillInstance* InSkillInstance)
{
	if (!this->EquippedSkillsInstances.Contains(InSkillInstance))
	{
		this->LoadCastingSkillAssets(InSkillInstance);
		this->EquippedSkillsInstances.Add(InSkillInstance);
	} else
	{
		InSkillInstance->ReleaseAuraHandle();
		InSkillInstance->ReleaseCastingHandle();
		InSkillInstance->ReleaseEndingHandle();
		this->EquippedSkillsInstances.Remove(InSkillInstance);
	}
}

void APlayerClass::DefineSkills()
{

	if (this->Level == 1)
	{
		UE_LOG(LogTemp, Error, TEXT("AEntityClass::DefineSkills 1"));
		
		// Obtém a instância global do Asset Manager. 
		UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
		if (!AssetManager)
		{
			UE_LOG(LogTemp, Error, TEXT("APlayerClass::DefineSkills - AssetManager invalido."));
			return;
		}
		
		TArray<FAssetData> AssetDataList;
		TArray<FPrimaryAssetId> PlayerInitialSkills;

		AssetManager->GetPrimaryAssetDataList(FPrimaryAssetType("Skill"), AssetDataList);
		
		for (const FAssetData& Data : AssetDataList)
		{
		UE_LOG(LogTemp, Error, TEXT("AEntityClass::DefineSkills 2"));
			
			FName SpecializationValue;
			bool bIsInitialValue = false;
	       
			this->UISkills.Add(Data.GetPrimaryAssetId());
			
			if (Data.GetTagValue("SpecializationName", SpecializationValue) && 
				Data.GetTagValue("bIsInitial", bIsInitialValue))
			{
		UE_LOG(LogTemp, Error, TEXT("AEntityClass::DefineSkills 3"));
				
				if (SpecializationValue == this->SpecializationName && bIsInitialValue)
				{
		UE_LOG(LogTemp, Error, TEXT("AEntityClass::DefineSkills 4"));
					
					PlayerInitialSkills.Add(Data.GetPrimaryAssetId());
				}
			}
		}
		
		// Criamos o delegate. 
		// O parâmetro extra (SkillIds) é passado para que a função de callback saiba o que foi carregado.
		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &APlayerClass::OnAllSkillsLoaded, PlayerInitialSkills);

		// Inicia o carregamento em massa
		AssetManager->LoadPrimaryAssets(PlayerInitialSkills, TArray<FName>(), Delegate);
	}
}

void APlayerClass::OnAllSkillsLoaded(TArray<FPrimaryAssetId> LoadedIds)
{
	Super::OnAllSkillsLoaded(LoadedIds);	
	this->EquippedSkillsInstances.Append(this->SkillsInstances);
	this->InitializeSkills(this->EquippedSkillsInstances);
	
	for (USkillInstance* EquippedSkillsInstance : this->EquippedSkillsInstances)
	{
		this->LoadCastingSkillAssets(EquippedSkillsInstance);
	}
}
