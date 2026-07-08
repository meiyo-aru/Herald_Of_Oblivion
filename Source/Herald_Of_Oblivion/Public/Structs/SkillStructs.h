#pragma once

#include "CoreMinimal.h" 
#include "Engine/OverlapResult.h"
#include "Enumerators/EntityEnums.h"
#include "SkillStructs.generated.h"

class USkillInstance;
class AEntityClass;
class ASkillActor;
class UNiagaraComponent;

// Custo de recurso necessario para usar uma habilidade.
USTRUCT(BlueprintType)
struct FResourceCost
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float Value = 0.0f;
	UPROPERTY(EditAnywhere)
	EEntityTrueAttributeEnum Resource = EEntityTrueAttributeEnum::None;
};

// Informa se a colisao da skill veio de hit, overlap ou nenhum dos dois.
UENUM(BlueprintType)
enum class EHitOverlapResultType : uint8
{
	None UMETA(DisplayName = "None"),
	Hit UMETA(DisplayName = "Hit"),
	Overlap UMETA(DisplayName = "Overlap")
};


// Estagio atual do fluxo da habilidade.
UENUM(BlueprintType)
enum class ESkillStage : uint8
{
	None UMETA(DisplayName = "None"),
	Casting UMETA(DisplayName = "Casting"),
	InExecution UMETA(DisplayName = "InExecution"),
	Hitting UMETA(DisplayName = "Hitting"),
};

USTRUCT(BlueprintType)
struct FHitOverlapResult
{
	GENERATED_BODY()
public:
	// Controller da entidade que causou o dano, IA ou Player
	UPROPERTY()
	TWeakObjectPtr<AController> ControllerInstigator;

	// Actor que está causando o dano
	UPROPERTY()
	TWeakObjectPtr<AActor> DamageCauser;
	
	UPROPERTY()
	FHitResult HitResult;
	
	UPROPERTY()
	TArray<FOverlapResult> OverlapResults;
	
	FHitOverlapResult() {};
	explicit FHitOverlapResult(const FHitResult& InHitResult) : HitResult(InHitResult), OverlapResults(TArray<FOverlapResult>()) {};
	explicit FHitOverlapResult(const TArray<FOverlapResult>& InOverlapResults) : HitResult(FHitResult()), OverlapResults(InOverlapResults) {};
};

// Contexto temporario que acompanha uma habilidade durante cast, execucao e hit.
USTRUCT(BlueprintType)
struct FSkillContext
{
	GENERATED_BODY()

public:
	FSkillContext();
	FSkillContext(USkillInstance* InSkillInstance, AEntityClass* InOwner);
	
	// Ponteiro para a instancia da habilidade
	UPROPERTY()
	TWeakObjectPtr<USkillInstance> SkillInstance;

	// Estágio da skill
	UPROPERTY()
	ESkillStage SkillStage = ESkillStage::None;
	
	// Actor da habilidade quando a execucao precisa existir no mundo.
	UPROPERTY()
	TWeakObjectPtr<ASkillActor> SkillActor;
	
	// Componentes Niagara criados durante a habilidade para limpeza posterior.
	UPROPERTY()
	TArray<TWeakObjectPtr<UNiagaraComponent>> SpawnedNiagaraComponents;
	
	// Ponteiro fraco para a entidade que castou a habilidade
	UPROPERTY()
	TWeakObjectPtr<AActor> EntityOwner;
	
	// Classe da entidade dona
	UPROPERTY()
	TSubclassOf<AActor> EntityOwnerClass;
	
	// Um Vector normalizado que representa a direcao da skill
	UPROPERTY()
	FVector Direction = FVector::ZeroVector;
	
	// Entidade que esta na localizacao inicial
	UPROPERTY()
	TWeakObjectPtr<AEntityClass> EntityOnStartLocation;
	
	// Entidade na localizacao final
	UPROPERTY()
	TWeakObjectPtr<AEntityClass> EntityOnEndLocation;

	UPROPERTY()
	FHitOverlapResult HitOverlapResult;
	
	/*// Resultado de hit/overlap usado pelas features OnHit.
	UPROPERTY(VisibleAnywhere)
	EHitOverlapResultType HitOverlapResultType = EHitOverlapResultType::None;

	// Dados completos quando a habilidade bate via hit fisico.
	UPROPERTY(VisibleAnywhere)
	FHitResult HitResult;

	// Dados completos quando a habilidade atinge via overlap.
	UPROPERTY(VisibleAnywhere)
	TArray<FOverlapResult> OverlapResults;*/
	
	// Controla se a skill foi ativada ou não, ativar significa que a skill foi spawnada no mundo e lançada
	UPROPERTY()
	bool bActivated = false;
	
	// O normal da superfície, serve para rotacionar o vfx para correponder a superficie
	UPROPERTY()
	FVector_NetQuantizeNormal StartSurfaceNormal;
	
	UPROPERTY()
	FVector_NetQuantizeNormal EndSurfaceNormal;

	// O inicio do trajeto que a skill percorrerá
	UPROPERTY()
	FVector StartLocation = FVector::ZeroVector;
	
	// O fim do trajeto que a skill percorrerá ou a localizacao do spawn, caso não seja um projétil
	UPROPERTY()
	FVector EndLocation = FVector::ZeroVector;
	
	// Tempo ao apertar o botão
	UPROPERTY()
	float CastTime = 0.0f;
	
	// Tempo ao soltar o botão
	UPROPERTY()
	float ReleasedTime = 0.0f;
	
	// Tempo total que o botão ficou pressionado
	UPROPERTY()
	float HoldDuration = 0.0f;

	// geralmente variando de 0.0 a 1.0, representa a porcentagem de "preenchimento" 
	// de uma barra de carga ou o nível de intensidade de uma habilidade
	UPROPERTY()
	float ChargeRatio = 0.0f;
	
	// Limpa dados temporarios e desativa VFX antigos antes de um novo uso da skill.
	void Reset();
};
