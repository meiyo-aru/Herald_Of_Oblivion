#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "UObject/Object.h"
#include "Structs/SkillStructs.h"
#include "SkillFeature.generated.h"

/**
 * USkillFeature
 * Classe pai de todas as features de habilidade. É um UObject editável no editor e instanciável.
 * Uma feature são objetos de lógica e comportamentos essenciais para o funcionamento das habilidades
 * As features se comunicam entre si através de Delegates disparados pela instância de skill da qual pertencem.
 */

class UAnimSequenceBase;
class USoundCue;
class UNiagaraSystem;
// Enum para o tipo de aura
UENUM(BlueprintType)
enum class EAuraType : uint8
{
	StaticMesh		UMETA(DisplayName = "Static Mesh"),
	SkeletalMesh    UMETA(DisplayName = "Skeletal Mesh"),
	None			UMETA(DisplayName = "None")
};

// Enum para o tipo de superfície
UENUM(BlueprintType)
enum class ESurfaceType : uint8
{
	Floor       UMETA(DisplayName = "Floor"),
	Wall        UMETA(DisplayName = "Wall"),
	Ceiling     UMETA(DisplayName = "Ceiling"),
	Irregular   UMETA(DisplayName = "Irregular"),
	None        UMETA(DisplayName = "None")
};

class USkillDataAsset;
class APlayerClass;
class USkillInstance;
class UNiagaraComponent;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class HERALD_OF_OBLIVION_API USkillFeature : public UObject
{
	GENERATED_BODY()
	
public:
	
	// Um Struct que guarda todos os dados relevantes sobre o evento de input
	UPROPERTY()
	FSkillContext Context;
	
	// Inicializa a Feature, registrando-a nos delegates necessários
	virtual void Initialize(USkillInstance* Owner) {};

	// Aquece o niagara, spawnando ele invisivel para compilar os shaders
	void WarmupNiagara(UNiagaraSystem* Niagara);
	
	// Para efeitos de habilidades em que nao quero criar um actor, apenas o niagara é necessário e ele precisa estar anexado a algum socket
	UNiagaraComponent* SpawnAuraVFX(UNiagaraSystem* VFX, AEntityClass* InEntityOwner,
	                                FSkillContext InSkillContext, EAuraType AuraType, USceneComponent* Target, FName AttachSocketName = NAME_None);
	// Efeitos que precisa spawnar o skil actor
	ASkillActor* SpawnSkillActor(AEntityClass* InEntityOwner, FTransform SpawnTransform);
	UNiagaraComponent* SpawnVFXAtLocation(UNiagaraSystem* Niagara, FRotator Rotation, FVector Location, FSkillContext& InSkillContext);
	
	UFUNCTION()
	void OnNiagaraSystemFinished(UNiagaraComponent* NiagaraComponent);

	// Limpa o Niagara
	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>>& SpawnedNiagaraComponents);
	
	// Cria uma caixa de overlap
	TArray<FOverlapResult> MakeOverlapBox(FVector Size, FQuat Rotation, FSkillContext& InSkillContext, FVector Center);
	// Cria uma esfera de overlap
	virtual TArray<FOverlapResult> MakeOverlapSphere(float Radius, FSkillContext& InSkillContext, FVector Center);
	bool CanUseThisSkill(AEntityClass* EntityOwner);

	FTimerHandle CleanNiagaraTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "Properties")
	bool bMakeRotFromZ = true;
	
	// Define as propriedades de colisao da skill.
	UPROPERTY(EditAnywhere, Category="Collision Component")
	bool bHaveCollisionComponent = false;
	
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	float RadiusCollision = 0.0f;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	bool bGenerateOverlapEvents = true;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	bool bGenerateHitEvents = true;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	bool bNotifyRigidBodyCollision = true;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled = ECollisionEnabled::QueryAndPhysics;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	TEnumAsByte<ECollisionChannel> CollisionObjectType = ECC_GameTraceChannel1;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	TEnumAsByte<ECollisionResponse> PawnCollision = ECR_Overlap;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	TEnumAsByte<ECollisionResponse> WorldStaticCollision = ECR_Block;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	TEnumAsByte<ECollisionResponse> WorldDynamicCollision = ECR_Block;
	
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UNiagaraSystem>> CachedEffects;
	
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<USoundCue>> CachedSounds;
	
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UAnimSequenceBase>> CachedAnimations;
};
