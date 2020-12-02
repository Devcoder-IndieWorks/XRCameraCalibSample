// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class XRCameraCalibration : ModuleRules
{
	public XRCameraCalibration(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = false;
		
		PrivateIncludePaths.AddRange(
		    new string[] {
				"XRCameraCalibration/Private"
			});

		PrivateDependencyModuleNames.AddRange(
		    new string[] {
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore"
			});
	}
}