// Fill out your copyright notice in the Description page of Project Settings.


#include "Curveball.h"
//#include "K2Node_DynamicCast.h"
#include "CourseworkCodeCharacter.h"
#include "Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"




// Sets default values
ACurveball::ACurveball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// setting scene component as root component
	curveballSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Curveball Scene"));
	RootComponent = curveballSceneComp;

	// set up the static mesh component for the curveball
	curveballStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Curveball Sphere"));
	curveballStaticMesh->SetupAttachment(GetRootComponent());

	// loads in the static mesh which will be used for the static mesh component of the curveball
	static ConstructorHelpers::FObjectFinder<UStaticMesh> curveballVisualAsset(TEXT("/Game/StarterContent/Props/MaterialSphere"));

	// if it loads successfully
	if (curveballVisualAsset.Succeeded())
	{
		// set the static mesh to the component
		// change the scale value for the appropriate size and shape of the curveball
		curveballStaticMesh->SetStaticMesh(curveballVisualAsset.Object);
		curveballStaticMesh->SetWorldScale3D(FVector(0.25f));
	}

	// set up the spline component for the curveball
	curveballSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Curveball Spline"));
	curveballSpline->SetupAttachment(GetRootComponent());

	// sets initial value of the offset the curveball will use
	curveballEndOffset = FVector(300.0f, 300.0f, 0.0f);

	
	
}


// creates the spline points that the curveball will travel through once
// the curveball is activated

void ACurveball::UpdateSpline(const FVector& CurveStart, const FVector& CurveEnd)
{
	TArray<FVector> splineArray;

	FVector curvePoint;

	// calculates the curving point of the curveball
	// uses a set value for consistent curving each use
	curvePoint = (CurveStart + CurveEnd / 2.0f) + FVector(100.0f,-150.0f,0.0f);
	
	// adds each point of the spline to an array
	splineArray.Add(CurveStart);
	splineArray.Add(curvePoint);
	splineArray.Add(CurveEnd);

	// sets the spline points to the spline component for use when curveball is called
	curveballSpline->SetSplinePoints(splineArray, ESplineCoordinateSpace::Type::Local, true);
}

// updates the position of the curveball along the spline based on the time value from a timeline
void ACurveball::SplineLocationProgress(float timeVal, USplineComponent* splineComp, UStaticMeshComponent* staticMeshComp)
{
	FVector newSplineLocation;

	// gets location along the spline from timeline
	newSplineLocation = splineComp->GetLocationAtTime(timeVal, ESplineCoordinateSpace::Type::Local, false);

	// sets the location based on the current time from the timeline
	staticMeshComp->SetRelativeLocation(newSplineLocation,false);
}


// checks if the curveball can see the player camera using a line trace
// if there is block between the trace, the player will not be flashed

void ACurveball::curveballFlash()
{

	// casts to the player character in order to access the character variables and functions

	class ACourseworkCodeCharacter* playerPawn = Cast<ACourseworkCodeCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	// set variables to be used within the line trace
	FVector startPoint = curveballStaticMesh->GetComponentLocation();
	FVector endPoint = playerPawn->GetFirstPersonCameraComponent()->GetComponentLocation();
	FHitResult hit;
	float distanceRange;

	FCollisionQueryParams traceParams;
	
	// ignore itself for collisions

	traceParams.AddIgnoredActor(this);

	// begin the line trace 
	bool bHit = GetWorld()->LineTraceSingleByChannel(hit, startPoint, endPoint, ECC_Visibility, traceParams);

	// draw debug lines to help with making sure the line is drawing correctly

	DrawDebugLine(GetWorld(), startPoint, endPoint, FColor::Red, false, 3.0f);

	// if there was a hit with the line trace
	// destroy the curveball and dont flash
	if (bHit)
	{
		Destroy();
	}

	// if there was no hit
	else
	{
		// determine distance from flash
		// determine angle from flash
		// destroy the curveball once the flash has went off

		distanceRange =	GetDistanceTo(playerPawn);
		playerPawn->ifInFlashbangRangeEvent(distanceRange, startPoint);
		Destroy();
	}

}


// Called when the game starts or when spawned
void ACurveball::BeginPlay()
{
	Super::BeginPlay();

	FVector curveballStart, curveballEnd;

	// casts to the player character in order to access the character variables and functions
	class ACourseworkCodeCharacter* playerPawn = Cast<ACourseworkCodeCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	// set start and end points of the curveball ability to be passed in to the spline
	curveballStart = playerPawn->GetFirstPersonCameraComponent()->GetForwardVector();

	curveballEnd = playerPawn->GetFirstPersonCameraComponent()->GetForwardVector() + curveballEndOffset;

	// calls the function that will create the spline path
	UpdateSpline(curveballStart, curveballEnd);
	
}

// Called every frame
void ACurveball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}



