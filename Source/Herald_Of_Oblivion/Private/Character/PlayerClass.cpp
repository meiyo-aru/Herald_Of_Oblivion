// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerClass.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "Data/AnimationDataAsset.h"
#include "Data/SurfacePhysMaterialClass.h"
#include "Engine/AssetManager.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SkillFeatures/Activation/ActivationFeature.h"
#include "Subsystems/GameInstanceClass.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerClass::APlayerClass()
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
	this->bUseControllerRotationYaw = false;
	
	// Define a camera
	this->Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	this->Camera->SetupAttachment(SpringArm);
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
			if (UGameInstanceClass* GI = Cast<UGameInstanceClass>(World->GetGameInstance()))
				GI->InitializeNewPlayer(*this);
		}
	}
	
		
	this->GetCharacterMovement()->MaxWalkSpeed = MaxWalkMoveSpeed;
	this->GetCharacterMovement()->MaxWalkSpeedCrouched = 150;	
	this->GetCharacterMovement()->RotationRate = DefaultRotationRate;
}

void APlayerClass::LoadSkillAssets(USkillInstance* SkillInstance, bool bAsync)
{
	Super::LoadSkillAssets(SkillInstance, bAsync);
}

// Called every frame
void APlayerClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsTurningInPlace || bIsHardTurningInPlace)
	{
		FRotator CurrentRotation = GetActorRotation();

		FRotator NewRotation = CurrentRotation;
		NewRotation.Pitch = 0.0f;
		NewRotation.Roll = 0.0f;

		SetActorRotation(NewRotation);

		// Se o personagem chegou muito perto do ângulo final, encerra o giro físico
		if (CurrentRotation.Equals(TargetRotation, 10.0f))
		{
			bIsTurningInPlace = false;
			bIsHardTurningInPlace = false;
		}
		if (bIsHardTurningInPlace)
		{
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				int32 StateMachineIndex = AnimInstance->GetStateMachineIndex(FName("Default"));
				if (const FAnimNode_StateMachine* StateMachine = AnimInstance->GetStateMachineInstance(StateMachineIndex))
				{
					int32 StateIndex = YawOffset < 0 ? StateMachine->GetStateIndex(FName("Hard Turn In Place Left")) : StateMachine->GetStateIndex(FName("Hard Turn In Place Right"));
					float TimeRemaining = AnimInstance->GetRelevantAnimTimeRemainingFraction(StateMachineIndex,StateIndex);
					if (TimeRemaining <= 0.1f)		
						bIsHardTurningInPlace = false;
				}
			}
		}
	}
	
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
	{
		EIC->BindAction(IAMove, ETriggerEvent::Triggered, this, &APlayerClass::Move);
		EIC->BindAction(IAMove, ETriggerEvent::Completed, this, &APlayerClass::MoveCompleted);
	}
    
	if (UInputAction* IARun = RunAction.LoadSynchronous())
	{
		TWeakObjectPtr WeakThis(this);
		EIC->BindAction(IARun, ETriggerEvent::Started, this, &APlayerClass::RunStarted);
		EIC->BindAction(IARun, ETriggerEvent::Completed, this, &APlayerClass::RunCompleted);
	}
	
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
	
	if (InSkillInstance->bIsCasting || InSkillInstance->bIsCharging || InSkillInstance->bInCooldown)
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
	
	InSkillInstance->OnSkillReleasedDelegate.Broadcast(InSkillInstance->CurrentContext);
}

void APlayerClass::RunStarted(const FInputActionValue& Value)
{
	bIsRunning = true;
	
	FRotator NewRotationRate = DefaultRotationRate;
	NewRotationRate.Yaw -= 10.0f;
	this->GetCharacterMovement()->RotationRate = NewRotationRate;
}

void APlayerClass::RunCompleted(const FInputActionValue& Value)
{
	bIsRunning = false;
	this->GetCharacterMovement()->RotationRate = DefaultRotationRate;
}

void APlayerClass::Move(const FInputActionValue& Value)
{
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	// Valor do input (1D axis ou 2D axis)
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	// 1. Atualiza a velocidade física máxima permitida no CharacterMovementComponent
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? MaxRunMoveSpeed : MaxWalkMoveSpeed;
	}	
	
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
	
	// 1. Pega a velocidade no espaço do mundo
	FVector WorldVelocity = GetVelocity();

	// 2. Pega a rotação atual da cápsula do personagem
	FRotator ActorRotation = GetActorRotation();

	// 3. Converte a velocidade do mundo para o espaço local do personagem
	FVector LocalVelocity = ActorRotation.UnrotateVector(WorldVelocity);

	// 4. Agora você extrai os eixos perfeitamente para o seu Blend Space:
	ForwardMoveSpeed = LocalVelocity.X; // Positivo = Indo para frente | Negativo = Indo para trás
	RightMoveSpeed = LocalVelocity.Y;  // Positivo = Indo para a direita | Negativo = Indo para a esquerda
	
	// Atualiza o booleano baseando-se no input real, apenas quando ele muda!
	bIsMoving = !MovementVector.IsNearlyZero();
	bIsWalking = bIsRunning ? false : true;
}

void APlayerClass::MoveCompleted(const FInputActionValue& Value)
{
	bIsMoving = false;
	bIsWalking = false;
	bIsRunning = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
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
	
	YawOffset = (UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(),GetActorRotation())).Yaw;
	
	if (!bIsHardTurningInPlace)
	{
		bIsHardTurningInPlace = !FMath::IsNearlyEqual(YawOffset, 0.0f, 120.0f) && bIsMoving ? true : false;
		
		if (bIsHardTurningInPlace) bIsTurningInPlace = false;
		else bIsTurningInPlace = !FMath::IsNearlyEqual(YawOffset, 0.0f, 45.0f) && !bIsMoving ? true : false;
		
		if (bIsHardTurningInPlace || bIsTurningInPlace)
		{
			// 1. Calcula o destino de rotação somando o ângulo desejado (ex: +90 ou -90)
			FRotator CurrentRot = GetActorRotation();
			TargetRotation = FRotator(0.0f, CurrentRot.Yaw + YawOffset, 0.0f);
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
		return Instance && Instance->DataAsset->GetPrimaryAssetId() == SkillId;
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