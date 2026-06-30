// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Core/EntityClass.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "PlayerClass.generated.h"

/**
 * APlayerClass
 * Classe do jogador.
 */


struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
struct FSkillContext;
class USkillDataAsset;

// Delegate para notificar mudanças de vida
DECLARE_DELEGATE_OneParam(FOnHealthChangedSignature, float);

UCLASS(Blueprintable, BlueprintType)
class HERALD_OF_OBLIVION_API APlayerClass : public AEntityClass
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerClass();
	
	FOnHealthChangedSignature OnHealthChanged;

// Métodos
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Carrega os Assets de ativação necessários da skill
	void LoadActivationSkillAssets(USkillInstance* SkillInstance) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// TArray<USkillInstance*> GetEquippedSkillsInstances() const {return this->EquippedSkillsInstances;};
	// TArray<FPrimaryAssetId> GetUISkills() const {return this->UISkills;};

	USpringArmComponent* GetSpringArmComponent() const {return this->SpringArm;};
	UCameraComponent* GetCameraComponent() const {return this->Camera;};
	
protected:
	virtual void DefineAttributes() override;
	
	// Define as skills iniciais da entidade
	UFUNCTION(BlueprintCallable, Category="Functions")
	virtual void DefineSkills() override;
	
	// Recebe um id e cria uma instancia e o adiciona a SkillInstances
	UFUNCTION(BlueprintCallable, Category="Functions")
	void TakeSkill(FPrimaryAssetId AssetId);

	bool HasSkill(FPrimaryAssetId SkillId) const;
	
	// Recebe uma instância de skill e equipa ou desequipa
	UFUNCTION(BlueprintCallable, Category="Functions")
	void ToggleEquipSkill(USkillInstance* InSkillInstance);
	
	// Recebe um atributo simbólico, o método calcula os atributos verdadeiros baseado no valor atual dele
	UFUNCTION(BlueprintCallable, Category="Functions")
	void UpdateSimbolicAttribute(EEntitySimbolicAttributeEnum SimbolicAttribute);
	// Recebe um atributo verdadeiro alvo e um novo valor, atualiza o atributo em questão na entidade
	UFUNCTION(BlueprintCallable, Category="Functions")
	void UpdateTrueAttribute(EEntityTrueAttributeEnum TrueAttribute, float NewValue);
	// Incrementa o atributo simbolico alvo
	UFUNCTION(BlueprintCallable, Category="Functions")
	void IncrementSimbolicAttribute(EEntitySimbolicAttributeEnum TargetSimbolicAttribute);
	
	// Métodos com lógica executada ao apertar e soltar a skil
	void CastFirstSkill() {if (EquippedSkillsInstances.IsValidIndex(0)) HandleCastSkill(EquippedSkillsInstances[0]);};
	void CastSecondSkill() {if (EquippedSkillsInstances.IsValidIndex(1)) HandleCastSkill(EquippedSkillsInstances[1]);};
	void CastThirdSkill() {if (EquippedSkillsInstances.IsValidIndex(2)) HandleCastSkill(EquippedSkillsInstances[2]);};
	void CastFourthSkill() {if (EquippedSkillsInstances.IsValidIndex(3)) HandleCastSkill(EquippedSkillsInstances[3]);};
	
	void ReleasedFirstSkill() {if (EquippedSkillsInstances.IsValidIndex(0)) HandleReleasedSkill(EquippedSkillsInstances[0]);};
	void ReleasedSecondSkill() {if (EquippedSkillsInstances.IsValidIndex(1)) HandleReleasedSkill(EquippedSkillsInstances[1]);};
	void ReleasedThirdSkill() {if (EquippedSkillsInstances.IsValidIndex(2)) HandleReleasedSkill(EquippedSkillsInstances[2]);};
	void ReleasedFourthSkill() {if (EquippedSkillsInstances.IsValidIndex(3)) HandleReleasedSkill(EquippedSkillsInstances[3]);};
	
	void HandleCastSkill(USkillInstance* InSkillInstance);
	void HandleReleasedSkill(USkillInstance* InSkillInstance);
	
	// void CastFirstQuickAccess() {if (EquippedSkillsInstances.IsValidIndex(0)) HandleCastQuickAccess(EquippedSkillsInstances[0]);};
	// void CastSecondQuickAccess() {if (EquippedSkillsInstances.IsValidIndex(1)) HandleCastQuickAccess(EquippedSkillsInstances[1]);};
	// void CastThirdQuickAccess() {HandleCastQuickAccess(2);};
	//
	// void HandleCastQuickAccess(int slot);
	
// Propriedades
public:
	// As skills equipadas do jogador
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Properties")
	TArray<USkillInstance*> EquippedSkillsInstances;
	
	// O Inventario do jogador
	// UPROPERTY(EditAnywhere, Category="Player Properties")
	// TArray<UItemInstance*> Inventory;
	
	// Um mapa de itens consumíveis
	// UPROPERTY(EditAnywhere, Category="Player Properties")
	// TMap<FName, UItemInstance*> QuickAccess;
	
	// As proezas de combate conquistadas pelo jogador
	// UPROPERTY(EditAnywhere, Category="Player Properties")
	// TArray<ECombatFeatEnum> CombatFeats;
	
	// Zoom máximo possível na camera
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	float MaxZoom;
	
	// Zoom minimo possível na camera
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	float MinZoom;	
	
	// Velocidade de interpolação do zoom, quanto menor, mais lento
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	float ZoomInterpSpeed;	
	
	// Usado no processo de interpolação do zoom da camera
	float DesiredZoom;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	USpringArmComponent* SpringArm;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	UCameraComponent* Camera;
	
	// Referências para o Editor
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputAction> CastSkillAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputAction> MouseLookAction;
	
	// Função que processa o Input
	UFUNCTION(BlueprintCallable, Category="Functions")
	void Move(const FInputActionValue& Value);
	
	// Função que processa o Input
	UFUNCTION(BlueprintCallable, Category="Functions")
	void MouseLook(const FInputActionValue& Value);
};
