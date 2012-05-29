#ifndef ENUM_H
#define ENUM_H

#include <string>
#include <list>

class Enum {

protected:
	std::string standardName;
	int value;

	static std::list<Enum*> enums;

	Enum(int value, const std::string &standardName)
	{
		this->value = value;
		this->standardName = standardName;
	}

public:

	static Enum *getEnumByValue(int v)
	{
		for(std::list<Enum*>::iterator iter = enums.begin();iter != enums.end();++iter)
		{
			Enum *e = (*iter);
			if( e->getValue() == v )
			{
				return e;
			}
		}
		
		return NULL;
	}

	std::string getStandardName() const {
		
		return standardName;
	}
	int getValue() const {

		return value;
	}
};

#endif
