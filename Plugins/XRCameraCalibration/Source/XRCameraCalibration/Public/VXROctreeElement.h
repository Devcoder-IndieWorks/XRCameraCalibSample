// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "VXROctreeElement.generated.h"

UCLASS()
class XRCAMERACALIBRATION_API AVXROctreeElement : public AActor
{
    GENERATED_UCLASS_BODY()
public:
    UFUNCTION( BlueprintCallable, Category="VXROctreeElement|Functons" )
    void DrawDebug();
    UFUNCTION( BlueprintCallable, Category="VXROctreeElement|Functons" )
    void PrintData();

public:
    void Setup( float InOffsetYaw, float InOffsetPitch, const FVector& InDrawExtent, const FColor& InColor );

    float GetOffsetYaw() const;
    float GetOffsetPitch() const;

    FString DataToString() const;

public:
    static float DrawLifeTime;

    //-------------------------------------------------------------------------

    FVector DrawExtent;
    FColor DrawColor;

private:
    UPROPERTY( EditInstanceOnly, BlueprintReadWrite, Category="VXROctreeElement|Properties", meta=(AllowPrivateAccess=true))
    float OffsetYaw;
    UPROPERTY( EditInstanceOnly, BlueprintReadWrite, Category="VXROctreeElement|Properties", meta=(AllowPrivateAccess=true))
    float OffsetPitch;
};
