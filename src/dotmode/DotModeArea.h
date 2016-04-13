#ifndef DOT_MODE_AREA_H
#define DOT_MODE_AREA_H

#include "../Enum.h"

class DotModeArea : public Enum<DotModeArea> {

private:
	DotModeArea(int value, const std::string &standardName, const std::string &abbreviation) : Enum(value, standardName)
	{
		this->abbreviation = abbreviation;
	}
	
	std::string abbreviation;

public:

	std::string getAbbreviation() { return abbreviation; }

	static DotModeArea *getEnumByAbbreviation(const std::string &abbreviation)
	{
		const char *str1 = abbreviation.c_str();

		for(auto iter = getStartIterator();iter != getEndIterator();++iter)
		{
			auto dotModeArea = (*iter);
			const char *str2 = dotModeArea->getAbbreviation().c_str();

			{//Compare strings
				int ck;
				while(*str1 || *str2)
				{
					if(ck = toupper(*(str1)) - toupper(*(str2)) != 0)
						break;
					++str1;
					++str2;
				}

				if(ck == 0)
					return dotModeArea;
			}
		}

		return nullptr;
	}

	static DotModeArea *ground;
	static DotModeArea *inventory;
	static DotModeArea *equipment;
};


#endif


