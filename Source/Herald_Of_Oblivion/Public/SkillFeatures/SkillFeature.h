#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Structs/SkillStructs.h"
#include "SkillFeature.generated.h"

/**
 * USkillFeature
 * Classe pai de todas as features. É um UObject editável no inspetor e instanciável.
 * Uma feature são objetos de lógica e comportamentos essenciais para o funcionamento das habilidades
 * As features se comunicam entre si através de Delegates disparados pela instância de skill da qual pertencem.
 * Uma feature pode por exemplo disparar um Delegate específico da sua instância para que outras features
 * saibam que ela terminou seu trabalho e possam processar o resultado
 */

class UNiagaraSystem;
// Enum para o tipo de aura
UENUM(BlueprintType)
enum class EAuraType : uint8
{
	StaticMesh		UMETA(DisplayName = "Static Mesh"),
	SkeletalMesh    UMETA(DisplayName = "Skeletal Mesh"),
	None			UMETA(DisplayName = "None")
};

class USkillDataAsset;
class APlayerClass;
class USkillInstance;
class UNiagaraComponent;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class HERALD_OF_OBLIVION_API USkillFeature : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override {
		return FPrimaryAssetId("SkillFeatureFX", GetFName());
	}
	
	// Um Struct que guarda todos os dados relevantes sobre o evento de input
	UPROPERTY()
	FSkillContext Context;
	
	// // A entidade que castou a habilidade
	// UPROPERTY()
	// AEntityClass* EntityOwner;
	//
	// // O data asset da habilidade
	// UPROPERTY()
	// const USkillDataAsset* SkillDataAsset;
	//
	// // A instancia da habilidade
	// UPROPERTY()
	// USkillInstance* SkillInstance;
	
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) {};

	// Lógica de ativação (opcional para features do tipo activation)
	virtual void StartActivation(FSkillContext& InSkillContext) {};
	virtual void CompleteActivation(FSkillContext& InSkillContext) {};
	
	// Para efeitos de habilidades em que nao quero criar um actor, apenas o niagara é necessário e ele precisa estar anexado a algum socket
	UNiagaraComponent* SpawnAuraVFX(UNiagaraSystem* VFX, const USkillDataAsset* InSkillDataAsset, AEntityClass* InEntityOwner,
						FSkillContext InSkillContext, EAuraType AuraType, USceneComponent* Target, FName AttachSocketName = NAME_None);
	// Efeitos que precisa spawnar o skil actor
	ASkillActor* SpawnSkillActor(AEntityClass* InEntityOwner, FTransform SpawnTransform);
	UNiagaraComponent* SpawnVFXAtLocation(UNiagaraSystem* VFX, FRotator Rotation, FVector Location);
	// Para efeitos de habilidades em que nao quero criar um actor, apenas o niagara é necessário e ele não pode estar anexado a algum socket
	UNiagaraComponent* SpawnCastVfx(const USkillDataAsset* InSkillDataAsset, AEntityClass* InEntityOwner,
	                      FSkillContext InSkillContext, FVector TargetLocation);

	// Limpa os handles utilizados
	UFUNCTION()
	virtual void OnNiagaraSystemFinished(UNiagaraComponent* FinishedComponent);
	virtual TArray<FOverlapResult> MakeHitSphere(float Radius, FSkillContext& InSkillContext, FVector Location);
	virtual void OnHitEntity(FSkillContext& InSkillContext);
	UFUNCTION()
	virtual void OnAuraNiagaraSystemFinished(UNiagaraComponent* FinishedComponent);

};
