// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerActor.h"
#include "GameFramework/GameModeBase.h"
#include "TaskGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TASK_API ATaskGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay();


private:



public:

	UPROPERTY()
	APlayerActor* SpawnedActor;

	void LoadJsonData();

	void SpawnActorsWithID();

	void SetCurveData();

	UPROPERTY()
	UCurveBase* CurveBase;

	// Map to contain Time as Key and there ID with Transforms
	TMap<double, TArray< TPair< float, FVector> > > PlayerPosMap;

	//void UpdateActorPositions(double CurrentTime);

	//const FVector* FindNextPosition(const TMap<double, TArray<TPair<float, FVector>>>& tMap, double StartTime, float ActorID, double CurrentTime);

	TArray<APlayerActor*> PlayerActors;


	
};
