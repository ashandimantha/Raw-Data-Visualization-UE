// Fill out your copyright notice in the Description page of Project Settings.

#include "JsonUtilities/Public/JsonObjectConverter.h"

class FImportingJSONData : public FRunnable
{
private:
    FString JsonFilePath;
    const TArray<TSharedPtr<FJsonValue, ESPMode::ThreadSafe>>* PositionalDataArray;

    // Map to contain Time as Key and there ID with Transforms
    TMap< double, TArray< TPair< float, FVector> > > PlayerPosMap;

public:
    FImportingJSONData(const FString& InJsonFilePath)
        : JsonFilePath(InJsonFilePath) {}

    virtual uint32 Run() override
    {
        FString JsonString;
        if (FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
        {
            TSharedPtr<FJsonObject> JsonObject;
            const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);

            if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
            {
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
                            PairArray.Reserve(PositionsArray->Num());

                            for (const auto& Position : *PositionsArray)
                            {
                                const TSharedPtr<FJsonObject> PositionObject = Position->AsObject();
                                const float ID = PositionObject->GetNumberField("id");

                                if (ID != 0)
                                {
                                    FVector NewPosition;
                                    NewPosition.X = PositionObject->GetNumberField("x");
                                    NewPosition.Y = PositionObject->GetNumberField("y");
                                    NewPosition.Z = 10.0f;

                                    PairArray.Emplace(ID, NewPosition);
                                }
                            }
                            PlayerPosMap.FindOrAdd(Time, MoveTemp(PairArray));
                        }
                    }
                }
            }
            else
            {
                // Handle JSON deserialization failure.
            }
        }
        else
        {
            // Handle file loading failure.
        }

        return 0;
    }

    // ... Implement other necessary methods, e.g., Initialize, Stop, Exit, etc.

    auto GetPositionalDataArray()
    {
        return PlayerPosMap;
    }
};

// Example of using the FJsonReaderWorker:

//void ExampleUsage()
//{
//    FString JsonFilePath = FPaths::ProjectContentDir() + TEXT("pos_test.json");
//
//    // Create an instance of FJsonReaderWorker
//    FImportingJSONData* JsonReaderWorker = new FImportingJSONData(JsonFilePath);
//
//    // Create a thread for the FJsonReaderWorker
//    FRunnableThread* Thread = FRunnableThread::Create(JsonReaderWorker, TEXT("JsonReaderThread"));
//
//    // Wait for the thread to finish
//    Thread->WaitForCompletion();
//
//    // Access the results after the thread has finished
//    auto PositionalDataArray = JsonReaderWorker->GetPositionalDataArray();
//
//    // ... Further processing of PositionalDataArray
//}

