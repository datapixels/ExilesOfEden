// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/TargetInterface.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float Damage, ACharacter* Target, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(Target) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(Target, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(Target->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		const FTransform& TargetTransform = Target->GetActorTransform();
		FVector Offset(0.f, 0.f, 100.f); // Offset above the target
		FVector Location = TargetTransform.TransformPosition(Offset);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetWorldLocation(Location);
		DamageText->SetDamageText(Damage, bBlockedHit, bCriticalHit);
	}
	
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);	
	}

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

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &AAuraPlayerController::RollPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	
	AuraInputComponent->BindAbilityActions(InputConfig, this,
		&ThisClass::AbilityInputTagPressed,
		&ThisClass::AbilityInputTagReleased,
		&ThisClass::AbilityInputTagHeld);

	
}

void AAuraPlayerController::RollPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Roll Pressed"));
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
	GetHitResultUnderCursor(ECC_Visibility, false, CursorTraceHit);
	if (!CursorTraceHit.bBlockingHit) return;

	LastActor = ThisActor;

	ThisActor = CursorTraceHit.GetActor();

	if (LastActor != ThisActor)
	{
		if (LastActor != nullptr) LastActor->UnhighlightActor();
		if (ThisActor != nullptr) ThisActor->HighlightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor != nullptr;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);	
		}
		return;
	}

	if (GetASC())
	{
		GetASC()->AbilityInputTagReleased(InputTag);	
	}
	
	// if (!bTargeting && !bAutoAttackInDirection)
	// {
	// 	APawn* ControllerPawn = GetPawn<APawn>();
	// 	if (FollowTime < ShortPressThreshold && ControllerPawn != nullptr)
	// 	{
	// 		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
	// 			this,
	// 			ControllerPawn->GetActorLocation(), CachedDestination);
	//
	// 		if (NavPath != nullptr && NavPath->PathPoints.Num() > 0)
	// 		{
	// 			Spline->ClearSplinePoints();
	// 			for (const FVector& PathPoint : NavPath->PathPoints)
	// 			{
	// 				Spline->AddSplinePoint(PathPoint, ESplineCoordinateSpace::World, false);
	// 			}
	//
	// 			
	// 			CachedDestination = NavPath->PathPoints.Last();
	// 			bAutoRunning = true;
	// 		}
	// 	}
	// 	FollowTime = 0.f;
	// 	bTargeting = false;
	// }
}

void AAuraPlayerController::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	if (GetASC())
	{
		GetASC()->AbilityInputTagHeld(InputTag);	
	}
	// else
	// {
	// 	FollowTime += GetWorld()->GetDeltaSeconds();
	// 	
	// 	if (CursorTraceHit.bBlockingHit)
	// 	{
	// 		CachedDestination = CursorTraceHit.ImpactPoint;
	// 	}
	// 	else if (CachedDestination.IsZero())
	// 	{
	// 		CachedDestination = CursorTraceHit.ImpactPoint;
	// 	}
	//
	// 	if (APawn* ControllerPawn = GetPawn<APawn>())
	// 	{
	// 		const FVector WorldDirection = (CachedDestination - ControllerPawn->GetActorLocation()).GetSafeNormal();
	// 		ControllerPawn->AddMovementInput(WorldDirection, 1.f);
	// 	}
	// }
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return AuraAbilitySystemComponent;
}

// void AAuraPlayerController::AutoRun()
// {
// 	if (!bAutoRunning) return;
// 	
// 	if (APawn* ControlledPawn = GetPawn<APawn>())
// 	{
// 		const FVector PlayerLocation = ControlledPawn->GetActorLocation();
// 		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(PlayerLocation, ESplineCoordinateSpace::World);
// 		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
// 		ControlledPawn->AddMovementInput(Direction);
//
// 		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
// 		if (DistanceToDestination <= AutoRunAcceptanceRadius)
// 		{
// 			bAutoRunning = false;
// 		}
// 	}
// }
