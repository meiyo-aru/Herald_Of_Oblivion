// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/GameInstanceClass.h"

#include "Character/PlayerClass.h"
#include "Core/EquipmentInstance.h"
#include "Data/EquipmentDataAsset.h"
#include "Data/SkillDataAsset.h"
#include "Data/SpecializationDataAsset.h"

void UGameInstanceClass::Init()
{
	Super::Init();
}

void UGameInstanceClass::InitializeNewPlayer(APlayerClass& Player)
{
	if (const TSoftObjectPtr<USpecializationDataAsset>* DataAssetPtr = InitialEquipmentsAndSkillsBySpecialization.Find(Player.GetSpecializationId()))
	{
		const TSoftObjectPtr<USpecializationDataAsset> DataAsset = *DataAssetPtr;
		
		if (USpecializationDataAsset* Specialization = DataAsset.LoadSynchronous())
		{
			if (!Specialization->InitialSkills.IsEmpty())
				for (USkillDataAsset* SkillDataAsset : Specialization->InitialSkills)
				{
					USkillInstance* SkillInstance = SkillDataAsset->CreateInstance(&Player);
					Player.EquipSkill(SkillInstance, false);
				}
			if (!Specialization->InitialEquipments.IsEmpty())
				for (UEquipmentDataAsset* EquipmentDataAsset : Specialization->InitialEquipments)
				{
					UEquipmentInstance* EquipmentInstance = EquipmentDataAsset->GetInstance(FItemRarityStruct(EItemRarityEnum::Normal), 1, 1, &Player);
					EquipmentInstance->EquipmentActor = EquipmentInstance->GetEquipmentActorAndAttach();
					EquipmentInstance->EquipmentActor->StaticMesh = EquipmentDataAsset->StaticMesh.LoadSynchronous();
					EquipmentInstance->EquipmentActor->StaticMeshComponent->SetStaticMesh(EquipmentInstance->EquipmentActor->StaticMesh);
					Player.EquipEquipment(EquipmentInstance);
				}		
			/*for (UItemDataAsset* ItemDataAsset : Specialization->InitialItems)
			{
				
			}*/
		}
	}
}
