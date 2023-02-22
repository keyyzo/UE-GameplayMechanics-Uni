// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CourseworkCodeCharacter.h"
#include "CourseworkCodeProjectile.h"
#include "Curveball.h"
#include "SageWall.h"
#include "FuryShot.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ACourseworkCodeCharacter

ACourseworkCodeCharacter::ACourseworkCodeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// initialsing variables


	// bool checks
	isPlacingWall = false;
	isRotatingWall = false;
	isFuryActivated = false;
	isShooting = false;

	// fire rates for the gun
	furyFireRate = 0.1f;
	autoFireRate = 0.175f;

	// fury shot ability length
	furyFireAbilityLength = 10.0f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// create a scene component which will be used as a spawn location for the Curveball ability
	Curveball_SpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("CurveballLocation"));
	Curveball_SpawnLocation->SetupAttachment(Mesh1P);
	Curveball_SpawnLocation->SetRelativeLocation(FVector(88.533005f, 27.03812f, 157.630051f));
	Curveball_SpawnLocation->SetRelativeRotation(FRotator(-5.8f, -0.585005f, 19.423147f));

	// create a scene component which will be used as a spawn location for the Sage Wall ability
	SageWall_SpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("SageWallSpawnLocation"));
	SageWall_SpawnLocation->SetupAttachment(GetRootComponent());
	SageWall_SpawnLocation->SetRelativeLocation(FVector(60.0f, 0.0f, 65.0f));
	SageWall_SpawnLocation->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}


// getters and setters

// checks and sets if player is placing a Sage Wall
bool ACourseworkCodeCharacter::getIsPlacingWall()
{
	return isPlacingWall;
}

void ACourseworkCodeCharacter::setIsPlacingWall(bool val)
{
	isPlacingWall = val;
}

// checks and sets if player is rotating a Sage Wall
bool ACourseworkCodeCharacter::getIsRotatingWall()
{
	return isRotatingWall;
}

void ACourseworkCodeCharacter::setisRotatingWall(bool val)
{
	isRotatingWall = val;
}

// checks and sets if player has Fury Shot ability activated

bool ACourseworkCodeCharacter::getIsFuryActivated()
{
	return isFuryActivated;
}

void ACourseworkCodeCharacter::setIsFuryActivated(bool val)
{
	isFuryActivated = val;
}


// calls function when character is first loaded into the world

void ACourseworkCodeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACourseworkCodeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event

	// Shooting
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACourseworkCodeCharacter::OnFireAuto);

	// Stop Shooting
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACourseworkCodeCharacter::StopFiring);

	// Fury Fire
	PlayerInputComponent->BindAction("Activate_FuryFire", IE_Pressed, this, &ACourseworkCodeCharacter::ActivateFuryFire);

	// Bind Curveball flash events

	// Right
	PlayerInputComponent->BindAction("Curveball_Right", IE_Pressed, this, &ACourseworkCodeCharacter::CurveballFlashRight);

	// Left
	PlayerInputComponent->BindAction("Curveball_Left", IE_Pressed, this, &ACourseworkCodeCharacter::CurveballFlashLeft);

	// Bind Sage Wall events

	// Placing
	PlayerInputComponent->BindAction("Place_SageWall", IE_Pressed, this, &ACourseworkCodeCharacter::PlaceSageWall);

	// Spawning
	PlayerInputComponent->BindAction("Spawn_SageWall", IE_Pressed, this, &ACourseworkCodeCharacter::SpawnSageWall);

	// Rotating
	PlayerInputComponent->BindAction("Rotate_SageWall", IE_Pressed, this, &ACourseworkCodeCharacter::RotateSageWall);

	// Stop Rotating
	PlayerInputComponent->BindAction("Rotate_SageWall", IE_Released, this, &ACourseworkCodeCharacter::StopRotatingSageWall);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACourseworkCodeCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACourseworkCodeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACourseworkCodeCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	
	
	
	PlayerInputComponent->BindAxis("TurnRate", this, &ACourseworkCodeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACourseworkCodeCharacter::LookUpAtRate);
}


// Sets up the range of the flashbang and checks if the player is within range of the flashbang once it has been thrown
// outputs a bool if in range and also the location of where to flash
bool ACourseworkCodeCharacter::ifInFlashbangRange(const float& distance, const FVector& facingAngle, FVector& facingAngleOutput)
{
	float normalizeDistance;
	bool outOfRange;

	// normalizing distance using preset values for a consistent range

	normalizeDistance = UKismetMathLibrary::NormalizeToRange(distance, 20.f, 100.0f);

	// invert values and make smaller to control 
	// the flash amount at the end of the overall flash event

	normalizeDistance /= 10.0f;
	normalizeDistance *= -1.0f;

	flashAmount = normalizeDistance;

	// check if player is out of range 
	
	// if true, player will not be flashed
	if (distance >= 2000.0f)
	{
		outOfRange = true;
	}

	// if false, player will be flashed based on angle from flash function
	else
	{
		outOfRange = false;
	}

	// set location of flash to output from function
	facingAngleOutput = facingAngle;

	return outOfRange;

}

