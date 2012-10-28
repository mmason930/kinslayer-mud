#ifndef ENUM_H
#define ENUM_H

#include <string>
#include <list>

class Enum {

protected:
	std::string standardName;
	int value;


	Enum(int value, const std::string &standardName)
	{
		this->value = value;
		this->standardName = standardName;
	}

public:

	std::string getStandardName() const {
		
		return standardName;
	}
	int getValue() const {

		return value;
	}
};

#endif
