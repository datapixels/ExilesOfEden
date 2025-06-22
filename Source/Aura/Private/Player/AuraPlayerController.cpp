// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/TargetInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);

	Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);

	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& Value)
{
	const FVector2D InputAxisVector = Value.Get<FVector2D>();
	if (InputAxisVector.IsNearlyZero())
	{
		return;
	}
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorTraceHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorTraceHit);
	if (!CursorTraceHit.bBlockingHit) return;

	LastActor = ThisActor;

	ThisActor = CursorTraceHit.GetActor();

	if (LastActor == ThisActor)
	{
		return;
	}

	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			// highlight new actor
			ThisActor->HighlightActor();
		}
		else
		{
			// do nothing
		}
	}
	else
	{
		if (ThisActor == nullptr)
		{
			// unhighlight last actor
			LastActor->UnhighlightActor();
		}
		else
		{
			if (LastActor != ThisActor)
			{
				// unhighlight last actor and highlight new actor
				LastActor->UnhighlightActor();
				ThisActor->HighlightActor();	
			}
			else
			{
				// do nothing as its same actor
			}
		}
	}

}
