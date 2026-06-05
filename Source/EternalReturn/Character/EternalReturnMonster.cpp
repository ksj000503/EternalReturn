#include "Character/EternalReturnMonster.h"

AEternalReturnMonster::AEternalReturnMonster()
{
    MonsterStatComponent = CreateDefaultSubobject<UMonsterStatComponent>(TEXT("MonsterStatComponent"));
}