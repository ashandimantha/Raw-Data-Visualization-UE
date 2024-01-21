// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MaterialData.h"
#include <Components/TimelineComponent.h>
#include "PlayerActor.generated.h"

UCLASS()
class TASK_API APlayerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool DoOnce = true;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region ImportingData
	// Player ID from PositionalData
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Info")
    int32 PlayerID;

	// Initialization
	UPROPERTY()
	UStaticMeshComponent* SphereMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positional Data")
	UCurveVector* UniqueCurve = nullptr;
#pragma endregion

	//UPROPERTY()
	FTimeline Timeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Properties")
	FMaterialData MaterialData;

	// Update Actor Locations according the time
	void UpdateLocation();

	UFUNCTION()
	void SetActorLocationFromTimeline(FVector NewLocation);


};
