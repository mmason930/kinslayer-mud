#include <fstream>
#include "StringUtil.h"
#include "MiscUtil.h"

#include <boost/regex.hpp>

/*** Galnor - 01/14/2010 - Get the current string's color, if there is one set ***/
void MiscUtil::findBufferColor( const char *buf, char *destBuffer )
{
	const int bSize = strlen(buf);
	char *bPos = (char*)(buf);
	char cnBuf[3];
	unsigned int n;
	*destBuffer='\0';
	for(;*bPos;++bPos)
	{
		if( !strncmp( bPos, "\x1B[", 2 ) )
		{//Looks like a color.
			bPos += 2;
			for(n = 0;n < 2 && isdigit(*bPos);++bPos, ++n)
				cnBuf[n]=*bPos;
			if( *bPos == 'm' )
			{//Format is valid. Make sure color code is proper.
				cnBuf[ n ] = '\0';
				short int cnNum = atoi(cnBuf);
				if( (cnNum >= 0 && cnNum <= 1) || (cnNum >= 31 || cnNum <= 37) )
				{//Valid.
					strcpy(destBuffer, "\x1B[");
					strcat(destBuffer, cnBuf);
					strcat(destBuffer, "m");
				}
			}
		}
	}
}

bool MiscUtil::isValidEmailAddress(const std::string &emailAddress)
{
	return boost::regex_match(emailAddress, boost::regex("[^\\@\\s]+\\@[^\\@\\s\\.]+\\.[^\\@\\s]+"));
}

bool MiscUtil::isNewLine(char c)
{
	if(c == '\n' || (c) == '\r')
		return true;
	else
		return false;
}
bool MiscUtil::isNumber(const std::string &str)
{
	if(!str.empty() && (atoi(str.c_str()) || str[0] == '0'))
		return true;
	return false;
}
bool MiscUtil::isNumber(const char *str)
{
	if(atoi(str) || *str == '0')
		return true;
	return false;
}

bool MiscUtil::isInt(const char *str)
{
	if( *str == '-' )
		++str;
	unsigned int len = strlen(str);
	for(unsigned int i = 0;i < len;++i)
	{
		if( !isdigit(str[i]) )
			return false;
	}
	return true;
}
bool MiscUtil::isInt(const std::string &str)
{
	return (isInt(str.c_str()));
}


/* creates a random number in interval [from;to] */
int MiscUtil::random(int low, int high)
{
	if (low > high)
		low ^= high ^= low ^= high;
	return ((rand() % (high - low + 1)) + low);
}

/* simulates dice roll */
int MiscUtil::dice(int number, int size)
{
	int sum = 0;

	if (size <= 0 || number <= 0)
		return 0;

	while (number-- > 0)
		sum += ((rand() % size) + 1);

	return sum;
}
std::list<int> MiscUtil::splitToIntList(const std::string &s, const char sep_char)
{
	std::list<int> list;
	std::string token = "";
	std::string letter;

	for (int i = 0; i < (signed) s.size(); ++i)
	{
		letter = s.at(i);
		if (letter.at(0) == sep_char)
		{
			if (!token.empty())
			{
				list.push_back(atoi(token.c_str()));
				token.clear();
			}

		}
		else
		{
			token.append(letter);
			letter.clear();
		}
	}
	// This was plaguing me for a while -- it doesn't push back the last character of the string.
	// Normally this would be obvious, but all of my test cases ended with a space -- except one.
	if (!token.empty())
	{
		list.push_back(atoi(token.c_str()));
		token.clear();
			//	inAToken = false;
	}
	return list;
}

std::string MiscUtil::encodeDateTime(const DateTime &dateTime)
{
	return MiscUtil::formatDateYYYYdmmdddHHcMMcSS(dateTime);
}

std::string MiscUtil::formatDateYYYYdmmdddHHcMMcSS(const DateTime &dateTime)
{
	return DateTime::parse("%Y-%m-%d %H:%M:%S", dateTime);
}

std::string MiscUtil::formatDateYYYYmmdd(const DateTime &dateTime)
{
	return DateTime::parse("%Y%m%d", dateTime);
}

std::map<std::string, std::string> MiscUtil::loadResourcesFromFile(const std::string &fileName)
{
	std::ifstream inFile(fileName.c_str());
	std::map<std::string, std::string> mapping;
	if(inFile.is_open() == false) {
		return mapping;
	}

	while(inFile.eof() == false)
	{
		std::string line;
		std::getline(inFile, line);
		std::string::size_type pos = line.find(":");
		if(pos == std::string::npos) {
			continue;
		}

		std::string left = line.substr(0,pos);
		std::string right = line.substr(pos+1);
		StringUtil::trim(left);
		StringUtil::trim(right);

		if(!left.empty() && !right.empty())
		{
			mapping[left] = right;
		}
	}

	inFile.close();
	return mapping;
}

