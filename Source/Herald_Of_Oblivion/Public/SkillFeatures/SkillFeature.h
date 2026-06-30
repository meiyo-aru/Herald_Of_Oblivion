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
	UNiagaraComponent* SpawnAuraVFX(UNiagaraSystem* VFX, AEntityClass* InEntityOwner,
	                                FSkillContext InSkillContext, EAuraType AuraType, USceneComponent* Target, FName AttachSocketName = NAME_None);
	// Efeitos que precisa spawnar o skil actor
	ASkillActor* SpawnSkillActor(AEntityClass* InEntityOwner, FTransform SpawnTransform);
	UNiagaraComponent* SpawnVFXAtLocation(UNiagaraSystem* VFX, FRotator Rotation, FVector Location, FSkillContext& InSkillContext);

	virtual void CleanNiagara(TArray<TWeakObjectPtr<UNiagaraComponent>> SpawnedNiagaraComponents);
	virtual TArray<FOverlapResult> MakeHitSphere(float Radius, FSkillContext& InSkillContext, FVector Location);
	virtual void OnHitEntity(FSkillContext& InSkillContext);

	FTimerHandle CleanNiagaraTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "Properties")
	bool bMakeRotFromZ = true;
	
	// Define as propriedades de colisao da skill.
	UPROPERTY(EditAnywhere, Category="Collision Component")
	bool bHaveCollisionComponent = false;
	
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	float RadiusCollision = 0.0f;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	bool bGenerateOverlapEvents = false;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	bool bGenerateHitEvents = false;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	bool bNotifyRigidBodyCollision = false;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled = ECollisionEnabled::NoCollision;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	TEnumAsByte<ECollisionResponse> PawnCollision = ECR_Ignore;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	TEnumAsByte<ECollisionResponse> WorldStaticCollision = ECR_Ignore;
	UPROPERTY(EditAnywhere, Category="Collision Component", meta=(EditCondition="bHaveCollisionComponent", EditConditionHides))
	TEnumAsByte<ECollisionResponse> WorldDynamicCollision = ECR_Ignore;
};
