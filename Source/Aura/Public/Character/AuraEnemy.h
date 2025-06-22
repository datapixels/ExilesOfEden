// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacterBase.h"
#include "Interaction/TargetInterface.h"
#include "AuraEnemy.generated.h"

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public ITargetInterface
{
	GENERATED_BODY()
public:
	virtual void HighlightActor() override;
	virtual void UnhighlightActor() override;

	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = false;
};
