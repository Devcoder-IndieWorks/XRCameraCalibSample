#include "VXROctree.h"
#include "VXROctreeElement.h"
#include "DrawDebugHelpers.h"
#include "VXRLog.h"

int32 AVXROctree::MaxDepth = 0;
int32 AVXROctree::MaxElements = 0;
float AVXROctree::DrawLifeTime = 0.0f;
FColor AVXROctree::NodeColor;

//-----------------------------------------------------------------------------

AVXROctree::AVXROctree( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    PrimaryActorTick.bCanEverTick = false;

    SetBoundingBox( FVector::ZeroVector, FVector( 128.0f, 128.0f, 128.0f ), 1 );
}

AVXROctree* AVXROctree::SpawnRootOctree( UObject* InWorldContextObject, const FVector& InSpawnLocation, const FVector& InSpawnExtent, 
    TSubclassOf<AVXROctreeElement> InElementClass, int32 InMaxElements, int32 InMaxDepth, float InDrawLifeTime, FColor InNodeColor )
{
    auto world = InWorldContextObject != nullptr ? InWorldContextObject->GetWorld() : nullptr;
    if ( ensure( world != nullptr ) ) {
        FActorSpawnParameters spawnInfo;
        auto newOctree = world->SpawnActor<AVXROctree>( InSpawnLocation, FRotator::ZeroRotator, spawnInfo );
        if ( newOctree != nullptr ) {
            AVXROctree::MaxElements = InMaxElements;
            AVXROctree::MaxDepth = InMaxDepth;
            AVXROctree::DrawLifeTime = InDrawLifeTime;
            AVXROctree::NodeColor = InNodeColor;

            AVXROctreeElement::DrawLifeTime = InDrawLifeTime;

            newOctree->Init( InSpawnLocation, InSpawnExtent, 0, InElementClass, nullptr );
            return newOctree;
        }
    }

    return nullptr;
}

void AVXROctree::SetBoundingBox( const FVector& InOrigin, const FVector& InExtent, int32 InDepth )
{
    BoundingBox.Origin = InOrigin;
    BoundingBox.Extent = InExtent;
    BoundingBox.Depth = InDepth;
}

void AVXROctree::PrintDebugNode()
{
    PrintNode();

    if ( !IsLeafNode()) {
        for ( auto child : ChildrenTree )
            child->PrintDebugNode();
    }
}

void AVXROctree::PrintNode()
{
    VXR_LOG( Log, TEXT( "#### Origin: %s, Extent: %s, Depth: %d ####" ), 
        *(BoundingBox.Origin.ToString()), *(BoundingBox.Extent.ToString()), BoundingBox.Depth );
}

void AVXROctree::DrawDebugNode()
{
    DrawNode( BoundingBox.Origin, BoundingBox.Extent );

    if ( !IsLeafNode() ) {
        for ( auto child : ChildrenTree )
            child->DrawDebugNode();
    }
}

void AVXROctree::DrawNode( const FVector& InOrigin, const FVector& InExtent )
{
    auto world = GetWorld();
    if ( ensure( world != nullptr ) )
        DrawDebugBox( world, InOrigin, InExtent, AVXROctree::NodeColor, false, AVXROctree::DrawLifeTime + 0.1f, (uint8)'\000', 2.0f );
}

void AVXROctree::DrawDebugElement()
{
    if ( IsLeafNode() ) {
        for ( auto elem : ElementList )
            elem->DrawDebug();
    }
    else {
        for ( auto child : ChildrenTree )
            child->DrawDebugElement();
    }
}

FVector AVXROctree::GetBoundingBoxOrigin() const
{
    return BoundingBox.Origin;
}

FVector AVXROctree::GetBoundingBoxExtent() const
{
    return BoundingBox.Extent;
}

void AVXROctree::BuildOctreeWithPositions( const TArray<FVector>& InPositions )
{
    for ( auto pos : InPositions ) {
        VXR_LOG( Log, TEXT( "#### Instert octree. Element Position:[%s] ####" ), *(pos.ToString()) );
        InsertPositionInOctree( pos );
    }
}

