#include "VXROctreeController.h"
#include "VXROctree.h"
#include "VXROctreeElement.h"
#include "DrawDebugHelpers.h"

AVXROctreeController::AVXROctreeController( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    MaxDepth = 1;
    MaxElements = 2;
    UseDebugDraw = false;
    DebugDrawLifeTime = 0.1f;
}

void AVXROctreeController::BeginPlay()
{
    RootOctree = AVXROctree::SpawnRootOctree( GetWorld(), GetActorLocation(), Extent, ElementClass, 
        MaxElements, MaxDepth, DebugDrawLifeTime, NodeColor );

    Super::BeginPlay();
}

void AVXROctreeController::EndPlay( EEndPlayReason::Type InEndPlayReason )
{
    if ( DebugDrawHandle.IsValid() )
        GetWorldTimerManager().ClearTimer( DebugDrawHandle );

    Super::EndPlay( InEndPlayReason );
}

void AVXROctreeController::Tick( float InDeltaSeconds )
{
    if ( !DebugDrawHandle.IsValid() && RootOctree != nullptr ) {
        TWeakObjectPtr<AVXROctreeController> weakThis( this );
        GetWorldTimerManager().SetTimer( DebugDrawHandle, [weakThis]{
                if ( weakThis->UseDebugDraw ) {
                    weakThis->RootOctree->DrawDebugNode();
                    weakThis->RootOctree->DrawDebugElement();
                }
            }, DebugDrawLifeTime, true, 0.0f );
    }

    Super::Tick( InDeltaSeconds );
}

FRotator AVXROctreeController::GetCollectCameraRotationFromRootOctree( const FVector& InCameraPosition )
{
    auto found = FindOctreeNode( InCameraPosition );
    return GetCollectCameraRotationFromOctree( InCameraPosition, found ? CurrentOctree : RootOctree );
}

FRotator AVXROctreeController::GetCollectCameraRotationFromOctree( const FVector& InCameraPosition, AVXROctree* InOctreeNode )
{
    if ( ensure( InOctreeNode != nullptr ) ) {
        AVXROctreeElement* elems[2] = { nullptr, nullptr };
        elems[0] = InOctreeNode->FindElement( InCameraPosition, nullptr );

        if ( elems[0] != nullptr ) {
            elems[1] = InOctreeNode->FindElement( InCameraPosition, elems[0] );

            if ( elems[1] != nullptr ) {
                //VXR_LOG( Warning, TEXT( "#### Camera position:[%s] ####" ), *(InCameraPosition.ToString()) );
                //VXR_LOG( Warning, TEXT( "#### Collect find first. Name:[%s] Data:[%s] ####" ), *(elems[0]->GetName()), *(elems[0]->DataToString()) );
                //VXR_LOG( Warning, TEXT( "#### Collect find second. Name:[%s] Data:[%s] ####" ), *(elems[1]->GetName()), *(elems[1]->DataToString()) );
                FRotator offsetRot;
                offsetRot.Yaw = GetCollectCameraRotatorComponent( InCameraPosition, elems[0]->GetOffsetYaw(), elems[1]->GetOffsetYaw(), 
                    elems[0]->GetActorLocation(), elems[1]->GetActorLocation(), FColor::Red );
                offsetRot.Pitch = GetCollectCameraRotatorComponent( InCameraPosition, elems[0]->GetOffsetPitch(), elems[1]->GetOffsetPitch(), 
                    elems[0]->GetActorLocation(), elems[1]->GetActorLocation(), FColor::Blue );

                return offsetRot;
            }
        }
    }

    return FRotator::ZeroRotator;
}

float AVXROctreeController::GetCollectCameraRotatorComponent( const FVector& InCameraPosition, float InRotComp0, float InRotComp1, 
    const FVector& InElementPos0, const FVector& InElementPos1, const FColor& InColor )
{
    FVector minPos, maxPos;
    float minValue, maxValue;

    if ( InRotComp0 < InRotComp1 ) {
        minValue = InRotComp0;
        maxValue = InRotComp1;

        minPos = InElementPos0;
        maxPos = InElementPos1;
    }
    else {
        minValue = InRotComp1;
        maxValue = InRotComp0;

        minPos = InElementPos1;
        maxPos = InElementPos0;
    }

    auto direction = maxPos - minPos;
    auto dirCamera = InCameraPosition - minPos;
    auto projection = dirCamera.ProjectOnToNormal( direction.GetSafeNormal() );
    auto dirSize = direction.Size();
    auto projSize = projection.Size();

    //DrawDebugLine( GetWorld(), minPos, minPos + dirCamera, FColor::Green, true );
    //DrawDebugLine( GetWorld(), minPos, minPos + projection, InColor, true );

    if ( ensure( dirSize >= projSize ) ) {
        auto alpha = projSize / dirSize;
        //VXR_LOG( Warning, TEXT( "#### Project size:[%f] ####" ), projSize );
        //VXR_LOG( Warning, TEXT( "#### Direction size:[%f] ####" ), dirSize );
        //VXR_LOG( Warning, TEXT( "#### Lerp alpha:[%f] ####" ), alpha );
        return FMath::Lerp( minValue, maxValue, alpha );
    }

    return 0.0f;
}

