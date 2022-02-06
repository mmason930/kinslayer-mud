
#include "DotModeArea.h"

DotModeArea *DotModeArea::ground	= new DotModeArea(0, "Ground"	, "ground"	);
DotModeArea *DotModeArea::inventory	= new DotModeArea(1, "Inventory", "inv"		);
DotModeArea *DotModeArea::equipment	= new DotModeArea(2, "Equipment", "eq"		);

