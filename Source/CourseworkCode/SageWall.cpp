// Fill out your copyright notice in the Description page of Project Settings.


#include "SageWall.h"
#include "CourseworkCodeCharacter.h"
#include "SageCube.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
ASageWall::ASageWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// setting scene component as root component
	wallSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Wall Scene"));
	RootComponent = wallSceneComp;

	// set up the static mesh component for the sage wall
	wallStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall Static Mesh"));
	wallStaticMesh->SetupAttachment(GetRootComponent());

	// loads in the static mesh which will be used for the static mesh component of the sage wall
	static ConstructorHelpers::FObjectFinder<UStaticMesh> wallVisualAsset(TEXT("/Game/StarterContent/Architecture/Wall_400x400"));

	static ConstructorHelpers::FObjectFinder<UMaterial> wallMaterial(TEXT("/Game/FirstPersonCPP/Blueprints/SageWallMaterial"));

	// if it loads successfully
	if (wallVisualAsset.Succeeded())
	{
		// set the static mesh to the component
		// change the scale value for the appropriate size and shape of the sage wall
		// disable all collisions for the wall as the wall is only a placement for the sage cubes
		wallStaticMesh->SetStaticMesh(wallVisualAsset.Object);
		wallStaticMesh->SetWorldScale3D(FVector(1.5f, 5.0f, 0.55f));
		wallStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		wallStaticMesh->SetMaterial(0, wallMaterial.Object);
		
	}

	// intialising sage wall variables

	isWallPlaced = false;
	spawnDistanceFromPlayer = FVector(1000.0f, 0.0f, -100.0f);
	changeInRotation = 0.0f;
	defaultRotation = 90.0f;
	turnAxisVal = 0.0f;
}


// controls the spawning of the sage cube
void ASageWall::SpawnSageCube(const FVector FinalLoc, const FRotator FinalRot, FVector RotateVal)
{
	// try and spawn a sage cube
	if(SageCubeClass != NULL)
	{
		// if able to spawn, get world
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// calculate the final cube location and rotation
			// taking in values from the final sage wall location and rotation
			// rotating the cube with an entered vector offset
			// so that the cube may be positioned properly
			// and opens up the possibility of spawning more than one cube from the wall
			// with several function calls

			FVector finalCubeLoc;
			FVector rotateCube = FinalRot.RotateVector(RotateVal);
			finalCubeLoc = FinalLoc + rotateCube;

			//Set Spawn Collision Handling Override
			FActorSpawnParameters SageWallSpawnParams;
			SageWallSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// spawn the sage cube at the given location and rotation
			World->SpawnActor<ASageCube>(SageCubeClass, finalCubeLoc, FinalRot, SageWallSpawnParams);
		}
	}

	else
	{
		// if the sage wall doesnt spawn, output a debug message to screen telling the player
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString("Sage Cube Failed to Spawn"));
	}

}

// Called when the game starts or when spawned
void ASageWall::BeginPlay()
{
	Super::BeginPlay();

}


// takes in the mouse x-axis input and saves the value to a variable
// which is used for turning the wall on the spot
void ASageWall::RotateWall(float val)
{
	if (val != 0.0f)
	{
		// multiply the input variable by delta time in order to slow down the rotation
		// as the rotation was far too fast during gameplay

		turnAxisVal += val * UGameplayStatics::GetWorldDeltaSeconds(GetWorld()); //* GetWorld()->GetDeltaSeconds()
		
	}

}