bool AVXROctree::InsertPositionInOctree( const FVector& InPosition )
{
    if ( BoundingBox.Depth < AVXROctree::MaxDepth && IsInNodeRange( InPosition ) ) {
        if ( InsertChildrenTree( InPosition ) )
            return true;
        
        if ( IsLeafNode() ) {
            BuildChildrenTree();
            if ( InsertChildrenTree( InPosition ) )
                return true;
        }

        if ( ChildrenTree.Num() > 0 ) {
            for ( auto octree : ChildrenTree )
                octree->Destroy();
            ChildrenTree.Empty();
        }

        if ( ElementList.Num() < AVXROctree::MaxElements ) {
            SpawnElement( InPosition, 0.0f, 0.0f );
            return true;
        }

        VXR_LOG( Warning, TEXT( "#### Overflow elements per node. Max Elements:[%d] ####" ), AVXROctree::MaxElements );
        return false;
    }

    VXR_LOG( Log, TEXT( "#### Cannot be inserted to the Octree. Octree Depth:[%d] Element Position:[%s] ####" ), 
        BoundingBox.Depth, *(InPosition.ToString()) );
    return false;
}

bool AVXROctree::InsertChildrenTree( const FVector& InPosition )
{
    if ( !IsLeafNode() ) {
        for ( auto octree : ChildrenTree ) {
            if ( octree->InsertPositionInOctree( InPosition ) )
                return true;
        }
    }
    return false;
}

void AVXROctree::BuildOctreeWithCameraDatas( const TArray<FVXRCameraData>& InCameraDatas )
{
    for ( auto& data : InCameraDatas ) {
        VXR_LOG( Log, TEXT( "#### Insert octree. Camera Position:[%s], Offset[Yaw, Pitch]:[%f, %f] ####" ), 
            *(data.Position.ToString()), data.OffsetYaw, data.OffsetPitch );
        InsertElementInOctree( data.Position, data.OffsetYaw, data.OffsetPitch );
    }
}

bool AVXROctree::InsertElementInOctree( const FVector& InPosition, float InOffsetYaw, float InOffsetPitch )
{
    if ( BoundingBox.Depth < AVXROctree::MaxDepth && IsInNodeRange( InPosition ) ) {
        if ( InsertChildrenTree( InPosition, InOffsetYaw, InOffsetPitch ) )
            return true;

        if ( IsLeafNode() ) {
            BuildChildrenTree();
            if ( InsertChildrenTree( InPosition, InOffsetYaw, InOffsetPitch ) )
                return true;
        }

        if ( ChildrenTree.Num() > 0 ) {
            for ( auto octree : ChildrenTree )
                octree->Destroy();
            ChildrenTree.Empty();
        }

        if ( ElementList.Num() < AVXROctree::MaxElements ) {
            SpawnElement( InPosition, InOffsetYaw, InOffsetPitch );
            return true;
        }

        VXR_LOG( Warning, TEXT( "#### Overflow elements per node. Max Elements:[%d] ####" ), AVXROctree::MaxElements );
        return false;
    }

    VXR_LOG( Log, TEXT( "#### Cannot be inserted to the Octree. Octree Depth:[%d] Element Position:[%s] ####" ), 
        BoundingBox.Depth, *(InPosition.ToString()) );
    return false;
}

bool AVXROctree::InsertChildrenTree( const FVector& InPosition, float InOffsetYaw, float InOffsetPitch )
{
    if ( !IsLeafNode() ) {
        for ( auto octree : ChildrenTree ) {
            if ( octree->InsertElementInOctree( InPosition, InOffsetYaw, InOffsetPitch ) )
                return true;
        }
    }
    return false;
}

bool AVXROctree::SpawnElement( const FVector& InPosition, float InOffsetYaw, float InOffsetPitch )
{
    auto world = GetWorld();
    if ( ensure( world != nullptr ) ) {
        FActorSpawnParameters spawnInfo;
        auto newElement = world->SpawnActor<AVXROctreeElement>( NodeElementClass, InPosition, FRotator::ZeroRotator, spawnInfo );
        if ( ensure( newElement != nullptr ) ) {
            auto color = FColor( FMath::RandRange(0, 255), FMath::RandRange(0, 255), FMath::RandRange(0, 255) );
            newElement->Setup( InOffsetYaw, InOffsetPitch, BoundingBox.Extent, color );
            ElementList.Add( newElement );

            VXR_LOG( Log, TEXT( "#### Insert to the octree node. Depth:[%d] Position:[%s] ####" ), 
                BoundingBox.Depth, *(InPosition.ToString()) );
            return true;
        }
    }

    return false;
}

