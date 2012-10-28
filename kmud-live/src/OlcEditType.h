#ifndef OLC_EDIT_TYPE_H
#define OLC_EDIT_TYPE_H

#include <list>
#include "Enum.h"

class OlcEditType : public Enum {

private:

	static std::list<OlcEditType*> enums;

	OlcEditType(int value, const std::string &standardName) : Enum(value, standardName)
	{
		enums.push_back(this);
	}

public:

	static OlcEditType *room;
	static OlcEditType *object;
	static OlcEditType *mobile;
	static OlcEditType *zone;
};


#endif

