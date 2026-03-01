// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/SkillDataAsset.h"

#include "Core/EntityClass.h"
#include "Core/SkillInstance.h"
#include "Core/SpecializationDataAsset.h"
#include "SkillFeatures/Activation/ActivationFeature.h" 
#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "SkillFeatures/OnHit/OnHitFeature.h"

USkillDataAsset::USkillDataAsset()
{
	
}
// #if WITH_EDITOR
// void USkillDataAsset::PostEditChangeProperty(FPropertyChangedEvent& e)
// {
// 	Super::PostEditChangeProperty(PropertyChangedEvent);
//     
// 	// Log quando mudar no editor
// 	UE_LOG(LogTemp, Log, TEXT("PostEditChange: Features count = %d"), Features.Num());
//     
// 	for (int32 i = 0; i < EShaderOptionalDataKey::Features.Num(); i++)
// 	{
// 		if (Features[i])
// 		{
// 			UE_LOG(LogTemp, Log, TEXT("  Feature[%d]: %s"), i, *Features[i]->GetName());
// 		}
// 		else
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("  Feature[%d]: NULLPTR!"), i);
// 		}
// 	}
// }
// #endif
// Recebe uma entidade e cria uma instancia da habilidade para ela
USkillInstance* USkillDataAsset::CreateInstance(AEntityClass* Owner)
{
	// Define uma entidade como o Outer e cria o objeto
	USkillInstance* Instance =  NewObject<USkillInstance>(Owner);
	
	Instance->Initialize(Owner,
		this,
		DuplicateObject<UActivationFeature>(this->ActivationFeature, Instance), 
		DuplicateObject<UExecutionFeature>(this->ExecutionFeature, Instance),
		DuplicateObject<UOnHitFeature>(this->OnHitFeature, Instance));
	return Instance;
}

// Função para que sempre que a especializacao for alterada no editor o valor padrao do modificador seja alterado também
#if WITH_EDITOR
void USkillDataAsset::PostEditChangeChainProperty(FPropertyChangedChainEvent& e)
{
	Super::PostEditChangeChainProperty(e);

	// Verifica se os ponteiros do evento são válidos. Se não, sai imediatamente.
	if (e.Property == nullptr || e.MemberProperty == nullptr)
	{
		return;
	}

	// Obtem os nomes com segurança.
	const FName PropertyName = e.PropertyChain.GetActiveNode()->GetValue()->GetFName();

	// Verificação para USpecializationDataAsset
	if (PropertyName == GET_MEMBER_NAME_CHECKED(USkillDataAsset, Specialization))
	{
		if (this->Specialization)
		{
			this->SpecializationName = this->Specialization->GetFName();
		}
		else
		{
			// Se o usuário limpar o campo, limpamos o FName
			SpecializationName = NAME_None;
		}
	}
}
#endif
