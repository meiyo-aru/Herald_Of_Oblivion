// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/SkillDataAsset.h"

#include "Core/EntityClass.h"
#include "Core/SkillInstance.h"
#include "Data/SpecializationDataAsset.h"
#include "SkillFeatures/Activation/ActivationFeature.h" 
#include "SkillFeatures/Execution/ExecutionFeature.h"
#include "SkillFeatures/OnHit/OnHitFeature.h"

USkillDataAsset::USkillDataAsset()
{
}
// Recebe uma entidade e cria uma instancia da habilidade para ela
USkillInstance* USkillDataAsset::CreateInstance(AEntityClass* Owner)
{
	// Define uma entidade como o Outer e cria o objeto
	USkillInstance* Instance =  NewObject<USkillInstance>(Owner);
	
	// Novo array que vai receber as cópias
	TArray<UOnHitFeature*> DuplicatedArray;
	
	for (UOnHitFeature* SourceObj : this->OnHitFeature)
	{
		if (SourceObj)
		{
			// Duplica o objeto individualmente
			UOnHitFeature* NewObj = DuplicateObject<UOnHitFeature>(SourceObj, Instance);
        
			if (NewObj)
			{
				DuplicatedArray.Add(NewObj);
			}
		}
	}
	
	Instance->Initialize(Owner,
	                     this,
	                     DuplicateObject<UActivationFeature>(this->ActivationFeature, Instance), 
	                     DuplicateObject<UExecutionFeature>(this->ExecutionFeature, Instance),
	                     DuplicatedArray);
	return Instance;
}

// Função para que sempre que a especializacao for alterada no editor o valor padrao do modificador seja alterado também
/*
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
*/
