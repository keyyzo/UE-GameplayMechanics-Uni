// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CourseworkCodeCharacter.generated.h"

//class ACurveball;
class UInputComponent;

UCLASS(config=Game)
class ACourseworkCodeCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** location of where curveballs will spawn from player */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* Curveball_SpawnLocation;

	/** location of where sage wall will spawn from player */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* SageWall_SpawnLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

public:
	ACourseworkCodeCharacter();

	// Setters and getters

	// Check if player is placing the Sage Wall

	UFUNCTION()
		bool getIsPlacingWall();

	// Set if player is placing the Sage Wall

	UFUNCTION()
		void setIsPlacingWall(bool val);

	// Check if player is rotating the Sage Wall

	UFUNCTION()
		bool getIsRotatingWall();

	// Set if player is rotating the Sage Wall

	UFUNCTION()
		void setisRotatingWall(bool val);

	// Check if player has Fury Shot ability active

	UFUNCTION()
		bool getIsFuryActivated();

	// Set if player has Fury Shot ability active

	UFUNCTION()
		void setIsFuryActivated(bool val);

	/*UFUNCTION()
		float getFireRate();

	UFUNCTION()
		void setFireRate(float val);*/

protected:
	virtual void BeginPlay();

	// Variables used within the classes and blueprints if necessary


	// Curveball flash amount value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float flashAmount;

	// Standard auto fire rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float autoFireRate;

	// Rate of fire for auto shot when Fury Shot ability is active
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float furyFireRate;

	// Length of time Fury Shot ability lasts
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float furyFireAbilityLength;

	// bool to check if player is placing Sage Wall
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isPlacingWall;

	// bool to check if player is rotating Sage Wall 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isRotatingWall;

	// bool to check if Fury Shot is active
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isFuryActivated;

	// bool to check if player is shooting
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isShooting;

	
	// Timer handles to control rate of fire and fury shot ability length
	FTimerHandle AutoFireTimerHandle;
	FTimerHandle FuryFireTimerHandle;
	FTimerHandle FireHandle;


public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Curveball class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACurveball> CurveballClass;

	/** Sage Wall class to spawn */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASageWall> SageWallClass;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ACourseworkCodeProjectile> ProjectileClass;

	/** FuryShot class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AFuryShot> FuryShotClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

	/** Event called within the character blueprint to control flashbang */
	UFUNCTION(BlueprintImplementableEvent)
		void ifInFlashbangRangeEvent(const float& distance, const FVector& facingAngle);

	/** Function that controls the range of the flashbang and determines if player is in range */
	UFUNCTION(BlueprintCallable)
		bool ifInFlashbangRange(const float& distance, const FVector& facingAngle, FVector& facingAngleOutput);

	/** Function that controls how much of a flash the player gets based on the angle from looking at the flash*/
	UFUNCTION(BlueprintCallable)
	bool AngleFromFlash(const FVector& flashLocation);

protected:
	
	/** Throws a Curveball flashbang right */
	void CurveballFlashRight();

	/** Throws a Curveball flashbang left */
	void CurveballFlashLeft();

	/** Activate placing a Sage Wall */
	void PlaceSageWall();

	/** Spawns a Sage Wall */
	void SpawnSageWall();

	/** Activate rotating a Sage Wall */
	void RotateSageWall();

	/** Deactivate rotating a Sage Wall */
	void StopRotatingSageWall();

	/** Fires a projectile. */
	void OnFire();

	/** Activates Fury Fire ability */
	void ActivateFuryFire();

	/** Deactivate Fury Fire ability */
	void DeactivateFuryFire();

	/** Begins firing the gun in full auto */
	void OnFireAuto();

	/** Sets player to firing */
	void setFiring();

	/** Sets player to stop firing */
	void StopFiring();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

