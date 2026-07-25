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

	// Delegates essenciais para o funcionamento das features
	FOnSkillCast OnSkillCastDelegate;
	FOnSkillReleased OnSkillReleasedDelegate;
	FOnSkillActivate OnSkillActivateDelegate;
	FOnSkillHit OnSkillHitDelegate;
	
	// Ponteiro para o USkillDataAsset da habilidade
	UPROPERTY(Transient, VisibleAnywhere, Category="Properties", meta=(AllowedTypes="Skill"))
	TObjectPtr<USkillDataAsset> DataAsset;
	
	// TArray<TSoftObjectPtr<UEffect>> AGetEffects() const {return Effects;};
private:
	// Ponteiro para o dono da instância da habilidade
	UPROPERTY(Transient, VisibleAnywhere, Category="Properties")
	TWeakObjectPtr<AEntityClass> Owner;
	
	UPROPERTY(Transient)
	FTimerHandle TimerHandle;
	
	// Level da habilidade
	UPROPERTY(Transient, EditAnywhere, Category="Properties")
	uint8 Level;
	
	// Cada unidade representa % de redução de tempo de recarga, possui um máximo de 90%
	UPROPERTY(Transient, EditAnywhere, Category="Properties")
	uint8 CooldownReduce;
	
	// Um TArray contendo os efeitos causados por essa instância da habilidade, 
	// esses são efeitos à parte dos efeitos do DataAsset mas são aplicados da mesma forma. Deve usar Object Pooling
	// TArray<TSoftObjectPtr<UEffect>> Effects;

	// Um multiplicador de força da habilidade, o padrão é 1.0, debuffs e buffs de habilidade podem alterar esse modificador
	UPROPERTY(VisibleAnywhere, Category="Properties")
	float ForceMultiplier = 1.0;
	
public:
	// Handle dos FX das Skills, ele segura os efeitos na memória
	TSharedPtr<FStreamableHandle> SkillsHandle;
	
	// Features da skill duplicadas do DataAsset
	UPROPERTY(Transient, EditDefaultsOnly, Instanced, Category = "Features")
	TObjectPtr<UExecutionFeature> ExecutionFeature;
	UPROPERTY(Transient, EditDefaultsOnly, Instanced, Category = "Features")
	TObjectPtr<UActivationFeature> ActivationFeature;
	UPROPERTY(Transient, EditDefaultsOnly, Instanced, Category = "Features")
	TArray<TObjectPtr<UOnHitFeature>> OnHitFeature;
	
	UPROPERTY(Transient, VisibleAnywhere, Category="Properties")
	FSkillContext CurrentContext;

	// Método para inicializar a instancia de skill
	virtual void Initialize(AEntityClass* InOwner, USkillDataAsset* DataAsset, UActivationFeature* InActivationFeature, UExecutionFeature*
	                        InExecutionFeature, TArray<UOnHitFeature*> InOnHitFeature);
	
	// Método para inicializar as features
	virtual void Prepare();
	
	// Retorna a entidade dona da instancia de skill
	const AEntityClass* GetOwner() const
	{
		if (AEntityClass* EntOwner = Owner.Get())
			return EntOwner;
		else return nullptr;
	};
	
	// Determina se a skill está disponível para ser lançada
	UPROPERTY(Transient, EditAnywhere, Category="Properties")
	bool bInCooldown;
	
	// Determina se a skill está sendo lançada
	UPROPERTY(Transient, VisibleAnywhere, Category="Properties")
	bool bIsCasting;

	// Determina se a skill está sendo carregada
	UPROPERTY(Transient, VisibleAnywhere, Category="Properties")
	bool bIsCharging;
	
	UActivationFeature* GetActivationFeature() const {return this->ActivationFeature;};
	UExecutionFeature* GetExecutionFeature() const {return this->ExecutionFeature;};
	TArray<UOnHitFeature*> GetOnHitFeature() const {return this->OnHitFeature;};
};
