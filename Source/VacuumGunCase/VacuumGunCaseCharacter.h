// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "VacuumInterface.h"
#include "VacuumGunCaseCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class AVacuumGun;
class UInputAction;
class UInputMappingContext;

UCLASS(config = Game)
class AVacuumGunCaseCharacter : public ACharacter, public IVacuumInterface
{
	GENERATED_BODY()

public:	

	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/*VacuumInterface*/
	UCameraComponent* GetPlayerCamera_Implementation();
	/*VacuumInterface*/

private:

	AVacuumGunCaseCharacter();
	virtual void BeginPlay();

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);	

	UFUNCTION(BlueprintCallable)
	void EquipVacuumGun();

	UFUNCTION(BlueprintCallable)
	void PerformVacuum();

	UFUNCTION(BlueprintCallable)
	void StopVacuum();

	UFUNCTION(BlueprintCallable)
	void FireVacuumGun();

	void AddCrosshairToScreen();

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PerformVacuumAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StopPerformAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PerformFireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AVacuumGun> VacuumGunClass;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	AVacuumGun* VacuumGun;

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UUserWidget> CrosshairWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bHasRifle;
};