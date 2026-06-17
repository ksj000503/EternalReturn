#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Engine/DataTable.h"

#include "ERTypes.generated.h"

UENUM(BlueprintType)
enum class EStatusEffect : uint8
{
    None      UMETA(DisplayName = "없음"),
    Stun      UMETA(DisplayName = "기절"),
    Root      UMETA(DisplayName = "속박"),
    Silence   UMETA(DisplayName = "침묵"),
    Slow      UMETA(DisplayName = "감속"),
    Freeze    UMETA(DisplayName = "빙결"),
    Blind     UMETA(DisplayName = "실명"),
    Burn      UMETA(DisplayName = "화상"),
    Bleed     UMETA(DisplayName = "출혈"),
    Poison    UMETA(DisplayName = "중독"),
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Weapon      UMETA(DisplayName = "무기"),
    Head        UMETA(DisplayName = "머리"),
    Chest       UMETA(DisplayName = "몸통"),
    Arm         UMETA(DisplayName = "팔"),
    Leg         UMETA(DisplayName = "신발"),
    Food        UMETA(DisplayName = "음식"),
    Material    UMETA(DisplayName = "재료")
};

UENUM(BlueprintType)
enum class EItemGrade : uint8
{
    Common       UMETA(DisplayName = "일반"),
    Uncommon     UMETA(DisplayName = "고급"),
    Rare         UMETA(DisplayName = "희귀"),
    Epic         UMETA(DisplayName = "영웅"),
    Legend       UMETA(DisplayName = "전설"),
    Transcendent UMETA(DisplayName = "초월")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    None            UMETA(DisplayName = "없음"),
    Bow             UMETA(DisplayName = "활"),
    Crossbow        UMETA(DisplayName = "석궁"),
    Camera          UMETA(DisplayName = "카메라"),
    Glove           UMETA(DisplayName = "글러브"),
    Dagger          UMETA(DisplayName = "단검"),
    TwoHandedSword  UMETA(DisplayName = "양손검"),
    Bat             UMETA(DisplayName = "방망이"),
    Whip            UMETA(DisplayName = "채찍"),
    Pistol          UMETA(DisplayName = "권총"),
    Tonfa           UMETA(DisplayName = "톤파"),
    Nunchaku        UMETA(DisplayName = "쌍절곤"),
    Axe             UMETA(DisplayName = "도끼"),
    Hammer          UMETA(DisplayName = "망치"),
    Arcana          UMETA(DisplayName = "아르카나"),
    SniperRifle     UMETA(DisplayName = "저격총"),
    Throwable       UMETA(DisplayName = "투척"),
    Spear           UMETA(DisplayName = "창"),
    Shuriken        UMETA(DisplayName = "암기"),
    DualSword       UMETA(DisplayName = "쌍검"),
    Rapier          UMETA(DisplayName = "레이피어"),
    AssaultRifle    UMETA(DisplayName = "돌격소총"),
    Guitar          UMETA(DisplayName = "기타"),
    VFArm           UMETA(DisplayName = "VF의수")
};

UENUM(BlueprintType)
enum class EMonsterType : uint8
{
    Chicken     UMETA(DisplayName = "닭"),
    WildDog     UMETA(DisplayName = "들개"),
    Boar        UMETA(DisplayName = "멧돼지"),
    Wolf        UMETA(DisplayName = "늑대"),
    Bear        UMETA(DisplayName = "곰"),
    Crow        UMETA(DisplayName = "까마귀"),
    Bat         UMETA(DisplayName = "박쥐"),
    Alpha       UMETA(DisplayName = "알파"),
    Wickeline   UMETA(DisplayName = "위클라인")
};


UENUM(BlueprintType)
enum class ESpawnType : uint8
{
    NoSpawnMaterial     UMETA(DisplayName = "no스폰"),
    SpawnMaterial     UMETA(DisplayName = "스폰")

};

UENUM(BlueprintType)
enum class ESkillType : uint8
{
    Passive         UMETA(DisplayName = "패시브"),
    Toggle          UMETA(DisplayName = "모드 토글"),
    Buff            UMETA(DisplayName = "강화"),
    Projectile      UMETA(DisplayName = "단일 투사체 공격"),
    Dash            UMETA(DisplayName = "이동기"),
    CC              UMETA(DisplayName = "상태이상 적용"),
    AreaDamage      UMETA(DisplayName = "광역 공격")
};

UENUM(BlueprintType)
enum class ESkillKeyType : uint8
{
    Skill_Q            UMETA(DisplayName = "Q"),
    Skill_W            UMETA(DisplayName = "W"),
    Skill_E            UMETA(DisplayName = "E"),
    Skill_R            UMETA(DisplayName = "R"),
    Skill_D            UMETA(DisplayName = "D"),
    Skill_F            UMETA(DisplayName = "F")
};

UENUM(BlueprintType)
enum class ETacticalSkillType : uint8
{
    Blink                UMETA(DisplayName = "블링크"),
    Quake                UMETA(DisplayName = "지진"),
    ProtocolViolation    UMETA(DisplayName = "프로토콜 위반"),
    ElectricShift        UMETA(DisplayName = "붉은 폭풍"),
    ForceField           UMETA(DisplayName = "초월"),
    Totem                UMETA(DisplayName = "아티팩트"),
    Nullification        UMETA(DisplayName = "무효화"),
    SoulStealer          UMETA(DisplayName = "강한 결속"),
    TheStrijder          UMETA(DisplayName = "스트라이더 - A13"),
    BladeofTruth         UMETA(DisplayName = "진실의 칼날"),
    HealingWind          UMETA(DisplayName = "치유의 바람")
};

USTRUCT(BlueprintType)
struct FS_ItemStatData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHp = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HPRegen = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HPRegenPercent = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackPower = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Defense = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MoveSpeed = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackSpeed = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackRange = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SkillAmplification = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CooldownReduction = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BasicAttackBonus = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageReduction = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CriticalChance = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CriticalDamage = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AdaptiveStatValue = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ArmorPenetration = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LifeSteal = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Sight = 0.f;
};

USTRUCT(BlueprintType)
struct FS_ItemData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESpawnType SpawnType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EWeaponType WeaponType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemGrade ItemGrade;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HPRestore = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Material1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Material2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FS_ItemStatData Stats;
};

USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    bool bIsEmpty = true;

    UPROPERTY(BlueprintReadWrite)
    FName ItemID;
};

USTRUCT(BlueprintType)
struct FStructureInventory
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FName ItemID;
};

USTRUCT(BlueprintType)
struct FTacticalSkillStat
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Cooldown_L1 = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Cooldown_L2 = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage_L1 = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage_L2 = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Heal_L1 = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Heal_L2 = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Shield_L1 = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Shield_L2 = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Range = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;
};

USTRUCT(BlueprintType)
struct FTacticalSkillTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ETacticalSkillType SkillType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> Image;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTacticalSkillStat SkillStat;
};