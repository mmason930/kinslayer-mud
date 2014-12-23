#ifndef ENUM_H
#define ENUM_H

#include <string>
#include <list>
#include <map>

template <typename Type>
class Enum {

protected:
	std::string standardName;
	int value;
	
	Enum(int value, const std::string &standardName)
	{
		this->value = value;
		this->standardName = standardName;
		getEnumList()->push_back((Type*)this);
		(*(getEnumMap()))[value] = (Type*)this;
	}

	static std::map<int, Type*> *getEnumMap()
	{
		static std::map<int, Type*> *enumMap = new std::map<int, Type*>();

		return enumMap;
	}

	static std::list<Type*> *getEnumList()
	{
		static std::list<Type*> *enumList = new std::list<Type*>();

		return enumList;
	}
public:

	static typename std::list<Type*>::const_iterator getStartIterator() { return getEnumList()->begin(); }
	static typename std::list<Type*>::const_iterator getEndIterator() { return getEnumList()->end(); }

	std::string getStandardName() const {
		
		return standardName;
	}

	int getValue() const {

		return value;
	}
	
	static Type *getEnumByValue(int v)
	{
		auto map = getEnumMap();
		auto iter = map->find(v);

		return iter == map->end() ? nullptr : (*iter).second;
	}

	/***
	static Type *getEnumByStandardName(const std::string &standardNameToFind, bool ignoreCase = true)
	{
		auto list = getEnumList();
		for(Enum *e : (*list))
		{
			if(ignoreCase)
			{
				if(!str_cmp(e->getStandardName(), standardNameToFind))
					return (Type*)e;
			}
			else if(!strcmp(e->getStandardName().c_str(), standardNameToFind.c_str()))
				return (Type*)e;
		}
	}
	***/

	static void cleanup()
	{
		auto map = getEnumMap();
		auto list = getEnumList();
		for(Enum *e : (*list))
			delete e;

		delete map;
		delete list;
	}
};

#endif
