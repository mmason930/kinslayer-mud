#ifndef DATA_OBJECT_WITH_INT_ID
#define DATA_OBJECT_WITH_INT_ID

#include <functional>

class DataObjectWithIntId
{
protected:
	int id;
public:

	DataObjectWithIntId()
	{
		id = -1;
	}

	bool isNew() const
	{
		return id == -1;
	}

	int getId() const
	{
		return id;
	}

	std::function<int()> getIdFunction() {
		return std::bind(&DataObjectWithIntId::getId, this);
	}

	void setId(const int id)
	{
		this->id = id;
	}
};

#endif