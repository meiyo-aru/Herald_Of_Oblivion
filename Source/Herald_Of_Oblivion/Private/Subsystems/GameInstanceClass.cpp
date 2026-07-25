// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/GameInstanceClass.h"

#include "Character/PlayerClass.h"
#include "Core/EquipmentInstance.h"
#include "Core/SkillInstance.h"
#include "Data/EquipmentDataAsset.h"
#include "Data/SkillDataAsset.h"
#include "Data/SpecializationDataAsset.h"

void UGameInstanceClass::Init()
{
	Super::Init();
}

void UGameInstanceClass::InitializeNewPlayer(APlayerClass& Player)
{
	if (Player.Specialization)
	{
		if (!Player.Specialization->InitialSkills.IsEmpty())
			for (TSoftObjectPtr<USkillDataAsset> SoftSkillDataAsset : Player.Specialization->InitialSkills)
			{
				if (USkillDataAsset* StrongSkillDataAsset = SoftSkillDataAsset.LoadSynchronous())
				{
					USkillInstance* SkillInstance = StrongSkillDataAsset->CreateInstance(&Player);
					Player.EquipSkill(SkillInstance, false);
				}
			}
		if (!Player.Specialization->InitialEquipments.IsEmpty())
			for (TSoftObjectPtr<UEquipmentDataAsset> SoftEquipmentDataAsset : Player.Specialization->InitialEquipments)
			{
				if (UEquipmentDataAsset* StrongEquipmentDataAsset = SoftEquipmentDataAsset.LoadSynchronous())
				{
					UEquipmentInstance* EquipmentInstance = StrongEquipmentDataAsset->GetInstance(FItemRarityStruct(EItemRarityEnum::Normal), 1, 1, &Player);
					EquipmentInstance->GetEquipmentActorFromPoolAndAttach(StrongEquipmentDataAsset->StaticMesh.LoadSynchronous());
					Player.EquipEquipment(EquipmentInstance);
				}
			}		
		/*for (UItemDataAsset* ItemDataAsset : Specialization->InitialItems)
		{
			
		}*/
		
	}
}
