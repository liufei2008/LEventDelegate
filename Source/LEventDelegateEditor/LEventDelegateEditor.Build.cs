// Copyright 2019-Present LexLiu. All Rights Reserved.

using UnrealBuildTool;

public class LEventDelegateEditor : ModuleRules
{
	public LEventDelegateEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "Engine",
                "LEventDelegate",
				// ... add other public dependencies that you statically link with here ...
                "AppFramework",
                "EditorWidgets",
                "PropertyEditor",
                "InputCore",
                "Projects",

            }
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	

            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

    }
}
