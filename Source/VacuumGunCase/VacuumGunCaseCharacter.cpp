// Copyright Epic Games, Inc. All Rights Reserved.

#include "VacuumGunCaseCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "VacuumGun.h"
#include "UMG.h"

AVacuumGunCaseCharacter::AVacuumGunCaseCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void AVacuumGunCaseCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVacuumGunCaseCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVacuumGunCaseCharacter::Look);
		EnhancedInputComponent->BindAction(PerformVacuumAction, ETriggerEvent::Triggered, this, &AVacuumGunCaseCharacter::PerformVacuum);
		EnhancedInputComponent->BindAction(StopPerformAction, ETriggerEvent::Triggered, this, &AVacuumGunCaseCharacter::StopVacuum);
		EnhancedInputComponent->BindAction(PerformFireAction, ETriggerEvent::Triggered, this, &AVacuumGunCaseCharacter::FireVacuumGun);
	}
}

void AVacuumGunCaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	EquipVacuumGun();
	AddCrosshairToScreen();
}

void AVacuumGunCaseCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AVacuumGunCaseCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

UCameraComponent* AVacuumGunCaseCharacter::GetPlayerCamera_Implementation()
{
	return FirstPersonCameraComponent;
		//->GetForwardVector() * VacuumTraceStartOffset + FirstPersonCameraComponent->GetComponentLocation();
}

void AVacuumGunCaseCharacter::EquipVacuumGun()
{
	UWorld* World = GetWorld();
	if (!World || !VacuumGunClass) return;
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	VacuumGun = World->SpawnActor<AVacuumGun>(VacuumGunClass);
	VacuumGun->SetOwner(this);
	VacuumGun->SetInstigator(this);
	VacuumGun->AttachToComponent(GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	bHasRifle = true;
}

void AVacuumGunCaseCharacter::PerformVacuum()
{
	VacuumGun->EnableVacuuming(true);
}

void AVacuumGunCaseCharacter::StopVacuum()
{
	VacuumGun->EnableVacuuming(false);
	VacuumGun->CancelVacuumEffect();
}

void AVacuumGunCaseCharacter::FireVacuumGun()
{
	VacuumGun->FireAmmo();
}

void AVacuumGunCaseCharacter::AddCrosshairToScreen()
{
	if (CrosshairWidget && GetWorld())
	{
		UUserWidget* CrosshairWidgetInstance = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), CrosshairWidget);
		CrosshairWidgetInstance->AddToViewport();
	}
}