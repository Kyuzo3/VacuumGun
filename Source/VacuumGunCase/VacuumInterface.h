// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VacuumInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UVacuumInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VACUUMGUNCASE_API IVacuumInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UCameraComponent* GetPlayerCamera();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetVacuumTraceEndLocation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UPrimitiveComponent* GetVacuumableMesh();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ShrinkDown(AVacuumGun* VacuumGun);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddToAmmo(AVacuumable* Vacuumable);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GetFired();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GetHit();
};
