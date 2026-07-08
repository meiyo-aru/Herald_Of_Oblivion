// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectFeatures/Execution/ExecutionApplyDamage.h"

#include "Core/EntityClass.h"
#include "Structs/SkillStructs.h"

void UExecutionApplyDamage::Execute(const FHitOverlapResult& HitOverlapResult)
{
	Super::Execute(HitOverlapResult);
	
	if (UEffectInstance* Instance = Cast<UEffectInstance>(GetOuter()))
	{
		if (AEntityClass* Target = Instance->TargetEntity.Get())
		{
			FDamageStruct DamageStruct = FDamageStruct(TypeDamage);
			
			DamageStruct.Damage = Damage;
			DamageStruct.HitInfo = HitOverlapResult.HitResult;
			AController* ControllerInstigator = HitOverlapResult.ControllerInstigator.Get();
			AActor* DamageCauser = HitOverlapResult.DamageCauser.Get();
			
			if (IsValid(DamageCauser) && IsValid(ControllerInstigator))
			{
				Target->TakeDamage(Damage, DamageStruct, ControllerInstigator, DamageCauser);
			}
		}
	}
	
}