void AVXROctree::BuildChildrenTree()
{
    int32 depth = BoundingBox.Depth + 1;
    if ( depth > AVXROctree::MaxDepth ) {
        VXR_LOG( Warning, TEXT( "#### Overflow octree depth. Max Depth:[%d] ####" ), AVXROctree::MaxDepth );
        return;
    }

    VXR_LOG( Log, TEXT( "#### Build children octree. ####" ) );
    auto halfDimension = BoundingBox.Extent * 0.5f;
    auto center = halfDimension;

    FVector nodeOrigins[8];

    // Top Left Back: -X, -Y, +Z
    nodeOrigins[0] = FVector( BoundingBox.Origin.X - center.X, BoundingBox.Origin.Y - center.Y, BoundingBox.Origin.Z + center.Z );
    // Top Right Back: +X, -Y, +Z
    nodeOrigins[1] = FVector( BoundingBox.Origin.X + center.X, BoundingBox.Origin.Y - center.Y, BoundingBox.Origin.Z + center.Z );
    // Top Left Front: -X, +Y, +Z
    nodeOrigins[2] = FVector( BoundingBox.Origin.X - center.X, BoundingBox.Origin.Y + center.Y, BoundingBox.Origin.Z + center.Z );
    // Top Right Front: +X, +Y, +Z
    nodeOrigins[3] = FVector( BoundingBox.Origin.X + center.X, BoundingBox.Origin.Y + center.Y, BoundingBox.Origin.Z + center.Z );

    // Bottom Left Back: -X, _y, -Z
    nodeOrigins[4] = FVector( BoundingBox.Origin.X - center.X, BoundingBox.Origin.Y - center.Y, BoundingBox.Origin.Z - center.Z );
    // Bottom Right Back: +X, -Y, -Z
    nodeOrigins[5] = FVector( BoundingBox.Origin.X + center.X, BoundingBox.Origin.Y - center.Y, BoundingBox.Origin.Z - center.Z );
    // Bottom Left Front: -X, +Y, -Z
    nodeOrigins[6] = FVector( BoundingBox.Origin.X - center.X, BoundingBox.Origin.Y + center.Y, BoundingBox.Origin.Z - center.Z );
    // Bottom Right Front: +X, +Y, -Z
    nodeOrigins[7] = FVector( BoundingBox.Origin.X + center.X, BoundingBox.Origin.Y + center.Y, BoundingBox.Origin.Z - center.Z );

    for ( auto origin : nodeOrigins ) {
        if ( !SpawnOctree( origin, halfDimension, depth ) )
            return;
    }
}

bool AVXROctree::SpawnOctree( const FVector& InSpawnLocation, const FVector& InSpawnExtent, int32 InDepth )
{
    if ( ensure( ChildrenTree.Num() < 8 ) ) {
        auto world = GetWorld();
        if ( ensure( world != nullptr ) ) {
            FActorSpawnParameters spawnInfo;
            auto newOctree = world->SpawnActor<AVXROctree>( InSpawnLocation, FRotator::ZeroRotator, spawnInfo );
            if ( newOctree != nullptr ) {
                newOctree->Init( InSpawnLocation, InSpawnExtent, BoundingBox.Depth + 1, NodeElementClass, this );
                ChildrenTree.Push( newOctree );
                return true;
            }
        }
    }

    return false;
}

void AVXROctree::Init( const FVector& InOrigin, const FVector& InExtent, int32 InDepth, TSubclassOf<AVXROctreeElement> InElementClass, 
    AVXROctree* InParentNode )
{
    ParentTree = InParentNode;
    NodeElementClass = InElementClass;

    SetBoundingBox( InOrigin, InExtent, InDepth );
}

