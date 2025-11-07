// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class store_playground : ModuleRules
{
	public store_playground(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "SlateCore", "Slate", "InputCore", "EnhancedInput", "UMG", "Paper2D", "AIModule", "NavigationSystem", "GameplayTags", "Landscape" }); 

		PrivateDependencyModuleNames.AddRange(new string[] { "MassEntity", "MassCommon", "MassMovement", "Renderer", "RenderCore", "RHI", "CommonUI", "MediaAssets", "PhysicsCore", "OnlineSubsystem"});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
