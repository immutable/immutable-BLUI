using System.IO;
using UnrealBuildTool;

public class BluExtension : ModuleRules
{
	public string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
	}

	public BluExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Blu",
			"Core",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"InputCore",
			"RHI",
			"RenderCore",
			"Slate",
			"SlateCore"
		});

		PublicIncludePaths.AddRange(new string[]
		{
			Path.Combine(ThirdPartyPath, "cef/Win")
		});
	}
}