// Copyright Epic Games, Inc. All Rights Reserved.


#include "TaskGameModeBase.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "EngineUtils.h"
#include "Curves/CurveVector.h"
#include <cmath>

void ATaskGameModeBase::BeginPlay()
{

    LoadJsonData();
    SpawnActorsWithID();
    SetCurveData();

}

// Read the JSON file
void ATaskGameModeBase::LoadJsonData()
{
    FString JsonFilePath = FPaths::ProjectContentDir() + TEXT("pos_test.json");
    FString JsonString;

    FFileHelper::LoadFileToString(JsonString, *JsonFilePath);

    TSharedPtr<FJsonObject> JsonObject;
    const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);

    // Deserialize the JSON string into a JSON object
    if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        const TArray<TSharedPtr<FJsonValue>>* PositionalDataArray;
        if (JsonObject->TryGetArrayField("positionalData", PositionalDataArray))
        {
            for (const auto& PositionalData : *PositionalDataArray)
            {
                double Time;
                const TArray<TSharedPtr<FJsonValue>>* PositionsArray;

                if (PositionalData->AsObject()->TryGetArrayField("positions", PositionsArray)
                    && PositionalData->AsObject()->TryGetNumberField("time", Time))
                {
                    TArray<TPair<float, FVector>> PairArray;
                    for (const auto& Position : *PositionsArray)
                    {
                        const TSharedPtr<FJsonObject> PositionObject = Position->AsObject();
                        if (PositionObject->GetNumberField("id") != 0)  // ID 0 is being ignored as it has duplicated entries in the JSON file
                        {
                            FVector NewPosition;
                            float ID = PositionObject->GetNumberField("id");

                            NewPosition.X = PositionObject->GetNumberField("x");
                            NewPosition.Y = PositionObject->GetNumberField("y");
                            NewPosition.Z = 10.0f;

                            PairArray.Push(TPair<float, FVector>(ID, NewPosition));
                        }
                    }
                    PlayerPosMap.FindOrAdd(Time, PairArray);
                }
            }
        }
    }
    else
    {
        // Failed
        UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON data."));
    }

}

// Spawn all IDs on Key value 0
void ATaskGameModeBase::SpawnActorsWithID()
{
    // Spawn player actors based on PlayerPositions array
    PlayerActors.Empty();
    for (const auto& Entry : PlayerPosMap.FindOrAdd(0))
    {
		FVector position = Entry.Value;
		float ID = Entry.Key;

		FTransform ActorTransform;
		ActorTransform.SetLocation(position);
		APlayerActor* MyPlayer = GetWorld()->SpawnActor<APlayerActor>(APlayerActor::StaticClass(), ActorTransform);
		MyPlayer->PlayerID = ID;
        MyPlayer->SetActorLabel(FString::Printf(TEXT("ID_%d"), FMath::FloorToInt(ID)));
		PlayerActors.Add(MyPlayer);
		MyPlayer->UniqueCurve = NewObject<UCurveVector>(GetTransientPackage(), UCurveVector::StaticClass());
    }
}

void ATaskGameModeBase::SetCurveData()
{
    //CurveBase->GetCurves()[0].CurveToEdit->UpdateOrAddKey(0.0f, 100.0f);
    //CurveBase->GetCurves()[1].CurveToEdit->UpdateOrAddKey(0.0f, 200.0f);
    //CurveBase->GetCurves()[2].CurveToEdit->UpdateOrAddKey(0.0f, 500.0f);

    int index = 0;
    for (const auto& id : PlayerPosMap)
    {
        auto time = id.Key;
        auto& PairsArray = id.Value;

        // Iterate over the PairsArray
        for (int PairIndex = 0; PairIndex < PairsArray.Num(); ++PairIndex)
        {
            auto& Pair = PairsArray[PairIndex];

            // UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), Pair.Value.X, Pair.Value.Y, Pair.Value.Z)
            // Noticed That tolarence needs to be 0 otherwise it does a normalized thing...........
            PlayerActors[PairIndex]->UniqueCurve->GetCurves()[0].CurveToEdit->UpdateOrAddKey(time, Pair.Value.X, false, 0);
            PlayerActors[PairIndex]->UniqueCurve->GetCurves()[1].CurveToEdit->UpdateOrAddKey(time, Pair.Value.Y, false, 0);
            PlayerActors[PairIndex]->UniqueCurve->GetCurves()[2].CurveToEdit->UpdateOrAddKey(time, Pair.Value.Z, false, 0);
        }
    }
}