// gathers the players camera location and look at rotation
// based on that value determine the angle from which the player is looking at the flash
// then return if player should be flashed

bool ACourseworkCodeCharacter::AngleFromFlash(const FVector& flashLocation)
{
	// Gatherering camera position and rotation from player
	FVector camLocation = FirstPersonCameraComponent->GetComponentLocation();
	FRotator camRotation = FirstPersonCameraComponent->GetComponentRotation();

	float angleFacingFromFlash;
	bool isFacingFlash;

	// finds where the player camera is looking at against the flash location
	angleFacingFromFlash = UKismetMathLibrary::FindLookAtRotation(camLocation, flashLocation).Yaw;


	// calculates final angle from flash 
	angleFacingFromFlash -= camRotation.Yaw;

	// if outside of a 90 degree range left or right of the player camera then only half flash the player
	if (angleFacingFromFlash >= 90.0f || angleFacingFromFlash <= -90.0f)
	{
		isFacingFlash = true;
	}

	// if within the 90 degree value either side, full flash the player
	else
	{
		isFacingFlash = false;
	}

	return isFacingFlash;
}


// spawns and throws a Curveball to the right of the player
void ACourseworkCodeCharacter::CurveballFlashRight()
{
	// try to throw the Curveball
	if (CurveballClass != NULL)
	{
		// if able to throw, get world
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// retrieve spawn location and rotation from the Curveball Spawn Scene component
			const FRotator SpawnRotation = Curveball_SpawnLocation->GetComponentRotation();
			const FVector SpawnLocation = Curveball_SpawnLocation->GetComponentLocation();

			// Spawn collision parameter handle
			FActorSpawnParameters CurveballSpawnParams;
			CurveballSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// Spawn the Curveball actor based on retrieved location and rotation variables
			World->SpawnActor<ACurveball>(CurveballClass, SpawnLocation, SpawnRotation, CurveballSpawnParams);
		}
	}

	else
	{
		// if the curveball can't be thrown, output a debug message to screen telling the player
		GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Cyan,FString("Function has not been called correctly"));
	}
}

// spawns and throws a Curveball to the left of the player
void ACourseworkCodeCharacter::CurveballFlashLeft()
{

	// try to throw the Curveball
	if (CurveballClass != NULL)
	{
		// if able to throw, get world
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// retrieve spawn location and rotation from the Curveball Spawn Scene component

			const FRotator SpawnRotation = Curveball_SpawnLocation->GetComponentRotation();
			const FVector SpawnLocation = Curveball_SpawnLocation->GetComponentLocation();

			// as this Curveball is going left, a transform was created to set the Y-Scale to -1
			// this allows for a quick and easy change from throwing it right

			const FTransform SpawnTransform(SpawnRotation,SpawnLocation, FVector(1.0f, -1.0f, 1.0f));

			// Spawn collision parameter handle
			FActorSpawnParameters CurveballSpawnParams;
			CurveballSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// Spawn the Curveball actor based on retrieved transform variables
			World->SpawnActor<ACurveball>(CurveballClass, SpawnTransform, CurveballSpawnParams);
		}
	}

	else
	{
		// if the curveball can't be thrown, output a debug message to screen telling the player
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString("Function has not been called correctly"));
	}

}


// spawns a Sage Wall which will determine the placement of the Sage Cubes that will make up the Sage Wall
void ACourseworkCodeCharacter::PlaceSageWall()
{
	// try to spawn the Sage Wall
	if (SageWallClass != NULL)
	{
		// if able to spawn, get world
		UWorld* const World = GetWorld();
		if (World != NULL)
		{

			// only spawn if player is actively trying to place a wall within the correct parameters
			if (!isPlacingWall)
			{
				isPlacingWall = true;

				// sets the spawn location of the Sage Wall based on a trace from the 
				// Sage Wall Spawn Scene component

				const FRotator SpawnRotation = SageWall_SpawnLocation->GetComponentRotation();
				const FVector SpawnLocation = SageWall_SpawnLocation->GetComponentLocation();

				// Spawn collision parameter handle
				FActorSpawnParameters SageWallSpawnParams;
				SageWallSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				// Spawn the Sage Wall actor based on retrieved transform variables
				World->SpawnActor<ASageWall>(SageWallClass, SpawnLocation, FRotator(0.0f, SpawnRotation.Yaw, 0.0f), SageWallSpawnParams);

			}
		}
	}

	else
	{
		// if the Sage Wall can't be placed, output a debug message to screen telling the player
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString("Function has not been called correctly"));
	}
}


// gets called when player presses the input button
// while placing the Sage Wall
// stops placing the wall and sets the wall to the last known
// placing location
void ACourseworkCodeCharacter::SpawnSageWall()
{
	isPlacingWall = false;
}

