// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntityClass.h"
#include "Data/SkillDataAsset.h"
#include "Structs/SkillStructs.h"
#include "UObject/Object.h"
#include "SkillInstance.generated.h"

/**
 * USkillInstance
 * Classe que representa uma instância de uma habilidade. Uma entidade só pode ter uma instância da mesma habilidade.
 * As instâncias são únicas para cada entidade e não são compartilhadas.
 * Guarda informações exclusivas para a entidade a qual pertence, como o nível da habilidade.
 */

class UOnHitFeature;
class UActivationFeature;
class UExecutionFeature;
struct FStreamableHandle;
class AEntityClass;
class USkillFeature;
struct FSkillContext;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillCast, FSkillContext&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillReleased, FSkillContext&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillActivate, FSkillContext&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillHit, FSkillContext&);

UCLASS(Blueprintable, BlueprintType)
class HERALD_OF_OBLIVION_API USkillInstance : public UObject
{
	GENERATED_BODY()
	
public:
	USkillInstance();
	virtual void BeginDestroy() override;

	// Chamado ao lançar a habilidade, faz ela entrar em tempo de recarga de acordo com o CooldownReduce
	void CastSkill();
	void GoOnCooldown();
	
	// Reseta as propriedades para que possa ser guardada na Pool
	void PrepareForPooling();

	
	FOnSkillCast OnSkillCastDelegate;
	FOnSkillReleased OnSkillReleasedDelegate;
	FOnSkillActivate OnSkillActivateDelegate;
	FOnSkillHit OnSkillHitDelegate;
	
	// TArray<TSoftObjectPtr<UEffect>> AGetEffects() const {return Effects;};
private:
	// Ponteiro para o dono da instância da habilidade
	UPROPERTY(EditAnywhere, Category="Properties")
	TWeakObjectPtr<AEntityClass> Owner;
	
	FTimerHandle TimerHandle;
	
	// Ponteiro para o USkillDataAsset da habilidade
	UPROPERTY(EditAnywhere, Category="Properties", meta=(AllowedTypes="Skill"))
	FPrimaryAssetId AssetId;
	
	// Level da habilidade
	UPROPERTY(EditAnywhere, Category="Properties")
	uint8 Level = 1;
	
	// Cada unidade representa % de redução de tempo de recarga, possui um máximo de 90%
	UPROPERTY(EditAnywhere, Category="Properties")
	uint8 CooldownReduce = 0;
	
	// Um TArray contendo os efeitos causados por essa instância da habilidade, 
	// esses são efeitos à parte dos efeitos do DataAsset mas são aplicados da mesma forma. Deve usar Object Pooling
	// TArray<TSoftObjectPtr<UEffect>> Effects;
	
	// Determina se a skill está disponível para ser lançada
	UPROPERTY(EditAnywhere, Category="Properties")
	bool bInCooldown = false;
	
	// Determina se a skill está sendo lançada
	UPROPERTY(EditAnywhere, Category="Properties")
	bool bIsCasting = false;

	// Um multiplicador de força da habilidade, o padrão é 1.0, debuffs e buffs de habilidade podem alterar esse modificador
	UPROPERTY(EditAnywhere, Category="Properties")
	float ForceMultiplier = 1.0;
	
public:
	// Handles SEPARADOS para controle independente
	TSharedPtr<FStreamableHandle> EntityOwnerAuraHandle;      // Aura
	TSharedPtr<FStreamableHandle> WeaponAuraHandle;      // Aura
	TSharedPtr<FStreamableHandle> ActivationHandle;      // Activation e execution
	/*
	TSharedPtr<FStreamableHandle> OnHitHandle;   // OnHit
	*/
	
	// Features da skill
	UPROPERTY(EditAnywhere, Instanced, Category = "Features")
	UExecutionFeature* ExecutionFeature;
	UPROPERTY(EditAnywhere, Instanced, Category = "Features")
	UActivationFeature* ActivationFeature;
	UPROPERTY(EditAnywhere, Instanced, Category = "Features")
	TArray<UOnHitFeature*> OnHitFeature;
	
	UPROPERTY(VisibleAnywhere, Category="Properties")
	FSkillContext CurrentContext;

	// Método para inicializar a instancia de skill
	virtual void Initialize(AEntityClass* InOwner, FPrimaryAssetId InAssetId, UActivationFeature* InActivationFeature, UExecutionFeature*
	                        InExecutionFeature, TArray<UOnHitFeature*> InOnHitFeature);
	
	// Método para inicializar as features
	virtual void InitializeFeatures();
	
	// Retorna o DataAsset
	FPrimaryAssetId GetAssetId() const {return AssetId;}
	
	// Retorna a entidade dona da instancia de skill
	const AEntityClass* GetOwner() const
	{
		if (AEntityClass* EntOwner = Owner.Get())
			return EntOwner;
		else return nullptr;
	};
	
	const bool GetInCooldown() const {return this->bInCooldown;};
	const bool GetIsCasting() const {return this->bIsCasting;};
	const void FinishCast() {this->bIsCasting = false;};
	
	void ReleaseActivationHandle();
	void ReleaseAuraHandle();
	
	UActivationFeature* GetActivationFeature() const {return this->ActivationFeature;};
	UExecutionFeature* GetExecutionFeature() const {return this->ExecutionFeature;};
	TArray<UOnHitFeature*> GetOnHitFeature() const {return this->OnHitFeature;};
};
