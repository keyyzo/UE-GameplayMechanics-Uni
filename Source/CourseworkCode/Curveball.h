// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "CourseworkCodeCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Curveball.generated.h"

//class ACourseworkCodeCharacter;

UCLASS(config=game)
class COURSEWORKCODE_API ACurveball : public AActor
{
	GENERATED_BODY()

protected:


	/** sets the static mesh component for the curveball */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* curveballStaticMesh;

	/** sets the spline component for the curveball */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USplineComponent* curveballSpline;

	/** sets scene component for the curveball */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* curveballSceneComp;

	/** controls the offset of the curveball when thrown */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector	curveballEndOffset;


public:	
	// Sets default values for this actor's properties
	ACurveball();

	// function that sets the spline points of the curveball 
	// which the curveball will travel through before flashing
	UFUNCTION()
		void UpdateSpline(const FVector& CurveStart, const FVector& CurveEnd);

	// sets the location of the curveball along the spline based
	// on a timeline
	UFUNCTION(BlueprintCallable)
		void SplineLocationProgress(float timeVal, USplineComponent* splineComp, UStaticMeshComponent* staticMeshComp);

	// determines if the curveball can see the player camera
	// and whether it will flash the player or not
	UFUNCTION(BlueprintCallable)
		void curveballFlash();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
