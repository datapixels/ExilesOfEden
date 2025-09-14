// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/CharacterClassInfo.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintPure, Category="Aura Ability System Library|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="Aura Ability System Library|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);


	UFUNCTION(BlueprintCallable, Category="Aura Ability System Library|ClassDefaults")
	static  void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category="Aura Ability System Library|ClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category="Aura Ability System Library|ClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="Aura Ability System Library|GameplayEfffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="Aura Ability System Library|GameplayEfffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category="Aura Ability System Library|GameplayEfffects")
	static void SetIsBlockedHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bIsBlocked);

	UFUNCTION(BlueprintCallable, Category="Aura Ability System Library|GameplayEfffects")
	static void SetIsCriticalHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bIsCritical);
};
