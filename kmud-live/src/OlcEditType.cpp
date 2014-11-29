#include "OlcEditType.h"

OlcEditType *OlcEditType::room = new OlcEditType(0, "Room");
OlcEditType *OlcEditType::object = new OlcEditType(1, "Object");
OlcEditType *OlcEditType::mobile = new OlcEditType(2, "Mobile");
OlcEditType *OlcEditType::zone = new OlcEditType(3, "Zone");
