// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EquipmentInstance.h"

#include "Core/GameInstanceClass.h"
#include "Data/EquipmentDataAsset.h"

UEquipmentInstance::UEquipmentInstance() {}


void UEquipmentInstance::Initialize(FItemRarityStruct InRarity, int8 InLevel, AEntityClass* InOwner, FPrimaryAssetId InDataAsset, TArray<TObjectPtr<UOnHitFeature>> InOnHitFeatures)
{
	EntityOwner = InOwner;
	AssetId = InDataAsset;
	Rarity = InRarity;
	Level = InLevel;
	OnHitFeature = InOnHitFeatures;
}

AEquipmentActor* UEquipmentInstance::GetEquipmentActor(FVector Location = FVector::ZeroVector,
                                                                   FRotator Rotation = FRotator::ZeroRotator)
{
	UGameInstanceClass* GI = Cast<UGameInstanceClass>(GetWorld()->GetGameInstance());
	if (AEquipmentActor* Actor = Cast<AEquipmentActor>(GI->GetActorFromPool(AEquipmentActor::StaticClass())))
	{
		Actor->SetActorLocation(Location);
		Actor->SetActorRotation(Rotation);
		EquipmentActor = Actor;
		return Actor;
	}
	return nullptr;
}

AEquipmentActor* UEquipmentInstance::GetEquipmentActorAndAttach(FName SocketToAttach)
{	
	UGameInstanceClass* GI = Cast<UGameInstanceClass>(GetWorld()->GetGameInstance());
	if (AEquipmentActor* Actor = Cast<AEquipmentActor>(GI->GetActorFromPool(AEquipmentActor::StaticClass())))
	{
		if (AEntityClass* Owner = EntityOwner.Get())
		{
			// 4. Define as regras de transformação (O segredo da posição correta)
			FAttachmentTransformRules AttachmentRules(
				EAttachmentRule::SnapToTarget, // Posição: cola direto no socket
				EAttachmentRule::SnapToTarget, // Rotação: alinha com o socket
				EAttachmentRule::KeepWorld,    // Escala: mantém o tamanho original da arma
				true                           // Weld Simulated Bodies (junta a física se houver)
			);
			
			Actor->AttachToComponent(Owner->GetMesh(), AttachmentRules, SocketToAttach);
		}

		EquipmentActor = Actor;
		return Actor;
	}
	return nullptr;
}