// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AnimNotifyClass.h"

#include "Components/SkeletalMeshComponent.h"
#include "Core/EntityClass.h"

void UAnimNotifyClass::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (AEntityClass* Entity = Cast<AEntityClass>(MeshComp->GetOwner()))
	{
		Entity->OnAnimationNotify.Broadcast(EventName);
	}
}
