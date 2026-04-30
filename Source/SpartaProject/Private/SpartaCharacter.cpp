// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"


ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = GetCharacterMovement()->MaxWalkSpeed;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	bIsFrozen = false;
	RequiredEscapePressCount = 20;
	CurrentEscapePressCount = 0;

	MaxHealth = 100.f;
	Health = MaxHealth;
}




void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
}

void ASpartaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move
				);
			}
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);
			}
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
				);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
				);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}
			if (PlayerController->EscapeAction)
			{
				EnhancedInput->BindAction(
					PlayerController->EscapeAction,
					ETriggerEvent::Started,
					this,
					&ASpartaCharacter::Escape
				);
			}
		}
	}

}




void ASpartaCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;
	if (bIsFrozen) return;

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
	if (bIsFrozen) return;

	//if (!Controller) return; -> 이거 할 필요가 없는 이유: Jump/StopJumping 내부에 체크 내제되어 있음
	if (value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	if (bIsFrozen) return;

	//if (!Controller) return;
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
	//if (!Controller) return;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}

}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	//if (!Controller) return;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{
	if (!Controller) return;
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::Escape(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Escape Function Called"));

	if (!Controller) return;
	if (!bIsFrozen) return;
	CurrentEscapePressCount++;

	GEngine->AddOnScreenDebugMessage(1001, 2.f, FColor::Cyan, FString::Printf(TEXT("Player Frozen! Press F to Escape.")));
	UE_LOG(LogTemp, Warning, TEXT("Ice Escape Count: %d / %d"), CurrentEscapePressCount, RequiredEscapePressCount);

	if (CurrentEscapePressCount >= RequiredEscapePressCount)
	{
		UnfreezePlayer();
	}

}



float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
	//UE_LOG(LogTemp, Warning, TEXT("Health increased to: %f"), Health);
}

void ASpartaCharacter::FreezePlayer(int32 RequiredPressCount)
{
	if (bIsFrozen) return;

	bIsFrozen = true;
	RequiredEscapePressCount = RequiredPressCount;
	CurrentEscapePressCount = 0;

	GetCharacterMovement()->DisableMovement();
	GEngine->AddOnScreenDebugMessage(1001, 2.f, FColor::Cyan, FString::Printf(TEXT("Player Frozen! Press F to Escape.")));
	UE_LOG(LogTemp, Warning, TEXT("Player Frozen! Press EscapeIce key %d times."), RequiredEscapePressCount);
}

void ASpartaCharacter::UnfreezePlayer()
{
	if (!bIsFrozen) return;

	bIsFrozen = false;
	CurrentEscapePressCount = 0;

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	UE_LOG(LogTemp, Warning, TEXT("Player Unfrozen!"));
}

float ASpartaCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
	UpdateOverheadHP();
	//UE_LOG(LogTemp, Warning, TEXT("Health decreased to: %f"), Health);
	if (Health <= 0.f)
	{
		OnDeath();
	}

	return ActualDamage;
}


void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	const float HPPercent = (MaxHealth > 0.f)
		? FMath::Clamp(Health / MaxHealth, 0.f, 1.f)
		: 0.f;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
	if (UProgressBar* HPBar = Cast<UProgressBar>(OverheadWidgetInstance->GetWidgetFromName(TEXT("PB_HP"))))
	{
		HPBar->SetPercent(HPPercent);
	}
}
