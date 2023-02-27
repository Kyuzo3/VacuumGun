// Copyright Epic Games, Inc. All Rights Reserved.

#include "VacuumGunCaseGameMode.h"
#include "VacuumGunCaseCharacter.h"
#include "UObject/ConstructorHelpers.h"

AVacuumGunCaseGameMode::AVacuumGunCaseGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/PlayerCharacter/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}
