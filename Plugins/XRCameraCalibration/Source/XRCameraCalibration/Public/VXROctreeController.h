// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "VXROctreeController.generated.h"

UCLASS()
class XRCAMERACALIBRATION_API AVXROctreeController : public AActor
{
    GENERATED_UCLASS_BODY()
public:
    UFUNCTION( BlueprintCallable, Category="VXROctreeController|Functions" )
    FRotator GetCollectCameraRotationFromRootOctree( const FVector& InCameraPosition );
    UFUNCTION( BlueprintCallable, Category="VXROctreeController|Functions" )
    FRotator GetCollectCameraRotationFromOctree( const FVector& InCameraPosition, class AVXROctree* InOctreeNode );

    UFUNCTION( BlueprintCallable, Category="VXROctreeController|Functions" )
    bool FindOctreeNode( const FVector& InCameraPosition );
    UFUNCTION( BlueprintCallable, Category="VXROctreeController|Functions" )
    bool InsertToOctree( const FVector& InCameraPosition );

    UFUNCTION( BlueprintCallable, Category="VXROctreeController|Functions" )
    void SaveOctreeElementDatas();
    UFUNCTION( BlueprintCallable, Category="VXROctreeController|Functions" )
    void LoadOctreeElementDatas();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay( EEndPlayReason::Type InEndPlayReason ) override;

    bool InsertPositionInOctree( const FVector& InCameraPosition );
    bool InsertElementInOctree( const FVector& InCameraPosition, float InOffsetYaw, float InOffsetPitch );

private:
    float GetCollectCameraRotatorComponent( const FVector& InCameraPosition, float InRotComp0, float InRotComp1, 
        const FVector& InElementPos0, const FVector& InElementPos1, const FColor& InColor );

public:
    UPROPERTY( EditInstanceOnly, BlueprintReadWrite, Category="VXROctreeController|Operator" )
    bool InsertAction;
    UPROPERTY( EditInstanceOnly, BlueprintReadWrite, Category="VXROctreeController|Operator" )
    float OffsetYaw;
    UPROPERTY( EditInstanceOnly, BlueprintReadWrite, Category="VXROctreeController|Operator" )
    float OffsetPitch;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="VXROctreeController|Properties" )
    FVector Extent;
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="VXROctreeController|Properties" )
    TSubclassOf<class AVXROctreeElement> ElementClass;
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="VXROctreeController|Properties" )
    int32 MaxDepth;
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="VXROctreeController|Properties" )
    int32 MaxElements;
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="VXROctreeController|Properties" )
    bool UseDebugDraw;
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="VXROctreeController|Properties" )
    FColor NodeColor;
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="VXROctreeController|Properties" )
    float DebugDrawLifeTime;

    UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category="VXROctreeController|Properties", meta=(RelativePath) )
    FDirectoryPath ElementDataPath;
    UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category="VXROctreeController|Properties" )
    FString ElementDataFilename;

public:
    UPROPERTY( Transient, BlueprintReadOnly )
    class AVXROctree* RootOctree;
    UPROPERTY( Transient, BlueprintReadOnly )
    class AVXROctree* CurrentOctree;

private:
    FTimerHandle DebugDrawHandle;
};
