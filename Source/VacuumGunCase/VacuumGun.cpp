// Fill out your copyright notice in the Description page of Project Settings.


#include "VacuumGun.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Vacuumable.h"
#include "Camera/CameraComponent.h"
#include "AmmoCount.h"
#include "Components/TextBlock.h"

// Sets default values
AVacuumGun::AVacuumGun()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMeshComponent"));
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(WeaponMesh);	
}

void AVacuumGun::BeginPlay()
{
	Super::BeginPlay();
	SetOwnerWithDelay();

	InitializeAmmoCounterWidget();
}

void AVacuumGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Vacuum(DeltaTime);
}

void AVacuumGun::AddToAmmo_Implementation(AVacuumable* Vacuumable)
{
	if (Vacuumable)
	{
		Ammo.Add(Vacuumable);
		UpdateAmmoCounterWidget();
	}
}

void AVacuumGun::EnableVacuuming(bool ShouldVacuum)
{
	if (IsVacuuming != ShouldVacuum)
	{
		IsVacuuming = ShouldVacuum;
	}
}



void AVacuumGun::SetOwnerWithDelay()
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AVacuumGun::GetOwnerInterface);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.2f, false);
}

void AVacuumGun::GetOwnerInterface()
{
	if (GetOwner())
	{
		OwnerInterface = Cast<IVacuumInterface>(GetOwner());
	}	
}

void AVacuumGun::Vacuum(float DeltaTime)
{
	if (IsVacuuming && OwnerInterface)
	{
		
		TraceForVacuum();

		//Pull & SetProperties of caught actors
		PullAndAbsorb(DeltaTime);

		// if trace miss, set properties of the last frame hit actors
		CancelVacuumEffect();

		LastFrameHitActors = CurrentFrameHitActors;
		CurrentFrameHitActors.Empty();
	}
}

void AVacuumGun::TraceForVacuum()
{
	FVector StartPoint = GetTraceStartLocation();
	FVector EndPoint = GetTraceEndLocation();
	UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartPoint, EndPoint, VacuumTraceRadius, ObjectTypesForVacuumTrace, false, ActorsToIgnore, ShouldDrawDebugForTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, VacuumHitResultArray, true);	
}

void AVacuumGun::PullAndAbsorb(float DeltaTime)
{
	for (FHitResult HitResult : VacuumHitResultArray)
	{		
		IVacuumInterface* HitVacuumable = Cast<IVacuumInterface>(HitResult.GetActor());
		if (HitVacuumable)
		{
			CurrentFrameHitActors.AddUnique(HitResult.GetActor());

			/* Uncomment the if check below if you want to lower / cancel the object's velocity when it gets hit by the vacuum for the first time (if it wasn't hit by it lastframe).
			 * Since the object's velocity will be lowered/canceled it will stick to vacuum more easily. I don't like it because it feels more like a gravity gun when you cancel object's velocity.
			 */
			/*if (!LastFrameHitActors.Contains(HitResult.GetActor()))
			{				
				UPrimitiveComponent* VacuumableMesh = Execute_GetVacuumableMesh(HitResult.GetActor());
				VacuumableMesh->SetPhysicsLinearVelocity(VacuumableMesh->GetPhysicsLinearVelocity() / 5.f);				
			}*/
			
			if (CanPlayerSeeThisObject(HitResult))
			{
				HitResult.GetComponent()->AddForce(GetForceToAdd(HitResult, DeltaTime), FName(""), true);
				HitResult.GetComponent()->SetEnableGravity(false);				
				if (CanAbsorbThisActor(HitResult))
				{
					Absorb(HitResult.GetActor());
				}
			}
		}
	}
}

bool AVacuumGun::CanPlayerSeeThisObject(FHitResult& HitResult)
{
	UCameraComponent* PlayerCamera = Execute_GetPlayerCamera(GetOwner());
	UKismetSystemLibrary::LineTraceSingleForObjects(this, PlayerCamera->GetComponentLocation(), HitResult.ImpactPoint, ObjectTypesForVisibiltyTrace, false, ActorsToIgnore, ShouldDrawDebugForTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, VisibilityHitResult, true);
	return !VisibilityHitResult.bBlockingHit;
}

FVector AVacuumGun::GetForceToAdd(FHitResult& HitResult, float DeltaTime)
{
	FVector NormalizedToMuzzle = (WeaponMesh->GetSocketLocation(FName("Muzzle")) - HitResult.GetActor()->GetActorLocation()).GetSafeNormal();
	FVector VacuumForceVector = (NormalizedToMuzzle * VacuumForce) * DeltaTime;
	return VacuumForceVector;
}

void AVacuumGun::TraceForAbsorb()
{
	FVector TraceCenter = GetTraceStartLocation();
	UKismetSystemLibrary::SphereTraceMultiForObjects(this, TraceCenter, TraceCenter, VacuumTraceRadius, ObjectTypesForVacuumTrace, false, ActorsToIgnore, ShouldDrawDebugForTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, AbsorbHitResultArray, true);
}

