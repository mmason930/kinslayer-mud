#ifndef OLC_EDIT_TYPE_H
#define OLC_EDIT_TYPE_H

#include <list>
#include "../Enum.h"

class OlcEditType : public Enum<OlcEditType> {

private:
	OlcEditType(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:

	static OlcEditType *room;
	static OlcEditType *object;
	static OlcEditType *mobile;
	static OlcEditType *zone;
};


#endif

