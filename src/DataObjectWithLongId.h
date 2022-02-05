#ifndef DATA_OBJECT_WITH_LONG_ID
#define DATA_OBJECT_WITH_LONG_ID

#include <functional>

class DataObjectWithLongId
{
protected:
	long long id;
public:

	DataObjectWithLongId()
	{
		id = -1;
	}

	bool isNew() const
	{
		return id == -1;
	}

	long long getId() const
	{
		return id;
	}

	std::function<long long()> getIdFunction() {
		return std::bind(&DataObjectWithLongId::getId, this);
	}

	void setId(const long long id)
	{
		this->id = id;
	}
};

#endif