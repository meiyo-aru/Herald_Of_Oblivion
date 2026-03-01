// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerClass.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "Data/SurfacePhysMaterialClass.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	this->SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	// Define o Zoom base como a distanci inicial do SpringArm
	this->DesiredZoom = SpringArm->TargetArmLength;
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
		FName("CastVFX"), FName("CastSFX"),
		FName("PathVFX"), FName("PathSFX")
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

	if (Data->bAuraInCast)
	{
		if (!SkillInstance->AuraHandle || !SkillInstance->AuraHandle.IsValid())
		{
			if (Data->bAuraInSkeletalMesh)
				AuraBundles.Add(FName("SkeletalMeshAuraVFX"));
			if (Data->bAuraInStaticMesh)
				AuraBundles.Add(FName("StaticMeshAuraVFX"));
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
	
	if (this->SpringArm)
	{
		this->SpringArm->TargetArmLength = FMath::FInterpTo(this->SpringArm->TargetArmLength, this->DesiredZoom, DeltaTime, this->ZoomInterpSpeed);		
	}
}

// Called to bind functionality to input
void APlayerClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Movimenta o personagem utilizando o Navmesh
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &APlayerClass::MoveToMouseCursor);
	
	PlayerInputComponent->BindAction("FirstSkill", IE_Pressed, this, &APlayerClass::CastFirstSkill);
	PlayerInputComponent->BindAction("FirstSkill", IE_Released, this, &APlayerClass::ReleasedFirstSkill);
	
	PlayerInputComponent->BindAction("SecondSkill", IE_Pressed, this, &APlayerClass::CastSecondSkill);
	PlayerInputComponent->BindAction("SecondSkill", IE_Released, this, &APlayerClass::ReleasedSecondSkill);
	
	PlayerInputComponent->BindAction("ThirdSkill", IE_Pressed, this, &APlayerClass::CastThirdSkill);
	PlayerInputComponent->BindAction("ThirdSkill", IE_Released, this, &APlayerClass::ReleasedThirdSkill);
	
	PlayerInputComponent->BindAction("FourthSkill", IE_Pressed, this, &APlayerClass::CastFourthSkill);
	PlayerInputComponent->BindAction("FourthSkill", IE_Released, this, &APlayerClass::ReleasedFourthSkill);
		
	// PlayerInputComponent->BindAction("FirstQuickAccess", IE_Pressed, this, &APlayerClass::CastFirstQuickAccess);
	// PlayerInputComponent->BindAction("SecondQuickAccess", IE_Pressed, this, &APlayerClass::CastSecondQuickAccess);
	// PlayerInputComponent->BindAction("ThirdQuickAccess", IE_Pressed, this, &APlayerClass::CastThirdQuickAccess);
	
	
	// Aplica um zoom in ou zoom out
	PlayerInputComponent->BindAxis("HandleZoom", this, &APlayerClass::HandleZoom);
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


void APlayerClass::HandleZoom(float Delta)
{
	if (this->SpringArm)
	{
		this->DesiredZoom = FMath::Clamp(DesiredZoom + (Delta * 100.0f), this->MinZoom, this->MaxZoom);
	}
}

// Define os atributos da entidade
void APlayerClass::DefineAttributes()
{
	Super::DefineAttributes();
}

void APlayerClass::MoveToMouseCursor()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerClass::MoveToMouseCursor - PlayerController invalido."));
		return;
	}

	FHitResult Hit;
	// Captura a posição do mouse no mundo
	if (PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
	{
		DrawDebugSphere(GetWorld(), Hit.Location, 20.0f, 10, FColor::Red, false, 0.5f, 0, 0.1f);
		
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		FNavLocation ProjectedLocation;
		
		// Projeta o clique no NavMesh para garantir que o player não tente andar no "vazio"
		// Raio de busca de 100 unidades
		if (NavSys && NavSys->ProjectPointToNavigation(Hit.Location, ProjectedLocation, FVector(100.f, 100.f, 100.f)))
		{
			// Ordena o movimento usando a biblioteca de IA (funciona para o Player também)
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(PC, ProjectedLocation.Location);
		}
	}
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