bool AVXROctreeController::InsertToOctree( const FVector& InCameraPosition )
{
    if ( InsertAction ) {
        InsertAction = false;
        if ( !FMath::IsNearlyZero( OffsetYaw ) || FMath::IsNearlyZero( OffsetPitch ) ) {
            auto result = InsertElementInOctree( InCameraPosition, OffsetYaw, OffsetPitch );
            OffsetYaw = 0.0f;
            OffsetPitch = 0.0f;
            return result;
        }

        return InsertPositionInOctree( InCameraPosition );
    }

    return false;
}

bool AVXROctreeController::InsertPositionInOctree( const FVector& InCameraPosition )
{
    if ( FindOctreeNode( InCameraPosition ) )
        return CurrentOctree->InsertPositionInOctree( InCameraPosition );

    if ( RootOctree != nullptr )
        return RootOctree->InsertPositionInOctree( InCameraPosition );

    return false;
}

bool AVXROctreeController::InsertElementInOctree( const FVector& InCameraPosition, float InOffsetYaw, float InOffsetPitch )
{
    if ( FindOctreeNode( InCameraPosition ) )
        return CurrentOctree->InsertElementInOctree( InCameraPosition, InOffsetYaw, InOffsetPitch );

    if ( RootOctree != nullptr )
        return RootOctree->InsertElementInOctree( InCameraPosition, InOffsetYaw, InOffsetPitch );

    return false;
}

bool AVXROctreeController::FindOctreeNode( const FVector& InCameraPosition )
{
    if ( CurrentOctree != nullptr ) {
        if ( CurrentOctree->IsInNodeRange( InCameraPosition ) )
            return true;
    }

    if ( RootOctree != nullptr ) {
        CurrentOctree = RootOctree->FindNode( InCameraPosition );
        if ( CurrentOctree != nullptr )
            return true;
    }

    return false;
}

void AVXROctreeController::SaveOctreeElementDatas()
{
    if ( ElementDataPath.Path.IsEmpty() || ElementDataFilename.IsEmpty() )
        return;

    AsyncTask( ENamedThreads::GameThread, [this]{
        if ( RootOctree != nullptr  ) {
            TArray<FString> elementDatas;
            RootOctree->GetElementDatas( elementDatas );
        
            if ( elementDatas.Num() > 0 ) {
                auto filename = FString::Printf( TEXT( "%s.txt" ), *ElementDataFilename );
                auto saveFilePath = FPaths::Combine( ElementDataPath.Path, filename );
                auto successed = FFileHelper::SaveStringArrayToFile( elementDatas, *saveFilePath );
                VXR_CLOG( successed, Log, TEXT( "#### Success save file. File Name:[%s] ####" ), *filename );
                if ( successed ) {
                    for ( auto data : elementDatas )
                        VXR_LOG( Log, TEXT( "#### %s ####" ), *data );
                }
            }
        }
    } );
}

void AVXROctreeController::LoadOctreeElementDatas()
{
    AsyncTask( ENamedThreads::GameThread, [this]{
        if ( RootOctree != nullptr ) {
            auto filename = FString::Printf( TEXT( "%s.txt" ), *ElementDataFilename );
            auto loadFilePath = FPaths::Combine( ElementDataPath.Path, filename );
            TArray<FString> lines;
            auto successed = FFileHelper::LoadFileToStringArray( lines, *loadFilePath );
            if ( successed ) {
                TArray<FVXRCameraData> cameraDatas;
                for ( auto line : lines ) {
                    TArray<FString> data;
                    line.ParseIntoArray( data, TEXT( ":" ), false );
                    if ( !data[0].Equals( TEXT( "OctreeElement[Position, Yaw, Pitch]" ) ) )
                        continue;

                    TArray<FString> values;
                    data[1].ParseIntoArray( values, TEXT( "," ), false );
                    if ( values.Num() < 5 )
                        continue;

                    FVXRCameraData camData;
                    camData.Position.X  = FCString::Atof( *values[0] );
                    camData.Position.Y  = FCString::Atof( *values[1] );
                    camData.Position.Z  = FCString::Atof( *values[2] );
                    camData.OffsetYaw   = FCString::Atof( *values[3] );
                    camData.OffsetPitch = FCString::Atof( *values[4] );
                    cameraDatas.Add( camData );
                }

                if ( cameraDatas.Num() > 0 )
                    RootOctree->BuildOctreeWithCameraDatas( cameraDatas );
            }
        }
    } );
}
