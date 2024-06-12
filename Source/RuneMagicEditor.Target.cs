

using UnrealBuildTool;
using System.Collections.Generic;

public class RuneMagicEditorTarget : TargetRules
{
	public RuneMagicEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "RuneMagic" } );
	}
}