void AVacuumGun::Absorb(AActor* HitActor)
{
	if (IsVacuuming && OwnerInterface)
	{
		IVacuumInterface* HitVacuumable = Cast<IVacuumInterface>(HitActor);
		if (HitVacuumable)
		{			
			LastFrameHitActors.Remove(HitActor);
			CurrentFrameHitActors.Remove(HitActor);
			Execute_ShrinkDown(HitActor, this);
		}	
	}
}

bool AVacuumGun::CanAbsorbThisActor(FHitResult HitResult)
{
	if ((GetOwner()->GetActorLocation() - HitResult.ImpactPoint).Length() < AbsorbRange)
	{
		FVector TargetLocation = HitResult.ImpactPoint;
		UCameraComponent* PlayerCamera = Execute_GetPlayerCamera(GetOwner());
		FVector PlayerCameraLocation = PlayerCamera->GetComponentLocation();
		FVector TargetDirection = (TargetLocation - PlayerCameraLocation).GetSafeNormal();
		FVector CameraForward = PlayerCamera->GetForwardVector();
		const double ChosTheta = FVector::DotProduct(CameraForward, TargetDirection);
		double Theta = FMath::Acos(ChosTheta);
		Theta = FMath::RadiansToDegrees(Theta);		
		const FVector CrossProduct = FVector::CrossProduct(CameraForward, TargetDirection);		
		if (Theta < AbsorbWindow && Theta > -AbsorbWindow)
		{
			return true;
		}
		return false;
	}
	return false;
}

bool AVacuumGun::CanFire()
{	
	return !Ammo.IsEmpty() && Ammo.Last() != nullptr;
}

void AVacuumGun::CancelVacuumEffect()
{
	for (AActor* LastHitInterface : LastFrameHitActors)
	{
		if (!CurrentFrameHitActors.Contains(LastHitInterface))
		{
			UPrimitiveComponent* VacuumableMesh = Execute_GetVacuumableMesh(LastHitInterface);
			VacuumableMesh->SetEnableGravity(true);
			VacuumableMesh->SetPhysicsLinearVelocity(VacuumableMesh->GetPhysicsLinearVelocity() / 3.f);
		}
	}
}

void AVacuumGun::FireAmmo()
{	
	if (CanFire() && GetWorld())
	{		
		FVector LuanchDirection = (GetTraceEndLocation() - GetTraceStartLocation()).GetSafeNormal();
		
		TSubclassOf<AVacuumable> BlueprintClass = Ammo.Last()->GetClass();
		AVacuumable* SpawnedVacuumable = GetWorld()->SpawnActor<AVacuumable>(BlueprintClass, GetTraceStartLocation(), FRotator(0.f));
		SpawnedVacuumable->SetOwner(this);
		Ammo.Last()->Destroy();
		Ammo.Pop();
		UpdateAmmoCounterWidget();

		UPrimitiveComponent* SpawnedVacuumableMesh = Execute_GetVacuumableMesh(SpawnedVacuumable);
		if (SpawnedVacuumableMesh)
		{
			Execute_GetFired(SpawnedVacuumable);
			SpawnedVacuumableMesh->AddImpulse(LuanchDirection * AmmoLaunchVelocity, FName(""), true);
		}		
	}
}
FVector AVacuumGun::GetTraceStartLocation()
{
	if (GetOwner())
	{
		UCameraComponent* PlayerCamera = Execute_GetPlayerCamera(GetOwner());
		if (PlayerCamera)
		{
			FVector StartPoint = PlayerCamera->GetForwardVector() * VacuumTraceStartOffset + PlayerCamera->GetComponentLocation();
			return StartPoint;
		}
	}
	return FVector(0.f);
}

FVector AVacuumGun::GetTraceEndLocation()
{
	if (GetOwner())
	{
		UCameraComponent* PlayerCamera = Execute_GetPlayerCamera(GetOwner());
		if (PlayerCamera)
		{
			FVector EndPoint = PlayerCamera->GetForwardVector() * VacuumTraceRange + PlayerCamera->GetComponentLocation();
			return EndPoint;
		}
	}
	return FVector(0.f);
}

void AVacuumGun::InitializeAmmoCounterWidget()
{
	if (AmmoCountClass && GetWorld())
	{
		AmmoCountWidgetClass = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), AmmoCountClass);
		if (AmmoCountWidgetClass)
		{
			AmmoCountWidgetClass->AddToViewport();
			AmmoCountWidget = Cast<UAmmoCount>(AmmoCountWidgetClass);
			UpdateAmmoCounterWidget();
		}
	}
}

void AVacuumGun::UpdateAmmoCounterWidget()
{
	if (AmmoCountWidget)
	{
		AmmoCountWidget->SetAmmoText(Ammo.Num());
	}
}

FVector AVacuumGun::GetMuzzleLocation()
{
	return WeaponMesh->GetSocketLocation(FName("Muzzle"));
}