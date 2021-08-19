#include "VXROctreeElement.h"
#include "DrawDebugHelpers.h"
#include "VXRLog.h"

float AVXROctreeElement::DrawLifeTime = 1.0f;

//-----------------------------------------------------------------------------

AVXROctreeElement::AVXROctreeElement( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    OffsetYaw = 0.0f;
    OffsetPitch = 0.0f;
}

void AVXROctreeElement::Setup( float InOffsetYaw, float InOffsetPitch, const FVector& InDrawExtent, const FColor& InColor )
{
    OffsetYaw = InOffsetYaw;
    OffsetPitch = InOffsetPitch;
    DrawExtent = InDrawExtent;
    DrawColor = InColor;
}

float AVXROctreeElement::GetOffsetYaw() const
{
    return OffsetYaw;
}

float AVXROctreeElement::GetOffsetPitch() const
{
    return OffsetPitch;
}

FString AVXROctreeElement::DataToString() const
{
    auto location = GetActorLocation();
    return FString::Printf( TEXT( "OctreeElement[Position, Yaw, Pitch]:%f,%f,%f,%f,%f" ), location.X, location.Y, location.Z, 
        OffsetYaw, OffsetPitch );
}

void AVXROctreeElement::DrawDebug()
{
    auto world = GetWorld();
    if ( ensure( world != nullptr ) ) {
        auto location = GetActorLocation();
        auto extent = DrawExtent * 0.15f;
        DrawDebugBox( world, location, extent, DrawColor, false, AVXROctreeElement::DrawLifeTime, (uint8)'\000', 1.0f );
        DrawDebugString( world, FVector::ZeroVector, GetName(), this, DrawColor, AVXROctreeElement::DrawLifeTime, true );
    }
}

void AVXROctreeElement::PrintData()
{
    auto location = GetActorLocation();
    VXR_LOG( Log, TEXT( "#### Location: [%s] Offset:[Yaw: %f, Pitch: %f] ####" ), 
        *(location.ToString()), OffsetYaw, OffsetPitch );
}
