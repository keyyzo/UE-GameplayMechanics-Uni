// Fill out your copyright notice in the Description page of Project Settings.


#include "FuryShot.h"
#include "CourseworkCodeCharacter.h"
#include "SageCube.h"
#include "Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
AFuryShot::AFuryShot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

	// Use a sphere as a simple collision representation
	furySphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("FurySphereComp"));
	furySphereComp->InitSphereRadius(26.0f);
	furySphereComp->BodyInstance.SetCollisionProfileName("Projectile");
	furySphereComp->OnComponentHit.AddDynamic(this,&AFuryShot::OnSageCubeBeginOverlap);

	// Players can't walk on it
	furySphereComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	furySphereComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = furySphereComp;


	// Use a ProjectileMovementComponent to control fury shots movement
	furyProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("FuryProjectileComp"));
	furyProjectileMovement->UpdatedComponent = furySphereComp;

	// uses a low gravity value and high velocity value
	// to mimick real bullet movement
	furyProjectileMovement->ProjectileGravityScale = 0.2f;
	furyProjectileMovement->Velocity = FVector(5000.0f, 0.0f, 0.0f);


	// set up the static mesh component for the fury shot
	furyStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FuryStaticMesh"));
	furyStaticMesh->SetupAttachment(GetRootComponent());

	// loads in the static mesh which will be used for the static mesh component of the fury shot
	static ConstructorHelpers::FObjectFinder<UStaticMesh> furyVisualAsset(TEXT("/Game/StarterContent/Props/MaterialSphere"));

	// if it loads successfully
	if (furyVisualAsset.Succeeded())
	{
		// set the static mesh to the component
		// change the scale value for the appropriate size and shape of the fury shot
		furyStaticMesh->SetStaticMesh(furyVisualAsset.Object);
		furyStaticMesh->SetWorldScale3D(FVector(0.5f));

	}

	furyParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FlameParticles"));
	furyParticle->SetupAttachment(RootComponent);
	furyParticle->bAutoActivate = false;
	furyParticle->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"));

	if (ParticleAsset.Succeeded())
	{
		furyParticle->SetTemplate(ParticleAsset.Object);
	}

	// Die after 5 seconds by default
	InitialLifeSpan = 5.0f;


	// set initial damage value
	damage = 50;
}


// checks if the projectile is colliding with a sage cube
// if it collides with one, take damage away from the sage cube
// if not, destroy the projectile
void AFuryShot::OnSageCubeBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	// casts to the sage cube class in order to access it's variables and functions
	class ASageCube* sageCube = Cast<ASageCube>(OtherActor);

	// checks if collided actor is not null and if it is a sage cube
	// if it is a sage cube and not null, deal damage to cube
	if (OtherActor != NULL && OtherActor == sageCube)
	{
		int changeCubeHealth;
		
		// calculates new health value
		changeCubeHealth = sageCube->getCubeHealth() - damage;

		// sets new health value for cube
		sageCube->setCubeHealth(changeCubeHealth);

		// destroy the projectile
		Destroy();
	}

	// if it isnt a sage cube or the actor is null
	else
	{
		// destroy the projectile
		Destroy();
	}
	

}

// Called when the game starts or when spawned
void AFuryShot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFuryShot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// casts to the player character in order to access the character variables and functions
	class ACourseworkCodeCharacter* playerPawn = Cast<ACourseworkCodeCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	// check if fury shot ability is active
	// if it is
	if (playerPawn->getIsFuryActivated())
	{
		// set damage to double of standard damange
		damage = 100;
		furyParticle->SetActive(true);
	}

	// if it isn't
	else
	{
		// set damage back to standard damage
		damage = 50;
		furyParticle->SetActive(false);
	}

}

