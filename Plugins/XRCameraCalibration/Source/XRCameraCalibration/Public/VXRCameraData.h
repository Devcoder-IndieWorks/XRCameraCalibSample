// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "UObject/ObjectMacros.h"
#include "VXRCameraData.generated.h"

USTRUCT( BlueprintType ) 
struct XRCAMERACALIBRATION_API FVXRCameraData
{
    GENERATED_USTRUCT_BODY()
    FVXRCameraData() = default;

    //-------------------------------------------------------------------------

    UPROPERTY( Transient, BlueprintReadWrite )
    FVector Position;
    UPROPERTY( Transient, BlueprintReadWrite )
    float OffsetYaw;
    UPROPERTY( Transient, BlueprintReadWrite )
    float OffsetPitch;
};
