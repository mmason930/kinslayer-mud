#include "../conf.h"
#include "../sysdep.h"

#include "DotModeTargetType.h"

DotModeTargetType *DotModeTargetType::specific = new DotModeTargetType(0, "Specific");
DotModeTargetType *DotModeTargetType::all = new DotModeTargetType(1, "All");
