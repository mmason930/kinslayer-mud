#include "OlcEditType.h"

template < >
std::list<Enum<OlcEditType>*> Enum<OlcEditType>::enums = std::list<Enum<OlcEditType>*>();

OlcEditType *OlcEditType::room = new OlcEditType(0, "Room");
OlcEditType *OlcEditType::object = new OlcEditType(1, "Object");
OlcEditType *OlcEditType::mobile = new OlcEditType(2, "Mobile");
OlcEditType *OlcEditType::zone = new OlcEditType(3, "Zone");
