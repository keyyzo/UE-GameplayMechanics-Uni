// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "FuryShot.generated.h"

UCLASS()
class COURSEWORKCODE_API AFuryShot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFuryShot();


	// controls what happens when the projectile collides with other actors
	// if it hits a sage cube actor the sage cube would take damage
	UFUNCTION()
		void OnSageCubeBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:


	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly,Category = Projectile)
	USphereComponent* furySphereComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* furyProjectileMovement;

	/** sets the static mesh component for the fury shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* furyStaticMesh;

	/** sets the particle to the actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* furyParticle;

	/** damage variable that controls how much damage the projectile does */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int damage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
