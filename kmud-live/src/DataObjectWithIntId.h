#ifndef DATA_OBJECT_WITH_INT_ID
#define DATA_OBJECT_WITH_INT_ID

class DataObjectWithIntId
{
protected:
	int id;
public:

	bool isNew() const
	{
		return id == -1;
	}

	int getId() const
	{
		return id;
	}

	void setId(const int id)
	{
		this->id = id;
	}
};

#endif