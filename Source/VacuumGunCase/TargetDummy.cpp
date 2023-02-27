// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetDummy.h"
#include "Components/SkeletalMeshComponent.h"
#include "HealthBarComponent.h"

ATargetDummy::ATargetDummy()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
}

void ATargetDummy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void ATargetDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RegenerateHealth(DeltaTime);
}

void ATargetDummy::GetHit_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
	}
}

void ATargetDummy::RegenerateHealth(float DeltaTime)
{
	if (CanRegenHealth)
	{
		CurrentHealth += HealthRegenRate * DeltaTime;
		if (HealthBarWidget)
		{
			HealthBarWidget->SetHealthPercent(CurrentHealth / MaxHealth);
		}
		if (CurrentHealth >= MaxHealth)
		{
			CurrentHealth = MaxHealth;
			CanRegenHealth = false;
		}
	}
}

void ATargetDummy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATargetDummy::StartHealthRegenTimer()
{
	CanRegenHealth = false;
	GetWorldTimerManager().ClearTimer(HealthRegenTimer);
	GetWorldTimerManager().SetTimer(HealthRegenTimer, this, &ATargetDummy::StartHealthRegen, HealthRegenStartTime);
}

void ATargetDummy::StartHealthRegen()
{	
	CanRegenHealth = true;
}

float ATargetDummy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);	
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	StartHealthRegenTimer();
	UpdateHealthBar();
	return DamageAmount;
}

void ATargetDummy::UpdateHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(CurrentHealth / MaxHealth);
	}
}