// gets called when player presses the input button
// while placing the Sage Wall
// allows the player to rotate the wall on the spot
// while still trying to place the wall
void ACourseworkCodeCharacter::RotateSageWall()
{

	if (!isRotatingWall)
	{
		isRotatingWall = true;
		
	}
	
}

// gets called if player lets go of the button that
// allows them to rotate the wall
void ACourseworkCodeCharacter::StopRotatingSageWall()
{
	isRotatingWall = false;
}

// controls the spawning of the Fury Shot projectile
void ACourseworkCodeCharacter::OnFire()
{

	// only shoot if player is holding down or pressing the shoot button

	if (isShooting)
	{

		// made false to allow for a controlled loop
		// of automatic fire and changing fire rate
		isShooting = false;

		// checks if the Fury Shot ability is active

		// if it is not, spawn projectiles at normal fire rate when auto
		if (!isFuryActivated)
		{
			// try and fire a projectile
			if (FuryShotClass != NULL)
			{
				// if able to spawn, get world
				UWorld* const World = GetWorld();
				if (World != NULL)
				{


					const FRotator SpawnRotation = GetControlRotation();
					// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

					// spawn the Fury Shot projectile at the muzzle
					World->SpawnActor<AFuryShot>(FuryShotClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

				}
			}

			else
			{
				// if the projectile doesnt spawn, output a debug message to screen telling the player
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString("Function has not been called correctly"));
			}

			// try and play the sound if specified
			if (FireSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation(), 0.1f);
			}

			// try and play a firing animation if specified
			if (FireAnimation != NULL)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}

			// sets the fire rate of the auto fire based on a timer using the standard auto fire rate
			GetWorld()->GetTimerManager().SetTimer(FireHandle, this, &ACourseworkCodeCharacter::setFiring, autoFireRate, false);

		}


		// if Fury Shot ability is active, spawn projectiles at increased fire rate when auto
		else
		{
			// try and fire a projectile
			if (FuryShotClass != NULL)
			{
				// if able to spawn, get world
				UWorld* const World = GetWorld();
				if (World != NULL)
				{

					const FRotator SpawnRotation = GetControlRotation();
					// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

					// spawn the Fury Shot projectile at the muzzle
					World->SpawnActor<AFuryShot>(FuryShotClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

				}
			}

			else
			{
				// if the projectile doesnt spawn, output a debug message to screen telling the player
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString("Function has not been called correctly"));
			}

			// try and play the sound if specified
			if (FireSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation(), 0.1f);
			}

			// try and play a firing animation if specified
			if (FireAnimation != NULL)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}

			// sets the fire rate of the auto fire based on a timer using the fury shot increased fire rate
			GetWorld()->GetTimerManager().SetTimer(FireHandle, this, &ACourseworkCodeCharacter::setFiring, furyFireRate, false);
		}

		
	}

	
}

// when input from player received, activate the Fury Fire ability
// increase the fire of the weapon for a limited amount of time
void ACourseworkCodeCharacter::ActivateFuryFire()
{
	isFuryActivated = true;

	// carries out the deactivate function once the ability has been active for its set amount of time
	GetWorld()->GetTimerManager().SetTimer(FuryFireTimerHandle, this, &ACourseworkCodeCharacter::DeactivateFuryFire, furyFireAbilityLength,false);
	
	//gunSmokeParticle->SetActive(true);
}

// deactivates the Fury Fire ability after a set amount of time
void ACourseworkCodeCharacter::DeactivateFuryFire()
{

	isFuryActivated = false;

	// clears the time handle from the Fury Fire ability
	// allows it to be reused again 

	GetWorldTimerManager().ClearTimer(FuryFireTimerHandle);

	//gunSmokeParticle->SetActive(false);
}


// allows for the weapon to shoot in full auto
// by looping the firing function using a timer

void ACourseworkCodeCharacter::OnFireAuto()
{
	// set to allow for shooting
	isShooting = true;
	
	// loops the OnFire() function with a very low value to act as a constant fire rate
	GetWorld()->GetTimerManager().SetTimer(AutoFireTimerHandle, this, &ACourseworkCodeCharacter::OnFire, 0.001f, true);

}

// clears the timer handles used inside the OnFire() function
// allows for the next bullet to be shot
void ACourseworkCodeCharacter::setFiring()
{
	isShooting = true;

	GetWorldTimerManager().ClearTimer(FireHandle);
}

// stops the weapon from firing once the player stops pressing the button to fire
// clears the timer handle used inside the OnFireAuto() function 
void ACourseworkCodeCharacter::StopFiring()
{
	isShooting = false;

	GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);
}

void ACourseworkCodeCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACourseworkCodeCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ACourseworkCodeCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ACourseworkCodeCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ACourseworkCodeCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACourseworkCodeCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACourseworkCodeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	
	
}

void ACourseworkCodeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ACourseworkCodeCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ACourseworkCodeCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ACourseworkCodeCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ACourseworkCodeCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}
