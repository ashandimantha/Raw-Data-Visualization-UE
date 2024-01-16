// Copyright Epic Games, Inc. All Rights Reserved.


#include "TaskGameModeBase.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "EngineUtils.h"
#include "Curves/CurveVector.h"
#include "Async/Async.h"
#include <cmath>
#include <future>

THIRD_PARTY_INCLUDES_START
#include "tiff.h"
#include "tiffio.h"
THIRD_PARTY_INCLUDES_END

#include "Engine/Texture2D.h"
#include "Misc/FileHelper.h"
#include "Serialization/BulkData.h"

using std::async;

void ATaskGameModeBase::BeginPlay()
{

    //LoadJsonData();
    //ExampleUsage();
    //AsyncLoadJsonData();

    SpawnActorsWithID();
    SetCurveData();

}

void ATaskGameModeBase::ExampleUsage()
{
    FString JsonFilePath = FPaths::ProjectContentDir() + TEXT("pos_test.json");

    // Create an instance of FJsonReaderWorker
    FImportingJSONData* JsonReaderWorker = new FImportingJSONData(JsonFilePath);

    // Create a thread for the FJsonReaderWorker
    FRunnableThread* Thread = FRunnableThread::Create(JsonReaderWorker, TEXT("JsonReaderThread"));

    // Wait for the thread to finish
    Thread->WaitForCompletion();

    // Access the results after the thread has finished
    PlayerPosMap = JsonReaderWorker->GetPositionalDataArray();

    // ... Further processing of PositionalDataArray
}


// Read the JSON file
void ATaskGameModeBase::LoadJsonData()
{
    FString JsonFilePath = FPaths::ProjectContentDir() + TEXT("pos_test.json");
    FString JsonString;
    FFileHelper::LoadFileToString(JsonString, *JsonFilePath);

    TSharedPtr<FJsonObject> JsonObject;
    const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);

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
        UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON data."));
    }
}

// Read the JSON Data in Parrallel
void ATaskGameModeBase::AsyncLoadJsonData()
{
    FString JsonFilePath = FPaths::ProjectContentDir() + TEXT("pos_test.json");
    FString JsonString;
    FFileHelper::LoadFileToString(JsonString, *JsonFilePath);

    TSharedPtr<FJsonObject> JsonObject;
    const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);

    if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        const TArray<TSharedPtr<FJsonValue>>* PositionalDataArray;
        if (JsonObject->TryGetArrayField("positionalData", PositionalDataArray))
        {
            // Create a thread-safe array to store the results
            TArray<TPair<float, TArray<TPair<float, FVector>>>> ThreadSafeResultArray;
            ThreadSafeResultArray.Reserve(PositionalDataArray->Num());
            // Use parallel_for to process each PositionalData in parallel
            ParallelFor(PositionalDataArray->Num(), [&](int32 Index)
                {
                    const auto& PositionalData = (*PositionalDataArray)[Index];

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

                        // Add the result to the thread-safe array
                        ThreadSafeResultArray[Index] = TPair<float, TArray<TPair<float, FVector>>>(Time, MoveTemp(PairArray));
                    }
                });

            // Copy the results from the thread-safe array to the PlayerPosMap
            for (auto& Result : ThreadSafeResultArray)
            {
                PlayerPosMap.FindOrAdd(Result.Key, MoveTemp(Result.Value));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON data."));
    }
}

void ATaskGameModeBase::ProcessPositionalData(const TSharedPtr<FJsonValue>& PositionalData)
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

        // Use lock to safely modify PlayerPosMap in a multithreaded environment
        FCriticalSection* PairArrayLock = new FCriticalSection;
        TGuardValue<FCriticalSection*> Guard(PlayerPosMapLock, PairArrayLock);

        PlayerPosMap.FindOrAdd(Time, MoveTemp(PairArray));
    }
}

