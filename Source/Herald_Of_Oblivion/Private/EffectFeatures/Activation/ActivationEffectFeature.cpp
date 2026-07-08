// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectFeatures/Activation/ActivationEffectFeature.h"
#include "Core/EffectInstance.h"

void UActivationEffectFeature::Init(FHitOverlapResult& HitOverlapResult)
{
	Super::Init(HitOverlapResult);
	
	Execute(HitOverlapResult);
}

void UActivationEffectFeature::Execute(FHitOverlapResult& HitOverlapResult)
{

}