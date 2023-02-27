// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoCount.generated.h"

class UTextBlock;

UCLASS()
class VACUUMGUNCASE_API UAmmoCount : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoText;

	void SetAmmoText(int Value);
};
