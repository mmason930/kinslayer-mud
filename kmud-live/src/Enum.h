#ifndef ENUM_H
#define ENUM_H

#include <string>
#include <list>

class Enum {

protected:
	std::string standardName;
	int value;

	static std::list<Enum*> enums;

public:

	static Enum *getEnumByValue(int v) {

		for(std::list<Enum*>::iterator iter = enums.begin();iter != enums.end();++iter) {

			Enum *e = (*iter);
			if( e->getValue() == v ) {

				return e;
			}
		}
		
		return NULL;
	}

	std::string getStandardName() {
		
		return standardName;
	}
	int getValue() {

		return value;
	}
};

#endif
