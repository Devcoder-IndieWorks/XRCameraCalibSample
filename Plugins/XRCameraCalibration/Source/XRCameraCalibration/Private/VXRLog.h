// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN( LogVXR, Log, All )

#define VXR_LOG_CALLINFO                              (FString( TEXT( "[" ) ) + FString( __FUNCTION__ ) + TEXT( "(" ) + FString::FromInt( __LINE__ ) + TEXT( ")" ) + FString( TEXT( "]" ) ) )
#define VXR_LOG_CALLONLY( Verbosity )                 UE_LOG( LogVXR, Verbosity, TEXT( "%s" ), *VXR_LOG_CALLINFO )
#define VXR_LOG( Verbosity, Format, ... )             UE_LOG( LogVXR, Verbosity, TEXT( "%s LOG: %s" ), *VXR_LOG_CALLINFO, *FString::Printf( Format, ##__VA_ARGS__ ) )
#define VXR_CLOG( Condition, Verbosity, Format, ... ) UE_CLOG( Condition, LogVXR, Verbosity, TEXT( "%s LOG: %s" ), *VXR_LOG_CALLINFO, *FString::Printf( Format, ##__VA_ARGS__ ) )