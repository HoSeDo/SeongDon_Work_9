// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HSDChatX : ModuleRules
{
	public HSDChatX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",

			// UI
			"UMG", "Slate", "SlateCore",
			
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		
		PublicIncludePaths.AddRange(new string[] { "HSDChatX" });

	}
}
