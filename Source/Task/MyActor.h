// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class TASK_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "TIFF Loader")
	UTexture2D* CreateTextureFromChannelData(int32 Width, int32 Height, const TArray<uint8>& ChannelData, FString ChannelDepth);

	UFUNCTION(BlueprintCallable, Category = "TIFF Loader")
	TArray<uint8> ExtractTiffChannels(int& ImageWidth, int& ImageHeight, int DepthIndex);

	UFUNCTION(BlueprintCallable, Category = "TIFF Loader")
	UTexture2DArray* UpdateTexture2DArray(UTexture2D* Texture, int& Size, bool IsRemoving);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TIFF Loader")
	UTexture2DArray* Texture2DArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TIFF Loader")
	int AlphaChannel = 0;

	UPROPERTY(EditAnywhere)
	UTexture2D* SavedImage;
};
