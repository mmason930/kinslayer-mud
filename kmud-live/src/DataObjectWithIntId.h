#ifndef DATA_OBJECT_WITH_INT_ID
#define DATA_OBJECT_WITH_INT_ID

class DataObjectWithIntId
{
protected:
	int id;
public:

	bool isNew()
	{
		return id == -1;
	}
};

#endif