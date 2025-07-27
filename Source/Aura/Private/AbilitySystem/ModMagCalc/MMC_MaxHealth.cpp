// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"


UMMC_MaxHealth::UMMC_MaxHealth()
{
	StaminaDef.AttributeToCapture = UAuraAttributeSet::GetStaminaAttribute();
	StaminaDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	StaminaDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(StaminaDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;

	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Stamina = 0.f;

	GetCapturedAttributeMagnitude(StaminaDef, Spec, EvaluateParameters, Stamina);

	Stamina = FMath::Max<float>(Stamina, 0.f);

	ICombatInterface* CombatInterface =  Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());

	const int32 Level = CombatInterface->GetPlayerLevel();

	return  80.f + (2.5f * Stamina) + 10.f * Level; // Base health + stamina contribution + level contribution
}
