using UnrealBuildTool;
using System.Collections.Generic;

public class EternalReturnServerTarget : TargetRules
{
    public EternalReturnServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        ExtraModuleNames.Add("EternalReturn");
    }
}