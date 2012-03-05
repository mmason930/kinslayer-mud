#ifndef MISC_UTIL_H
#define MISC_UTIL_H

#include <list>
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
	i = Convert<int>(str);

	*****************/
	template<class _TT, class _FT> static _TT Convert(_FT Data)
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
	static std::map<std::string, std::string> MiscUtil::loadResourcesFromFile(const std::string &fileName);

	template<typename T>
	static std::string toString(T convertible)
	{
		static std::stringstream buffer;
		buffer.str("");
		buffer << convertible;
		return buffer.str();
	}
};

#endif
