// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "SageWall.generated.h"

UCLASS(config=game)
class COURSEWORKCODE_API ASageWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASageWall();


	/** Sage Wall class to spawn */
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ASageCube> SageCubeClass;

	// spawns a sage cube based on entered values for location and rotation
	void SpawnSageCube(const FVector FinalLoc, const FRotator FinalRot, FVector RotateVal);

protected:

	/** sets the static mesh component for the sage wall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* wallStaticMesh;

	/** sets the scene component for the sage wall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* wallSceneComp;

	/** bool check to see if player has finished placing the sage wall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isWallPlaced;

	/** set vector value for the maximum range 
	of the line trace the sage wall can spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector spawnDistanceFromPlayer;

	/** changes the rotation of the wall on the spot based on mouse movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float changeInRotation;

	/** initial rotation of wall so it is facing flat on */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float defaultRotation;

	/** receives the mouse movement value */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float turnAxisVal;

	/** saves the final location of wall to spawn the cubes from */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector finalLocation;

	/** saves the final rotation of wall to spawn the cubes from */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FRotator finalRotation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// function that takes in the input of the mouse X-axis movement
	// and saves the input to another float variable
	void RotateWall(float val);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
