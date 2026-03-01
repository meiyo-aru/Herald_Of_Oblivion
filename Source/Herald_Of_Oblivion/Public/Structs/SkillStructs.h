#pragma once

#include "Engine/OverlapResult.h"
#include "CoreMinimal.h" 
#include "Enumerators/EntityEnums.h"
#include "SkillStructs.generated.h"

class USkillInstance;
class AEntityClass;
class ASkillActor;

USTRUCT(BlueprintType)
struct FResourceCost
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float Value;
	UPROPERTY(EditAnywhere)
	EEntityTrueAttributeEnum Resource = EEntityTrueAttributeEnum::None;
};

UENUM(BlueprintType)
enum class EHitOverlapResultType : uint8
{
	None UMETA(DisplayName = "None"),
	Hit UMETA(DisplayName = "Hit"),
	Overlap UMETA(DisplayName = "Overlap")
};


UENUM(BlueprintType)
enum class ESkillStage : uint8
{
	None UMETA(DisplayName = "None"),
	Casting UMETA(DisplayName = "Casting"),
	InExecution UMETA(DisplayName = "InExecution"),
	Hitted UMETA(DisplayName = "Hitted"),
	Persistent UMETA(DisplayName = "Persistent")
};

USTRUCT(BlueprintType)
struct FSkillContext
{
	GENERATED_BODY()

public:
	FSkillContext();
	FSkillContext(USkillInstance* InSkillInstance, AEntityClass* InOwner);
	
	// Ponteiro para a instancia da habilidade
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<USkillInstance> SkillInstance;

	// Estágio da skill
	UPROPERTY(VisibleAnywhere)
	ESkillStage SkillStage;
	
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<ASkillActor> SkillActor;

	// Ponteiro fraco para a entidade que castou a habilidade
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AEntityClass> EntityOwner;
	
	// Um Vector normalizado que representa a direcao da skill
	UPROPERTY(EditAnywhere)
	FVector Direction = FVector::ZeroVector;
	
	// Alvo da habilidade
	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<AEntityClass> TargetEntity;
	

	UPROPERTY(VisibleAnywhere)
	EHitOverlapResultType HitOverlapResultType = EHitOverlapResultType::None;

	UPROPERTY(VisibleAnywhere)
	FHitResult HitResult;

	UPROPERTY(VisibleAnywhere)
	FOverlapResult OverlapResult;
	
	// Controla se a skill foi ativada ou não, ativar significa que a skill foi spawnada no mundo e lançada
	UPROPERTY(EditAnywhere)
	bool bActivated = false;
	
	// O normal da superfície, serve para rotacionar o vfx para correponder a superficie
	UPROPERTY()
	FVector_NetQuantizeNormal StartSurfaceNormal;
	
	UPROPERTY()
	FVector_NetQuantizeNormal EndSurfaceNormal;

	// O inicio do trajeto que a skill percorrerá
	UPROPERTY(EditAnywhere)
	FVector StartLocation = FVector::ZeroVector;
	
	// O fim do trajeto que a skill percorrerá ou a localizacao do spawn, caso não seja um projétil
	UPROPERTY(EditAnywhere)
	FVector EndLocation = FVector::ZeroVector;
	
	// Tempo ao apertar o botão
	UPROPERTY(VisibleAnywhere)
	float CastTime = 0.0f;
	
	// Tempo ao soltar o botão
	UPROPERTY(VisibleAnywhere)
	float ReleasedTime = 0.0f;
	
	// Tempo total que o botão ficou pressionado
	UPROPERTY(VisibleAnywhere)
	float HoldDuration = 0.0f;

	// geralmente variando de 0.0 a 1.0, representa a porcentagem de "preenchimento" 
	// de uma barra de carga ou o nível de intensidade de uma habilidade
	UPROPERTY(VisibleAnywhere)
	float ChargeRatio = 0.0f;
	
	void Reset();
};
