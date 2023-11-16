#pragma once

#include "CoreMinimal.h"
#include "MaterialData.generated.h"

 
USTRUCT(BlueprintType)
struct TASK_API FMaterialData
{
	GENERATED_USTRUCT_BODY()

public: 
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category =  HUD ) 
	UTexture* Texture = nullptr; 

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = HUD ) 
	FLinearColor Color; 
}; 
