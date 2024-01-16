// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerActor.h"
#include "GameFramework/GameModeBase.h"
#include "ImportingJSONData.h"
#include <vector>
#include <iostream>
#include "TaskGameModeBase.generated.h"

using namespace std;

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

	void ExampleUsage();

	void AsyncLoadJsonData();

	void ProcessPositionalData(const TSharedPtr<FJsonValue>& PositionalData);

	UPROPERTY()
	UCurveBase* CurveBase;

	// Map to contain Time as Key and there ID with Transforms
	TMap< double, TArray< TPair< float, FVector> > > PlayerPosMap;

	// Player ID as Key and their time with position
	TMap< float, TArray< TPair< double, FVector> > > PlayerPosMapTwo;

	// Array with pointers to hold dynamic object references
	TArray<APlayerActor*> PlayerActors;

	FCriticalSection* PlayerPosMapLock;

#pragma region TiffImport
	UFUNCTION(BlueprintCallable)
	UTexture2D* CreateTextureFromChannelData(int32 Width, int32 Height, const TArray<uint8>& ChannelData);

    UFUNCTION(BlueprintCallable)
    TArray<uint8> ChannelData();

#pragma endregion 
};


