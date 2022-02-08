#ifndef MISC_UTIL_H
#define MISC_UTIL_H

#include <list>
#include <vector>
#include <map>
#include <sstream>
#include "DateTime.h"

class MiscUtil {

protected:
private:
public:

	static void findBufferColor( const char *buf, char *destBuffer );

	//Galnor: 06-03-2006
	//If convertible through a stringstream, this method will convert from _FT to _TT.
	/******** Example ************

	int i;
	std::string str = "123";
	i = convert<int>(str);

	*****************/
	template<class _TT, class _FT> static _TT convert(_FT Data)
	{
		_TT NewItem;
		std::stringstream stream;
		stream << Data;
		stream >> NewItem;
		return NewItem;
	}

	template<typename T> static T * clone(const T *p)
	{
		return p ? new T(*p) : 0;
	}

	static bool isNewLine(char c);
	static bool isNumber(const std::string &str);
	static bool isNumber(const char *str);
	static bool isInt(const char *str);
	static bool isInt(const std::string &str);

	static int random(int low, int high);
	static int dice(int number, int size);
	static std::list<int> splitToIntList(const std::string &s, const char sep_char = ' ');
	static std::string encodeDateTime(const DateTime &dateTime);
	static std::map<std::string, std::string> loadResourcesFromFile(const std::string &fileName);
	static std::string formatDateYYYYdmmdddHHcMMcSS(const DateTime &dateTime);
	static std::string formatDateYYYYmmdd(const DateTime &dateTime);
	static bool isValidEmailAddress(const std::string &emailAddress);

	template<typename T>
	static std::string toString(T convertible)
	{
		std::stringstream buffer;
		buffer << convertible;
		return buffer.str();
	}

	template<typename T>
	static std::list<T> makeSingleObjectList(T element)
	{
		std::list<T> singleObjectList;
		singleObjectList.push_back(element);
		return singleObjectList;
	}

	template<typename KeyType, typename ValueType>
	static void pushToVectorMap(std::map<KeyType, std::vector<ValueType> *> &map, const KeyType &key, const ValueType &value)
	{
		std::vector<ValueType> *container = map[key];

		if(container == nullptr)
		{
			container = new std::vector<ValueType>();
			map[key] = container;
		}
		
		container->push_back(value);
	}
	template<typename KeyType, typename ValueType>
	static void popFromVectorMap(std::map<KeyType, std::vector<ValueType> *> map, const KeyType &key)
	{
		auto vectorIter = map.find(key);

		if(vectorIter != map.end())
		{
			std::vector<ValueType> *vectorPointer = vectorIter->second;
			
			//typename std::vector<ValueType>::iterator objectIter = std::find_if(vectorPointer->begin(), vectorPointer->end(), keyRetrievalFunction);

			typename std::vector<ValueType>::iterator objectIter;

			if(objectIter != vectorPointer->end())
			{
				vectorPointer->erase(objectIter);
			}

			if(vectorPointer->empty())
			{
				delete vectorPointer;
				map.erase(vectorIter);
			}
		}
	}

	template<typename KeyType, typename ValueType>
	static void freeMapValues(std::map<KeyType, ValueType> &map)
	{
		for(auto iter : map)
			delete iter.second;
		map.clear();
	}

	template<typename KeyType, typename ValueType>
	static std::vector<ValueType> mapValuesToVector(const std::map<KeyType, ValueType> &map)
	{
		std::vector<ValueType> v;

		for(auto iter : map)
			v.push_back(iter.second);

		return v;
	}
};

#endif
