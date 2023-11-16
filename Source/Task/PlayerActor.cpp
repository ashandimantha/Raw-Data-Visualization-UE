// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerActor.h"

// Sets default values
APlayerActor::APlayerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a static mesh component
	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMeshComponent");
	SphereMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SphereMesh;
	RootComponent->SetMobility(EComponentMobility::Movable);
	if (SphereMesh)
	{
		// Set the scale of the sphere
		SphereMesh->SetWorldScale3D(FVector(.05f, .05f, .05f));

		static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
		if (SphereMeshAsset.Succeeded())
		{
			SphereMesh->SetStaticMesh(SphereMeshAsset.Object);

			// Load and set the material
			static ConstructorHelpers::FObjectFinder<UMaterial> DefaultMaterial(TEXT("/Game/Materials/ActorMat"));
			if (DefaultMaterial.Succeeded())
			{
				SphereMesh->SetMaterial(0, DefaultMaterial.Object);
			}
			else
			{
				// Error massage
				UE_LOG(LogTemp, Error, TEXT("Failed to load material: ActorMat"));
			}
		}
	}

}

// Called when the game starts or when spawned
void APlayerActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Timeline.TickTimeline(DeltaTime);

	if (UniqueCurve && DoOnce) {
		UpdateLocation();
		DoOnce = false;
	}
}

void APlayerActor::UpdateLocation()
{
    float TimelineDuration = 30.f;
    if (UniqueCurve)
    {
		// Update locations from a Timeline Function using premade curves
		Timeline.SetTimelineLength(TimelineDuration);

        FOnTimelineVector TimelineProgress;
        TimelineProgress.BindUFunction(this, FName("SetActorLocationFromTimeline"));
		Timeline.AddInterpVector(UniqueCurve, TimelineProgress);
		Timeline.SetLooping(true);

        // Play the time line
		Timeline.PlayFromStart();
		//UE_LOG(LogTemp, Warning, TEXT("UpdateLocation Called"));

    }

}

void APlayerActor::SetActorLocationFromTimeline(FVector NewLocation)
{
    // Calculate the rotation based on your logic (e.g., using FRotator::ZeroRotator for simplicity)
    FRotator NewRotation = FRotator::ZeroRotator;

	//UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), NewLocation.X, NewLocation.Y, NewLocation.Z)

	SetActorLocation(NewLocation, true, 0, ETeleportType::None);
}
