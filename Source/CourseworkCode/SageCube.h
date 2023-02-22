// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "SageCube.generated.h"

UCLASS()
class COURSEWORKCODE_API ASageCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASageCube();

	/** blueprint function that scales the Z-Value of the cube
	when spawned from the ground*/

	UFUNCTION(BlueprintCallable)
	void RaiseCubeFromGround(const float& changeZValue);

	// getter and setter 

	// get cube health
	UFUNCTION()
		int getCubeHealth();

	// set cube health
	UFUNCTION()
		void setCubeHealth(int val);

protected:

	/** sets static mesh component for the cube */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* cubeStaticMesh;

	/** sets scene component for the cube */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* cubeSceneComp;

	/** health integer value used to determine health of cube */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int cubeHealth;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
