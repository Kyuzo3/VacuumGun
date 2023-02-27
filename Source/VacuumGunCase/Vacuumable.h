// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VacuumInterface.h"
#include "Components/TimelineComponent.h"
#include "Vacuumable.generated.h"


class UCurveFloat;

UCLASS()
class VACUUMGUNCASE_API AVacuumable : public AActor, public IVacuumInterface
{
	GENERATED_BODY()
	
public:
	AVacuumable();
	virtual void Tick(float DeltaTime) override;
	/*VacuumableInterface*/
	UPrimitiveComponent* GetVacuumableMesh_Implementation();
	void ShrinkDown_Implementation(AVacuumGun* VacuumGun);
	void GetFired_Implementation();
	/*VacuumableInterface*/

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void DisableVacuumability();

	UFUNCTION(BlueprintCallable)
	void EnableVacuumability();

	//Timeline functions for shrinking visual effects
	UFUNCTION()
	void ShrinkDownUpdate(float Alpha);

	UFUNCTION()
	void ShrinkDownFinished();

	UFUNCTION()
	void ExpandToNormalUpdate(float Alpha);

	UFUNCTION()
	void SetTimelineFunctions();

	UFUNCTION()
	void DisableOnHit();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vacuum")
	float Damage = 15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vacuum")
	float MinVelocityToDamageEnemy = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vacuum")
	float MaxVelocityToDisableHitEvent = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vacuum")
	float ReVacuumabilityResetTime = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vacuum")
	float OnHitDisableTime = 10.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vacuum")
	bool DisableTimerStarted;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vacuum")
	AVacuumGun* TargetVacuumGun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vacuum")
	FVector ShrinkStartLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vacuum")
	FVector ShrinkStartScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vacuum")
	UCurveFloat* ShirnkDownCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vacuum")
	UCurveFloat* ExpandToNormalCurve;
	
	FTimeline ShrinkDownTimeline;

	FTimeline ExpandToNormalTimeline;

	FVector MeshOriginalSize;
};
