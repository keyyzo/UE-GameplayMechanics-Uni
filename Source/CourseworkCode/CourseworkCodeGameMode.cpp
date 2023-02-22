// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CourseworkCodeGameMode.h"
#include "CourseworkCodeHUD.h"
#include "CourseworkCodeCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACourseworkCodeGameMode::ACourseworkCodeGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ACourseworkCodeHUD::StaticClass();
}