UTexture2D* ATaskGameModeBase::CreateTextureFromChannelData(int32 Width, int32 Height, const TArray<uint8>& ChannelData)
{
    // Needs to check this format if its 8Bit or 
    UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);
    if (!NewTexture) {
        return nullptr;
    }

    // Lock the texture for writing
    void* TextureData = NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

    // Copy the channel data to the texture
    for (int32 y = 0; y < Height; y++)
    {
        for (int32 x = 0; x < Width; x++)
        {
            int32 CurrentPixelIndex = y * Width + x;
            uint8 CurrentPixelValue = ChannelData[CurrentPixelIndex];

            // Assuming grayscale data for this channel, replicate the value across all color channels
            FColor PixelColor(CurrentPixelValue, CurrentPixelValue, CurrentPixelValue, 255); // R, G, B, A

            // Write the pixel color to the texture data
            ((FColor*)TextureData)[CurrentPixelIndex] = PixelColor;
        }
    }

    // Unlock the texture
    NewTexture->PlatformData->Mips[0].BulkData.Unlock();

    // Update the texture resource
    NewTexture->UpdateResource();

    return NewTexture;
}

void ATaskGameModeBase::SpawnActorsWithID()
{
    //PlayerActors.Reserve(PlayerPosMap.Num());
    PlayerActors.Empty();
    for (const auto& Entry : PlayerPosMap.FindOrAdd(0))
    {
        const FVector position = Entry.Value;
        const float ID = Entry.Key;

        FTransform ActorTransform;
        ActorTransform.SetLocation(position);

        APlayerActor* MyPlayer = GetWorld()->SpawnActor<APlayerActor>(APlayerActor::StaticClass(), ActorTransform);
        MyPlayer->PlayerID = ID;
        MyPlayer->SetActorLabel(FString::Printf(TEXT("ID_%d"), FMath::FloorToInt(ID)));

        PlayerActors.Emplace(MyPlayer);
        MyPlayer->UniqueCurve = NewObject<UCurveVector>(GetTransientPackage(), UCurveVector::StaticClass());
    }
}

void ATaskGameModeBase::SetCurveData()
{
    for (const auto& id : PlayerPosMap)
    {
        const auto time = id.Key;
        const auto& PairsArray = id.Value;

        for (int PairIndex = 0; PairIndex < PairsArray.Num(); ++PairIndex)
        {
            const auto& Pair = PairsArray[PairIndex];

            PlayerActors[PairIndex]->UniqueCurve->GetCurves()[0].CurveToEdit->UpdateOrAddKey(time, Pair.Value.X, false, 0);
            PlayerActors[PairIndex]->UniqueCurve->GetCurves()[1].CurveToEdit->UpdateOrAddKey(time, Pair.Value.Y, false, 0);
            PlayerActors[PairIndex]->UniqueCurve->GetCurves()[2].CurveToEdit->UpdateOrAddKey(time, Pair.Value.Z, false, 0);
        }
    }
}

// Function to extract RGB and Alpha channels from TIFF
inline std::vector<std::vector<uint8>> ExtractTIFFChannels(const char* filename) {
    TIFF* tif = TIFFOpen(filename, "r");
    if (!tif) {
        std::cerr << "Error opening TIFF file." << std::endl;
        return {};
    }

    uint32 width, height;
    size_t npixels;
    uint32* raster;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    npixels = width * height;
    raster = (uint32*)_TIFFmalloc(npixels * sizeof(uint32));

    if (!raster || !TIFFReadRGBAImage(tif, width, height, raster, 0)) {
        TIFFClose(tif);
        std::cerr << "Error reading TIFF data." << std::endl;
        return {};
    }

    // 21 channels: RGB + 18 Alpha
    std::vector<std::vector<uint8>> channels;
    channels.resize(21, std::vector<uint8>(npixels));

    for (size_t i = 0; i < npixels; ++i) {
        channels[0][i] = TIFFGetR(raster[i]); // Red channel
        channels[1][i] = TIFFGetG(raster[i]); // Green channel
        channels[2][i] = TIFFGetB(raster[i]); // Blue channel
        channels[3][i] = TIFFGetA(raster[i]); // Alpha channel
    }

    _TIFFfree(raster);
    TIFFClose(tif);
    return channels;
}

TArray<uint8> ATaskGameModeBase::ChannelData()
{
    TArray<uint8> ChannelData;
    std::string filePath = "path/to/your/file.tif";  // Replace with your actual file path
    //auto channels = ExtractTIFFChannels(filePath.c_str());

    //TIFF* image = nullptr;
    ////image.

    //for (auto channel : channels)
    //{
	   // for (uint8 i : channel)
	   // {
		  //  ChannelData.Push(i);
	   // }
    //}

    return ChannelData;
}

