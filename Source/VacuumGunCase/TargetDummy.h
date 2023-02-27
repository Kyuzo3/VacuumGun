// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VacuumInterface.h"
#include "TargetDummy.generated.h"

class UHealthBarComponent;

UCLASS()
class VACUUMGUNCASE_API ATargetDummy : public ACharacter, public IVacuumInterface
{
	GENERATED_BODY()

private:
	
	ATargetDummy();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/*VacuumableInterface*/
	void GetHit_Implementation();
	/*VacuumableInterface*/
	void RegenerateHealth(float DeltaTime);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartHealthRegenTimer();

	void StartHealthRegen();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HealthRegenStartTime = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HealthRegenRate = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool CanRegenHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactMontage;

	UPROPERTY(VisibleAnywhere, Category = "Widgets")
	UHealthBarComponent* HealthBarWidget;

	FTimerHandle HealthRegenTimer;

public:
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	void UpdateHealthBar();	
};