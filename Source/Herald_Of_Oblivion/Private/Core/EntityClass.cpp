// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EntityClass.h"

#include "Components/CapsuleComponent.h"
#include "Core/SpecializationDataAsset.h"
#include "Data/SkillDataAsset.h"
#include "Core/SkillInstance.h"
#include "Engine/AssetManager.h"
#include "Utility/AssetManagerUtility.h"


// Sets default values
AEntityClass::AEntityClass()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; 
	
	this->RightEquippedWeapon = CreateDefaultSubobject<UStaticMeshComponent>(FName("Right Equipped Weapon"));
	this->RightEquippedWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->RightEquippedWeapon->SetCastShadow(true);
	
	this->LeftEquippedWeapon = CreateDefaultSubobject<UStaticMeshComponent>(FName("Left Equipped Weapon"));
	this->LeftEquippedWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->LeftEquippedWeapon->SetCastShadow(true);
	
	this->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Block);
}

// Called when the game starts or when spawned
void AEntityClass::BeginPlay()
{
	Super::BeginPlay();
	DefineAttributes();
	DefineSkills();
	
	if (GetMesh() && this->RightEquippedWeapon)
	{
		this->RightEquippedWeapon->AttachToComponent(
			GetMesh(), 
			FAttachmentTransformRules::SnapToTargetIncludingScale, 
			FName("RightHandSocket") 
		);
	}
	
	if (GetMesh() && this->LeftEquippedWeapon)
	{
		this->LeftEquippedWeapon->AttachToComponent(
			GetMesh(), 
			FAttachmentTransformRules::SnapToTargetIncludingScale, 
			FName("LeftHandSocket") 
		);
	}
}

// Called every frame
void AEntityClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Função para que sempre que a raridade for alterada no editor o valor padrao do modificador seja alterado também
#if WITH_EDITOR
void AEntityClass::PostEditChangeChainProperty(FPropertyChangedChainEvent& e)
{
	Super::PostEditChangeChainProperty(e);

	// Verifica se os ponteiros do evento são válidos. Se não, sai imediatamente.
	if (e.Property == nullptr || e.MemberProperty == nullptr)
	{
		return;
	}

	// Obtem os nomes com segurança.
	const FName PropertyName = e.PropertyChain.GetActiveNode()->GetValue()->GetFName();
	const FName MemberPropertyName = e.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName();

	// Verificação para FEntityRarityStruct
	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(AEntityClass, Rarity) &&
		PropertyName == GET_MEMBER_NAME_CHECKED(FEntityRarityStruct, Rarity))
	{
		Rarity.UpdateModifierToDefault();
		return; // Retorne para evitar verificações desnecessárias
	}
	
	// Verificação para o TArray FSlaughterAchievements
	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(AEntityClass, SlaughterAchievements) &&
		PropertyName == GET_MEMBER_NAME_CHECKED(FEntitySlaughterAchievementsStruct, Achievements)) 
	{
		const int32 ChangedIndex = e.GetArrayIndex(e.PropertyChain.GetActiveMemberNode()->GetValue()->GetName());
		if (SlaughterAchievements.IsValidIndex(ChangedIndex))
		{
			SlaughterAchievements[ChangedIndex].UpdateModifierToDefault();
		}
	}
	
	// Verificação para USpecializationDataAsset
	if (PropertyName == GET_MEMBER_NAME_CHECKED(USkillDataAsset, Specialization))
	{
		if (this->Specialization)
		{
			this->SpecializationName = this->Specialization->GetFName();
		}
		else
		{
			// Se o usuário limpar o campo, limpamos o FName
			SpecializationName = NAME_None;
		}
	}
}
#endif

// Define as skills iniciais da entidade
void AEntityClass::DefineSkills()
{
	// Se a entidade é nível 1, significa que sempre vai ter o mesmo conjunto de habilidades, dependendo da especializacao.
	// se nao for nivel 1 entao o método é sobrescrito pela subclasse e tem lógica exclusiva 
	if (this->Level == 1)
	{
		// Pega os AssetsId baseado na Especializacao, raridade e nível do jogador
		TArray<FPrimaryAssetId> AssetsId = UAssetManagerUtility::GetInitialSkillsBySpecialization(this->SpecializationName);

		UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
		if (!AssetManager)
		{
			UE_LOG(LogTemp, Error, TEXT("AEntityClass::DefineSkills - AssetManager invalido."));
			return;
		}
		
		// Criamos o delegate. 
		// O parâmetro extra (SkillIds) é passado para que a função de callback saiba o que foi carregado.
		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &AEntityClass::OnAllSkillsLoaded, AssetsId);

		// Inicia o carregamento em massa
		AssetManager->LoadPrimaryAssets(AssetsId, TArray<FName>(), Delegate);
	}
}



