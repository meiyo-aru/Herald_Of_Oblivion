// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerClass.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "Data/SurfacePhysMaterialClass.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SkillFeatures/Activation/ActivationFeature.h"
#include "Subsystems/GameInstanceClass.h"

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
	
	// Seta o Spring Arm para o modo de visão ThirdPerson
	this->SpringArm->SetRelativeLocation(FVector(0.0f,0.0f,80.0f));
	this->SpringArm->TargetArmLength = 110.0f;
	this->SpringArm->SocketOffset = FVector(0.0f,50.0f,0.0f);
	
	// Seta a rotação para ser compatível com o ThirdPerson view
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
	
	// Adiciona o MappingContext para utilizar os InputActions	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				UInputMappingContext* Mapping = this->DefaultMappingContext.LoadSynchronous();
				if (Mapping)
				{
					Subsystem->AddMappingContext(Mapping, 0);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("APlayerClass::BeginPlay - Failed to load DefaultMappingContext for %s"), *GetName());
				}
			}
		}
	}
	
	FString SlotName = TEXT("Slot1");
	int8 UserIndex = 0;

	// A função mágica que resolve tudo:
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
			UE_LOG(LogTemp, Error, TEXT("ENTROU NO DECIMA DOGAO"));
	}
	else
	{
		if (UWorld* World = GetWorld())
		{
			UE_LOG(LogTemp, Error, TEXT("ENTROU NO DEBAIXO DOGAO"));
			UGameInstanceClass* GI = Cast<UGameInstanceClass>(World->GetGameInstance());
			GI->InitializeNewPlayer(*this);
		}
	}
}

void APlayerClass::LoadSkillAssets(USkillInstance* SkillInstance, bool bAsync)
{
	Super::LoadSkillAssets(SkillInstance, bAsync);
}

// Called every frame
void APlayerClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Verifica se o Personagem está no chão e atualiza o SurfaceMaterial
	if (this->GetCharacterMovement()->IsMovingOnGround())
	{
		FHitResult Hit;
		FVector Start = this->GetActorLocation();
		FVector End = Start - FVector(0, 0, 200.0f);

		// Configurações da Consulta
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this); // O laser não deve atingir o próprio personagem
		Params.bReturnPhysicalMaterial = true; // Crucial para o seu sistema de velocidade
		
		if (UWorld* World = GetWorld())
		{
			// Execução do Trace
			bool bHit = World->LineTraceSingleByChannel(
				Hit, 
				Start, 
				End, 
				ECC_Visibility, 
				Params
			);
			
			// Atualiza o MaxWalkSpeed com base na superfície que está pisando
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
}

void APlayerClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) return;
    
	if (UInputAction* IAMove = MoveAction.LoadSynchronous())
		EIC->BindAction(IAMove, ETriggerEvent::Triggered, this, &APlayerClass::Move);
    
	if (UInputAction* IAFirstSkill = CastFirstSkillAction.LoadSynchronous())
	{
		EIC->BindAction(IAFirstSkill, ETriggerEvent::Started, this, &APlayerClass::CastFirstSkill);
		EIC->BindAction(IAFirstSkill, ETriggerEvent::Completed, this, &APlayerClass::ReleasedFirstSkill);
	}
	if (UInputAction* IASecondSkill = CastSecondSkillAction.LoadSynchronous())
	{
		EIC->BindAction(IASecondSkill, ETriggerEvent::Started, this, &APlayerClass::CastSecondSkill);
		EIC->BindAction(IASecondSkill, ETriggerEvent::Completed, this, &APlayerClass::ReleasedSecondSkill);
	}
	if (UInputAction* IAThirdSkill = CastThirdSkillAction.LoadSynchronous())
	{
		EIC->BindAction(IAThirdSkill, ETriggerEvent::Started, this, &APlayerClass::CastThirdSkill);
		EIC->BindAction(IAThirdSkill, ETriggerEvent::Completed, this, &APlayerClass::ReleasedThirdSkill);
	}
	if (UInputAction* IAFourthSkill = CastFourthSkillAction.LoadSynchronous())
	{
		EIC->BindAction(IAFourthSkill, ETriggerEvent::Started, this, &APlayerClass::CastFourthSkill);
		EIC->BindAction(IAFourthSkill, ETriggerEvent::Completed, this, &APlayerClass::ReleasedFourthSkill);
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
	if (!InSkillInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerClass::HandleCastSkill - InSkillInstance invalido."));
		return;
	}
	
	if (InSkillInstance->GetIsCasting() || InSkillInstance->GetInCooldown())
		return;

	InSkillInstance->CurrentContext.EntityOwnerClass = this->GetClass();
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

void APlayerClass::TakeSkill(FPrimaryAssetId AssetId)
{
	if (this->HasSkill(AssetId))
	{
		UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
		if (!AssetManager)
		{
			UE_LOG(LogTemp, Error, TEXT("APlayerClass::TakeSkill - AssetManager invalido."));
			return;
		}
		
		TWeakObjectPtr WeakThis(this);
		
		AssetManager->LoadPrimaryAsset(AssetId, TArray<FName>(), FStreamableDelegate::CreateLambda([WeakThis, AssetId, AssetManager]
		{
			if (APlayerClass* StrongThis = WeakThis.Get())
			{
				if (USkillDataAsset* Asset = AssetManager->Get().GetPrimaryAssetObject<USkillDataAsset>(AssetId))
					StrongThis->SkillsInstances.Add(Asset->CreateInstance(StrongThis));
			}
		}));
	}
}

bool APlayerClass::HasSkill(FPrimaryAssetId SkillId) const
{
	// Procura no array de instâncias se alguma delas aponta para o DataAsset com esse ID
	return this->SkillsInstances.ContainsByPredicate([&](const USkillInstance* Instance) {
		return Instance && Instance->GetAssetId() == SkillId;
	});
}

void APlayerClass::ToggleEquipSkill(USkillInstance* InSkillInstance)
{
	if (!this->EquippedSkillsInstances.Contains(InSkillInstance))
	{
		this->LoadSkillAssets(InSkillInstance, true);
		this->EquippedSkillsInstances.Add(InSkillInstance);
		InSkillInstance->Prepare();
	} else
	{
		if (InSkillInstance->SkillsHandle && InSkillInstance->SkillsHandle.IsValid())
		{
			InSkillInstance->SkillsHandle->ReleaseHandle();
			InSkillInstance->SkillsHandle.Reset();
		}
		InSkillInstance->OnSkillCastDelegate.RemoveAll(InSkillInstance->ActivationFeature);
		InSkillInstance->OnSkillReleasedDelegate.RemoveAll(InSkillInstance->ActivationFeature);
		InSkillInstance->OnSkillActivateDelegate.RemoveAll(InSkillInstance->ActivationFeature);
		InSkillInstance->OnSkillHitDelegate.RemoveAll(InSkillInstance->ActivationFeature);
		this->EquippedSkillsInstances.Remove(InSkillInstance);
	}
}

void APlayerClass::EquipSkill(USkillInstance* SkillInstance, bool AsyncFXLoading)
{
// Define as skills iniciais da entidade
	if (!SkillsInstances.Find(SkillInstance))
		SkillsInstances.Add(SkillInstance);
	
	EquippedSkillsInstances.Add(SkillInstance);
	SkillInstance->Prepare();
	LoadSkillAssets(SkillInstance, AsyncFXLoading);
}