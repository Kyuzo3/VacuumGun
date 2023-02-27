// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoCount.h"
#include "Components/TextBlock.h"

void UAmmoCount::SetAmmoText(int Value)
{
	FString IntegerString = FString::FromInt(Value);
	FText NewText = FText::FromString(IntegerString);
	AmmoText->SetText(NewText);
}
