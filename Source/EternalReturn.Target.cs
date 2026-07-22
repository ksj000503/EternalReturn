using UnrealBuildTool;
using System.Collections.Generic;

public class EternalReturnTarget : TargetRules
{
    public EternalReturnTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V6;              // V6 ¡æ V7
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;   // 5_7 ¡æ 5_8
        ExtraModuleNames.Add("EternalReturn");
    }
}