#ifndef ENUM_H
#define ENUM_H

#include <string>
#include <list>

template <typename Type>
class Enum {

protected:
	std::string standardName;
	int value;


	Enum(int value, const std::string &standardName)
	{
		this->value = value;
		this->standardName = standardName;
		enums.push_back(this);
	}

	static std::list<Enum*> enums;

public:

	static typename std::list<Enum*>::iterator getStartIterator() { return enums.begin(); }
	static typename std::list<Enum*>::iterator getEndIterator() { return enums.end(); }

	std::string getStandardName() const {
		
		return standardName;
	}

	int getValue() const {

		return value;
	}

	
	static Enum *getEnumByValue(int v)
	{
		for(Enum *e : enums)
		{
			if( e->getValue() == v )
			{
				return e;
			}
		}
		
		return NULL;
	}

	static void cleanup()
	{
		for(Enum *e : enums)
			delete e;
	}
};

#endif