// Called every frame
void ASageWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	// checks if sage wall has been placed
	// if it has not, move to next check

	if (!isWallPlaced)
	{
		// casts to the player character in order to access the character variables and functions
		class ACourseworkCodeCharacter* playerPawn = Cast<ACourseworkCodeCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

		// checks if player is currently placing the sage wall
		// if it is, carry out the placing
		if (playerPawn->getIsPlacingWall() == true)
		{

			// set start point and calculate end point for the line trace where the sage wall
			// could possibly spawn at

			FVector startPoint = playerPawn->GetFirstPersonCameraComponent()->GetComponentLocation();
			FRotator camRotator = playerPawn->GetFirstPersonCameraComponent()->GetComponentRotation();

			// rotate the offset from camera to calculate the correct rotation
			// of the maximum reach of the line trace
			FVector rotatedVector = camRotator.RotateVector(spawnDistanceFromPlayer);

			// calculates the end point from adding the rotated offset vector to the start point
			// then create a transform 
			FVector transformLoc = startPoint + rotatedVector;
			FTransform lineTraceTransform(camRotator,transformLoc,FVector(1.0f,1.0f,1.0f));

			FHitResult hit;

			//Set Spawn Collision Handling Override
			// ignore itself for collisions
			FCollisionQueryParams traceParams;
			traceParams.AddIgnoredActor(this);

			// begin the line trace
			bool bHit = GetWorld()->LineTraceSingleByChannel(hit, startPoint, lineTraceTransform.GetLocation(), ECC_Visibility, traceParams);

			// draw debug lines to help with making sure the line is drawing correctly
			DrawDebugLine(GetWorld(), startPoint, lineTraceTransform.GetLocation(), FColor::Red, false, 3.0f);

			// sets variable to the location which the line trace hits
			FVector hitLocation = hit.Location;

			// set variable to the normal value of an object on the Z-Axis
			// to control the wall spawning purely on the ground
			float normalCheck = hit.Normal.Z;

			// if the line trace does hit and the z-value normal is straight up
			if (bHit && normalCheck == 1.0f)
			{
				// set initial location of the wall before rotating and offsetting
				wallStaticMesh->SetWorldLocation(hitLocation);


				// checks if player is inputting a rotation for the wall on the spot
				// if it is rotating
				if (playerPawn->getIsRotatingWall() == true)
				{
					// enables input for this class using the player controller
					EnableInput(UGameplayStatics::GetPlayerController(GetWorld(),0));
					
					// disables the turn input axis temporarily so player doesn't rotate the camera on the x-axis
					// by binding the turn input with no function call
					InputComponent->BindAxis("Turn");

					// binds a new input axis temporarily using the mouse x-axis for input
					// to control the rotation of the wall using the mouse 
					InputComponent->BindAxis("SageWallTurn", this, &ASageWall::RotateWall);

					// inverts the rotation so that the rotation makes more sense when
					// rotating left or right

					changeInRotation = -1.0f * turnAxisVal;


					float newWallRotation;
					float camYawVal = playerPawn->GetFirstPersonCameraComponent()->GetComponentRotation().Yaw;

					// calculates the final rotation to set based on the original rotation
					// with the camera rotation and the input rotation
					newWallRotation = defaultRotation + camYawVal + changeInRotation;
					
					// sets rotation of the wall
					wallStaticMesh->SetWorldRotation(FRotator(0.0f, newWallRotation, 0.0f));
					
					// new set of variables that take in the current location
					// and rotation of the sage wall before setting the final location 
					// with the offset in order to place the
					// wall in the correct spot facing the player 
					// in the middle and not off to the side

					FVector nextWorldLoc;
					FVector currentWorldLoc = wallStaticMesh->GetComponentLocation();
					FRotator currentWorldRot = wallStaticMesh->GetComponentRotation();
					FVector centreRotatedVector = currentWorldRot.RotateVector(FVector(-300.0f, 0.0f, 0.0f));

					// calculates offset with rotation 
					nextWorldLoc = currentWorldLoc + centreRotatedVector;


					// sets the final position of the wall
					wallStaticMesh->SetWorldLocation(nextWorldLoc);

					// saves the final location and rotation of the wall
					// to these variables in order to help set the cubes correctly

					finalLocation = wallStaticMesh->GetComponentLocation();
					finalRotation = wallStaticMesh->GetComponentRotation();
					


				}

				else
				{
					// disables input for this class using the player controller
					DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));


					float newWallRotation;
					float camYawVal = playerPawn->GetFirstPersonCameraComponent()->GetComponentRotation().Yaw;

					// calculates the final rotation to set based on the original rotation
					// with the camera rotation and the input rotation
					newWallRotation = defaultRotation + camYawVal + changeInRotation;

					// sets rotation of the wall
					wallStaticMesh->SetWorldRotation(FRotator(0.0f, newWallRotation, 0.0f));

					// new set of variables that take in the current location
					// and rotation of the sage wall before setting the final location 
					// with the offset in order to place the
					// wall in the correct spot facing the player 
					// in the middle and not off to the side

					FVector nextWorldLoc;
					FVector currentWorldLoc = wallStaticMesh->GetComponentLocation();
					FRotator currentWorldRot = wallStaticMesh->GetComponentRotation();
					FVector centreRotatedVector = currentWorldRot.RotateVector(FVector(-300.0f, 0.0f, 0.0f));

					// calculates offset with rotation 
					nextWorldLoc = currentWorldLoc + centreRotatedVector;

					// sets the final position of the wall
					wallStaticMesh->SetWorldLocation(nextWorldLoc);

					// saves the final location and rotation of the wall
					// to these variables in order to help set the cubes correctly

					finalLocation = wallStaticMesh->GetComponentLocation();
					finalRotation = wallStaticMesh->GetComponentRotation();
				}
			}

			// if the line trace doesn't hit anything
			else
			{
				// destroy the wall
				// reset player from placing
				// set wall is placed

				Destroy();
				playerPawn->setIsPlacingWall(false);
				isWallPlaced = true;
			}

		}


		// if the wall has been placed 
		else
		{

			// sets wall as placed
			// disables input incase the player was still attempting to rotate
			// destroys the wall

			isWallPlaced = true;
			DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

			Destroy();

			// spawns three sage cubes
			// places the sage cubes in the correct spot based on an entered vector
			// to create a small gap between each of them

			SpawnSageCube(finalLocation,finalRotation,FVector(200.0f,0.0f,0.0f));
			SpawnSageCube(finalLocation, finalRotation, FVector(401.0f, 0.0f, 0.0f));
			SpawnSageCube(finalLocation, finalRotation, FVector(-1.0f, 0.0f, 0.0f));


		}


	}

}