void AEntityClass::OnAllSkillsLoaded(TArray<FPrimaryAssetId> LoadedIds)
{
	UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
	if (!AssetManager)
	{
		UE_LOG(LogTemp, Error, TEXT("AEntityClass::OnAllSkillsLoaded - AssetManager invalido."));
		return;
	}
	
	for (const FPrimaryAssetId& Id : LoadedIds)
	{
		// GetPrimaryAssetObject busca exatamente esse ID específico
		if (USkillDataAsset* SkillData = Cast<USkillDataAsset>(AssetManager->GetPrimaryAssetObject(Id)))
		{
			this->SkillsInstances.Add(SkillData->CreateInstance(this));
		}
	}
}

// Define os atributos padrão para todas as criaturas, é sobrescrito pelo método da classe filha
void AEntityClass::DefineAttributes()
{
	// Definicao dos atributos simbolicos e dos atributos verdadeiros afetados por eles
	TArray<FAffectedAttributeStruct> AttributesAffectedByStrength = {
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::SlashingDamage, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::PiercingDamage, 2.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::BludgeoningDamage, 4.0f)
	};
	this->SimbolicAttributes.Emplace(EEntitySimbolicAttributeEnum::Strength, FAttribute(10.0f, EAttributeTypeEnum::SimbolicAttribute, EEntitySimbolicAttributeEnum::Strength, AttributesAffectedByStrength));
	
	TArray<FAffectedAttributeStruct> AttributesAffectedByDexterity = {
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::SlashingDamage, 2.5f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::PiercingDamage, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::BludgeoningDamage, 1.5f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::CriticalChance, 0.5f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::CriticalDamage, 2.5f)
	};
	this->SimbolicAttributes.Emplace(EEntitySimbolicAttributeEnum::Dexterity, FAttribute(10.0f, EAttributeTypeEnum::SimbolicAttribute, EEntitySimbolicAttributeEnum::Dexterity, AttributesAffectedByDexterity));
	
	TArray<FAffectedAttributeStruct> AttributesAffectedByConstitution = {
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::MaxEnergy, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::FatigueReduction, 1.5f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::Tenacity, 0.5f)
	};
	this->SimbolicAttributes.Emplace(EEntitySimbolicAttributeEnum::Constitution, FAttribute(10.0f, EAttributeTypeEnum::SimbolicAttribute, EEntitySimbolicAttributeEnum::Constitution, AttributesAffectedByConstitution));
	
	TArray<FAffectedAttributeStruct> AttributesAffectedByVitality = {
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::MaxHealth, 5.0f),
	};
	this->SimbolicAttributes.Emplace(EEntitySimbolicAttributeEnum::Vitality, FAttribute(10.0f, EAttributeTypeEnum::SimbolicAttribute, EEntitySimbolicAttributeEnum::Vitality, AttributesAffectedByVitality));
		
	TArray<FAffectedAttributeStruct> AttributesAffectedByIntelligence = {
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::MaxMana, 2.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::FireDamage, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::IceDamage, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::LightningDamage, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::EarthDamage, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::PoisonDamage, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::AcidDamage, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::HolyDamage, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::ProfaneDamage, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::LightDamage, 3.0f),
		FAffectedAttributeStruct(EEntityTrueAttributeEnum::ShadowDamage, 3.0f)
	};
	this->SimbolicAttributes.Emplace(EEntitySimbolicAttributeEnum::Intelligence, FAttribute(10.0f, EAttributeTypeEnum::SimbolicAttribute, EEntitySimbolicAttributeEnum::Intelligence, AttributesAffectedByIntelligence));
	
	// Danos
	// Dano cortante
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::SlashingDamage, FAttribute(0.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::SlashingDamage));
	// Dano perfurante
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::PiercingDamage, FAttribute(0.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::PiercingDamage));
	// Dano de concussão
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::BludgeoningDamage, FAttribute(0.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::BludgeoningDamage));
	// Chance de critico
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::CriticalChance, FAttribute(0.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::CriticalChance));
	// Dano de critico
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::CriticalDamage, FAttribute(150.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::CriticalDamage));
	// Roubo de vida
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::LifeSteal, FAttribute(0.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::LifeSteal));
	// Danos mágicos
	for (int i = 3; i < 13; ++i)
	{
		this->TrueAttributes.Emplace(static_cast<EEntityTrueAttributeEnum>(i), FAttribute(0.0f, EAttributeTypeEnum::TrueAttribute, static_cast<EEntityTrueAttributeEnum>(i)));
	}
	// Resistencias
	for (int i = 16; i < 29; ++i)
	{
		this->TrueAttributes.Emplace(static_cast<EEntityTrueAttributeEnum>(i), FAttribute(0.0f, EAttributeTypeEnum::TrueAttribute, static_cast<EEntityTrueAttributeEnum>(i)));
	}
	// Tenacidade
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::Tenacity, FAttribute(0.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::Tenacity));

	//Vital Stats
	//Vida maxima
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::MaxHealth, FAttribute(150.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::MaxHealth));
	//Vida 
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::Health, FAttribute(150.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::Health));
	//Mana maxima 
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::MaxMana, FAttribute(200.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::MaxMana));
	//Mana  
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::Mana, FAttribute(200.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::Mana));
	//Energia maxima  
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::MaxEnergy, FAttribute(200.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::MaxEnergy));
	//Energia   
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::Energy, FAttribute(200.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::Energy));
	//Redução de fadiga
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::FatigueReduction, FAttribute(0.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::FatigueReduction));

	//Utility & Speed
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::MovementSpeed, FAttribute(300.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::MovementSpeed));
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::AttackSpeed, FAttribute(0.7f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::AttackSpeed));
	this->TrueAttributes.Emplace(EEntityTrueAttributeEnum::AbilitySpeed, FAttribute(1.0f, EAttributeTypeEnum::TrueAttribute, EEntityTrueAttributeEnum::AbilitySpeed));
}

