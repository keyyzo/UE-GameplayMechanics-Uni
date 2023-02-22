// Fill out your copyright notice in the Description page of Project Settings.


#include "SageCube.h"
#include "Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
ASageCube::ASageCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// sets the scene component as root component for the actor to use within the world and blueprints
	cubeSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Cube Scene"));
	RootComponent = cubeSceneComp;

	// set up the static mesh component for the cube
	cubeStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube Static Mesh"));
	cubeStaticMesh->SetupAttachment(GetRootComponent());

	// loads in the static mesh whhich will be used for the static mesh component of the cube
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubeVisualAsset(TEXT("/Game/StarterContent/Architecture/Wall_400x400"));

	// if it loads successfully
	if (cubeVisualAsset.Succeeded())
	{
		// set the static mesh to the component
		// change the scale value for the appropriate size and shape of cube
		cubeStaticMesh->SetStaticMesh(cubeVisualAsset.Object);
		cubeStaticMesh->SetWorldScale3D(FVector(0.5f,5.0f,0.55f));
		
	}

	// set initial health value
	cubeHealth = 500;
}


// function is called once the cube is spawned in
// raises the cube from the ground by scaling the Z-Axis value
void ASageCube::RaiseCubeFromGround(const float & changeZValue)
{
	// gets overall scale values
	FVector currentScale = cubeStaticMesh->GetRelativeScale3D();

	
	// sets new scale values with the z-value coming from a timeline within the blueprints
	cubeStaticMesh->SetRelativeScale3D(FVector(currentScale.X, currentScale.Y, changeZValue));

}

// gets the health of the cube
int ASageCube::getCubeHealth()
{
	return cubeHealth;
}

// sets the health of the cube
void ASageCube::setCubeHealth(int val)
{
	cubeHealth = val;
}

// Called when the game starts or when spawned
void ASageCube::BeginPlay()
{
	Super::BeginPlay();
	


}

// Called every frame
void ASageCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// destroy the cube if its health reaches below 0
	if (cubeHealth <= 0)
	{
		Destroy();
	}

}

