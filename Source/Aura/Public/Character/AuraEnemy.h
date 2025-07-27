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
	AAuraEnemy();

	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = false;

	/** Enemy interface */
	virtual void HighlightActor() override;
	virtual void UnhighlightActor() override;
	/** End enemy interface */

	/** Combat interface */
	virtual int32 GetPlayerLevel() override { return Level; }

		
protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int Level = 1;
	
};