// Calcula o Retorno de XP ao abater a entidade
float AEntityClass::CalculateXPReturn(AEntityClass* Killer)
{
	// Diferença de nivel entre o assassino e a entidade morta
	float LevelDifference = this->Level  - Killer->Level;
	
	float LevelDifferenceModifier = 1.0f;
	
	if (LevelDifference > 20) LevelDifferenceModifier = 5.0f;       // Inimigo perigoso
	else if (LevelDifference > 10) LevelDifferenceModifier = 1.5f;       // Inimigo perigoso
	else if (LevelDifference > 5) LevelDifferenceModifier = 1.2f;  // Inimigo um pouco mais forte
	else if (LevelDifference < -15) LevelDifferenceModifier = 0.0f; // Inimigo trivial
	else if (LevelDifference < -10) LevelDifferenceModifier = 0.25f; // Inimigo muito fraco
	else if (LevelDifference < -5) LevelDifferenceModifier = 0.6f;  // Inimigo um pouco mais fraco

	float XPReturn = this->XP * LevelDifferenceModifier * this->Rarity.RarityModifier;
	
	// Verifica se a entidade possui conquistas de abate a serem reivindicadas e se o assassino alcançou alguma
	if (this->SlaughterAchievements.Num() && this->SlaughterAchievementsReached.Num())
	{
		for (EEntitySlaughterAchievementsEnum AchievementReached : SlaughterAchievementsReached)
		{
			FEntitySlaughterAchievementsStruct* AchievementReachedStruct = SlaughterAchievements.FindByPredicate([AchievementReached](const FEntitySlaughterAchievementsStruct& Achievement)
			{
				return Achievement.Achievements == AchievementReached;
			});
			
			if (AchievementReachedStruct)
			{
				XPReturn *= AchievementReachedStruct->AchievementsModifier;
			}
		}
	}
	
	return XPReturn;
};

float AEntityClass::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AEntityClass::TakeRestriction(ERestrictionTypeEnum Restriction)
{
	if (Restriction != ERestrictionTypeEnum::None)
	{
		this->Restrictions.Emplace(Restriction);
	}
}

void AEntityClass::Die()
{
}

void AEntityClass::InitializeSkills(TArray<USkillInstance*> SkillInstances)
{
	for (USkillInstance* Instance : SkillInstances)
	{
		Instance->InitializeFeatures();
	}
}

void AEntityClass::InitializeSkills(USkillInstance* SkillInstance)
{
	SkillInstance->InitializeFeatures();
}
