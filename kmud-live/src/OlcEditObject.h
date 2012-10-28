#ifndef OLC_EDIT_OBJECT_H
#define OLC_EDIT_OBJECT_H

#include "OlcEdit.h"

class OlcEditObject : public OlcEdit
{
private:
	int objectVnum;
public:

	int getObjectVnum() const;
	{
		return objectVnum;
	}

	void setObjectVnum(int objectVnum)
	{
		this->objectVnum = objectVnum;
	}
}

#endif