AVXROctreeElement* AVXROctree::FindElement( const FVector& InPosition, const AVXROctreeElement* InHasElement )
{
    if ( IsInNodeRange( InPosition ) ) {
        AVXROctreeElement* found = nullptr;
        if ( !IsLeafNode() ) {
            found = FindElementFromChildrenTree( InPosition, InHasElement );
            if ( found != nullptr )
                return found;
        }

        if ( ElementList.Num() > 0 ) {
            int32 beginIdx = 1;
            found = ElementList[0];
            if ( found == InHasElement ) {
                beginIdx = 2;
                found = ElementList[1];
            }

            auto location = found->GetActorLocation();
            auto min = FVector( FMath::Abs( InPosition.X - location.X ), 
                FMath::Abs( InPosition.Y - location.Y ), FMath::Abs( InPosition.Z - location.Z ) );

            for ( int32 i = beginIdx; i < ElementList.Num(); ++i ) {
                auto elem = ElementList[i];
                if ( elem == InHasElement )
                    continue;

                auto elemLocation = elem->GetActorLocation();
                auto dist = FVector( FMath::Abs( InPosition.X - elemLocation.X ), 
                    FMath::Abs( InPosition.Y - elemLocation.Y ), FMath::Abs( InPosition.Z - elemLocation.Z ) );

                if ( min.SizeSquared() > dist.SizeSquared() ) {
                    min = dist;
                    found = elem;
                }
            }

            return found;
        }
    }

    return nullptr;
}

AVXROctreeElement* AVXROctree::FindElementFromChildrenTree( const FVector& InPosition, const AVXROctreeElement* InHasElement )
{
    for ( auto child : ChildrenTree ) {
        auto found = child->FindElement( InPosition, InHasElement );
        if ( found != nullptr )
            return found;
    }

    return nullptr;
}

AVXROctree* AVXROctree::FindNode( const FVector& InPosition )
{
    if ( IsInNodeRange( InPosition ) ) {
        AVXROctree* found = nullptr;
        if ( !IsLeafNode() ) {
            found = FindNodeFromChildrenTree( InPosition );
            if ( found != nullptr )
                return found;
        }

        return this;
    }

    return nullptr;
}

AVXROctree* AVXROctree::FindNodeFromChildrenTree( const FVector& InPosition )
{
    for ( auto child : ChildrenTree ) {
        auto found = child->FindNode( InPosition );
        if ( found != nullptr )
            return found;
    }

    return nullptr;
}

bool AVXROctree::IsInNodeRange( const FVector& InObjectPos )
{
    auto minX = FMath::Min( BoundingBox.Origin.X - BoundingBox.Extent.X, BoundingBox.Origin.X + BoundingBox.Extent.X );
    auto maxX = FMath::Max( BoundingBox.Origin.X - BoundingBox.Extent.X, BoundingBox.Origin.X + BoundingBox.Extent.X );

    auto minY = FMath::Min( BoundingBox.Origin.Y - BoundingBox.Extent.Y, BoundingBox.Origin.Y + BoundingBox.Extent.Y );
    auto maxY = FMath::Max( BoundingBox.Origin.Y - BoundingBox.Extent.Y, BoundingBox.Origin.Y + BoundingBox.Extent.Y );

    auto minZ = FMath::Min( BoundingBox.Origin.Z - BoundingBox.Extent.Z, BoundingBox.Origin.Z + BoundingBox.Extent.Z );
    auto maxZ = FMath::Max( BoundingBox.Origin.Z - BoundingBox.Extent.Z, BoundingBox.Origin.Z + BoundingBox.Extent.Z );

    if ( (InObjectPos.X >= minX && InObjectPos.X <= maxX) && 
         (InObjectPos.Y >= minY && InObjectPos.Y <= maxY) && 
         (InObjectPos.Z >= minZ && InObjectPos.Z <= maxZ) ) {
        return true;
    }

    return false;
}

bool AVXROctree::IsLeafNode() const
{
    return ChildrenTree.Num() == 0;
}

void AVXROctree::GetElementDatas( TArray<FString>& OutElementDatas )
{
    if ( IsLeafNode() && ElementList.Num() > 0 ) {
        for ( auto elem : ElementList )
            OutElementDatas.Add( elem->DataToString() );

        return;
    }

    for ( auto child : ChildrenTree )
        child->GetElementDatas( OutElementDatas );
}
