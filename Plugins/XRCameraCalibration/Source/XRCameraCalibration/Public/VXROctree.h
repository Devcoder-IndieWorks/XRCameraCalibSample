// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "VXRCameraData.h"
#include "GameFramework/Actor.h"
#include "VXROctree.generated.h"

UCLASS()
class XRCAMERACALIBRATION_API AVXROctree : public AActor
{
    GENERATED_UCLASS_BODY()
public:
    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions", meta=(WorldContext="InWorldContextObject", AdvancedDisplay=6) )
    static AVXROctree* SpawnRootOctree( UObject* InWorldContextObject, const FVector& InOrigin, const FVector& InExtent, 
        TSubclassOf<class AVXROctreeElement> InElementClass, int32 InMaxElements, int32 InMaxDepth, float InDrawLifeTime = 0.1f, 
        FColor InNodeColor = FColor::Blue );

    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    bool InsertPositionInOctree( const FVector& InPosition );
    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    bool InsertElementInOctree( const FVector& InPosition, float InOffsetYaw, float InOffsetPitch );

    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    class AVXROctree* FindNode( const FVector& InPosition );
    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    class AVXROctreeElement* FindElement( const FVector& InPosition, const class AVXROctreeElement* InHasElement );

    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    void BuildOctreeWithPositions( const TArray<FVector>& InPositions );
    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    void BuildOctreeWithCameraDatas( const TArray<FVXRCameraData>& InCameraDatas );

    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    void GetElementDatas( TArray<FString>& OutElementDatas );

    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    FVector GetBoundingBoxOrigin() const;
    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    FVector GetBoundingBoxExtent() const;

    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    void PrintDebugNode();
    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    void DrawDebugNode();
    UFUNCTION( BlueprintCallable, Category="VXROctree|Functions" )
    void DrawDebugElement();

public:
    bool IsInNodeRange( const FVector& InObjectPos );
    bool IsLeafNode() const;

private:
    void Init( const FVector& InOrigin, const FVector& InExtent, int32 InDepth, TSubclassOf<class AVXROctreeElement> InElementClass, 
        AVXROctree* InParentNode );
    void BuildChildrenTree();

    bool InsertChildrenTree( const FVector& InPosition );
    bool InsertChildrenTree( const FVector& InPosition, float InOffsetYaw, float InOffsetPitch );
    class AVXROctree* FindNodeFromChildrenTree( const FVector& InPosition );
    class AVXROctreeElement* FindElementFromChildrenTree( const FVector& InPosition, const class AVXROctreeElement* InHasElement );

    bool SpawnOctree( const FVector& InSpawnLocation, const FVector& InSpawnExtent, int32 InDpeth );
    bool SpawnElement( const FVector& InPosition, float InOffsetYaw, float InOffsetPitch );

    void SetBoundingBox( const FVector& InOrigin, const FVector& InExtent, int32 InDepth );

    void DrawNode( const FVector& InOrigin, const FVector& InExtent );
    void PrintNode();

protected:
    static int32 MaxDepth;
    static int32 MaxElements;
    static float DrawLifeTime;
    static FColor NodeColor;

protected:
    struct FNode
    {
        FVector Origin;
        FVector Extent;
        int32 Depth;
        FNode() = default;
    };
    FNode BoundingBox;

    //-------------------------------------------------------------------------

    UPROPERTY( transient )
    TSubclassOf<class AVXROctreeElement> NodeElementClass;
    UPROPERTY( transient )
    TArray<class AVXROctreeElement*> ElementList;
    UPROPERTY( transient )
    AVXROctree* ParentTree;
    UPROPERTY( transient )
    TArray<AVXROctree*> ChildrenTree;
};
