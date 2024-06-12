

using UnrealBuildTool;
using System.Collections.Generic;

public class RuneMagicTarget : TargetRules
{
	public RuneMagicTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "RuneMagic" } );
	}
}
