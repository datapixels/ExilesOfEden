// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);

	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool isLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (isLocallyControlled)
	{
		SendTargetData();
	}
	else
	{
		const FGameplayAbilitySpecHandle AbilitySpecHandle = GetAbilitySpecHandle();
		const FPredictionKey PredictionKey = GetActivationPredictionKey();
		
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(AbilitySpecHandle, PredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(AbilitySpecHandle, PredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}

	
}

FGameplayAbilityTargetDataHandle UTargetDataUnderMouse::GetTargetData()
{
	APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult HitResult;
	
	// Get the hit location based on input device
	float MouseX, MouseY;
	if (PlayerController->GetMousePosition(MouseX, MouseY))
	{
		// Mouse input - use cursor position
		PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	}
	else
	{
		// Gamepad input - use analog stick direction
		APawn* ControlledPawn = PlayerController->GetPawn();
		if (ControlledPawn)
		{
			const FVector Start = ControlledPawn->GetActorLocation();
			
			// Get the right stick X and Y values
			float StickX = 0.f;
			float StickY = 0.f;
			PlayerController->GetInputAnalogStickState(EControllerAnalogStick::CAS_RightStick, StickX, StickY);
			
			FVector Direction;
			if (FMath::Abs(StickX) > 0.1f || FMath::Abs(StickY) > 0.1f)
			{
				// If stick has meaningful input, use that direction
				Direction = FVector(StickY, StickX, 0.f).GetSafeNormal();
			}
			else
			{
				// If stick is neutral, fall back to actor forward direction
				Direction = ControlledPawn->GetActorForwardVector();
			}
			
			const FVector End = Start + (Direction * 10000.f); // 100 meters in stick direction

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(ControlledPawn);
			
			PlayerController->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);
			if (!HitResult.bBlockingHit)
			{
				// If we didn't hit anything, use the end point
				HitResult.Location = End;
				HitResult.ImpactPoint = End;
			}
		}
	}

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	TargetData->HitResult = HitResult;
	DataHandle.Add(TargetData);
	
	return DataHandle;
}

void UTargetDataUnderMouse::SendTargetData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	FGameplayAbilityTargetDataHandle DataHandle = GetTargetData();

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
