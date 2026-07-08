// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/AssetManagerUtility.h"

#include "Character/PlayerClass.h"
#include "Engine/AssetManager.h"

//
// TArray<FPrimaryAssetId> UAssetManagerUtility::GetSkillsBySpecialization(FName TargetSpecialization)
// {
// 	// Obtém a instância global do Asset Manager. 
// 	UAssetManager& AssetManager = UAssetManager::Get();
// 	TArray<FAssetData> AssetDataList;
// 	TArray<FPrimaryAssetId> SkillsBySpecialization;
//
// 	AssetManager.GetPrimaryAssetDataList(FPrimaryAssetType("Skill"), AssetDataList);
//
// 	for (const FAssetData& Data : AssetDataList)
// 	{
// 		FName TagValue;
// 		if (Data.GetTagValue("Specialization", TagValue))
// 		{
// 			if (TagValue == TargetSpecialization)
// 			{
// 				SkillsBySpecialization.Add(Data.GetPrimaryAssetId());
// 			}
//
// 		}
// 	}
// 	
// 	return SkillsBySpecialization;
// }

TArray<FPrimaryAssetId> UAssetManagerUtility::GetInitialSkillsBySpecialization(FName TargetSpecialization)
{
	// Obtém a instância global do Asset Manager. 
	UAssetManager& AssetManager = UAssetManager::Get();
	TArray<FAssetData> AssetDataList;
	TArray<FPrimaryAssetId> SkillsBySpecialization;

	AssetManager.GetPrimaryAssetDataList(FPrimaryAssetType("Skill"), AssetDataList);
	
	for (const FAssetData& Data : AssetDataList)
	{
		FName SpecializationValue;
		bool bIsInitialValue = false;
       
		if (Data.GetTagValue("SpecializationName", SpecializationValue) && 
			Data.GetTagValue("bIsInitial", bIsInitialValue))
		{
			if (SpecializationValue == TargetSpecialization && bIsInitialValue)
			{
				SkillsBySpecialization.Add(Data.GetPrimaryAssetId());
			}
		}
	}
	
	return SkillsBySpecialization;
}
//
// TArray<FPrimaryAssetId> UAssetManagerUtility::GetPlayerSkills(APlayerClass Player)
// {
// 	// Obtém a instância global do Asset Manager. 
// 	UAssetManager& AssetManager = UAssetManager::Get();
// 	TArray<FAssetData> AssetDataList;
// 	TArray<FPrimaryAssetId> PlayerSkills;
//
// 	AssetManager.GetPrimaryAssetDataList(FPrimaryAssetType("Skill"), AssetDataList);
// 	
// 	for (const FAssetData& Data : AssetDataList)
// 	{
// 		FName SpecializationValue;
// 		bool bIsInitialValue = false;
//        
// 		
// 		
// 		if (Data.GetTagValue("SpecializationName", SpecializationValue) && 
// 			Data.GetTagValue("bIsInitial", bIsInitialValue))
// 		{
// 			// 2. Não precisamos mais do .ToBool(), usamos a variável diretamente
// 			if (SpecializationValue == TargetSpecialization && bIsInitialValue)
// 			{
// 				PlayerSkills.Add(Data.GetPrimaryAssetId());
// 			}
// 		}
// 	}
// 	
// 	return PlayerSkills;
// }

TArray<FPrimaryAssetId> UAssetManagerUtility::GetPrimaryAssetIdList(FName AssetType)
{
	UAssetManager& AssetManager = UAssetManager::Get();
	TArray<FPrimaryAssetId> IdList;
	FPrimaryAssetType SkillType = FPrimaryAssetType(AssetType); 

	AssetManager.GetPrimaryAssetIdList(SkillType, IdList);

	return IdList;
}
