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
		static auto *enumMap = new std::map<int, Type*>();

		return enumMap;
	}

	static std::list<Type*> *getEnumList()
	{
		static auto *enumList = new std::list<Type*>();

		return enumList;
	}
public:

	static std::list<Type*>::const_iterator getStartIterator() { return getEnumList()->begin(); }
	static std::list<Type*>::const_iterator getEndIterator() { return getEnumList()->end(); }

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
