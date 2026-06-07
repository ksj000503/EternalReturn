#include "StructureBase.h"

AStructureBase::AStructureBase()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
}
