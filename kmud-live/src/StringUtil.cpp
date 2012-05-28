<<<<<<< .mine
#include <string>
#include <vector>
#include <list>
#include <cstring>

#include "StringUtil.h"
#include "MiscUtil.h"

std::string StringUtil::getRandomString(const int size)
{
	if(size <= 0) {
		return "";
	}

	std::string buffer;
	static char *symbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
	int numberOfSymbols = strlen(symbols);
	for(int i = 0;i < size;++i) {

		buffer += symbols[MiscUtil::random(0,numberOfSymbols-1)];
	}

	return buffer;
}

//2011-02-06 - Replaces string contents based on the value of toFind.
void StringUtil::replace(std::string &input, const std::string &toFind, const std::string &replaceWith)
{
	std::string::size_type pos = 0;
	while(pos < input.size())
	{
		if(input.substr(pos, toFind.size()) == toFind)
		{

			input.replace(pos, toFind.size(), replaceWith);

			pos += replaceWith.size();
		}
		else {

			++pos;
		}
	}
}


//Galnor: 2006-07-26: Rewrote this function.
void StringUtil::trim(std::string &s)
{
	std::string::size_type pos;

	for(pos = 0;pos < s.size() && isspace(s[pos]);++pos);
	s.erase(0, pos);
	for(pos = (0 > (int)s.size() ? 0 : (int)s.size());pos && s[pos - 1] == ' ';--pos);
	s.erase(pos, s.size());
}

/*** Add slashes to escape characters. ***/
void StringUtil::addSlashes( std::string &sBuffer, const std::string &EscChars )
{
	for(unsigned int i = 0;i < sBuffer.size();++i)
	{
		if( EscChars.find( sBuffer[ i ] ) != std::string::npos )
			sBuffer.insert(i++,"\\");
	}
}
void StringUtil::formatPlayername(char *name)
{
	if( name == NULL || !(*name) ) return;

	*name = toupper(*name);
	for(++name;*name;++name)
		*name = tolower(*name);
}
/* Galnor 09/11/2009 - Escape a va_arg formatted string(%s,%d,%lld) */
const std::string StringUtil::vaEscape( const std::string &str )
{
	std::string Buffer;
	for( std::string::size_type i = 0;i < str.size();++i )
	{
		if( str[i] == '%' )
			Buffer += '%';
		Buffer += str[i];
	}
	return Buffer;
}
const std::string StringUtil::allLower(const std::string &str)
{
	const int len = str.size();
	static std::string s;

	s.erase();

	for(int i = 0;i < len;++i)
		s += tolower(str[i]);
	return s;
}

const char *StringUtil::allLower(const char *str)
{
	const int len = strlen(str);
	static std::string s;

	s.erase();

	for(int i = 0;i < len;++i)
		s += tolower(str[i]);
	return s.c_str();
}

const std::string StringUtil::allUpper(const std::string &str)
{
	const int len = str.size();
	static std::string s;

	s.erase();

	for(int i = 0;i < len;++i)
		s += toupper(str[i]);
	return s;
}

const char *StringUtil::allUpper(const char *str)
{
	const int len = strlen(str);
	static std::string s;

	s.erase();

	for(int i = 0;i < len;++i)
		s += toupper(str[i]);
	return s.c_str();
}

//Return an altered copy capitalized
std::string &StringUtil::cap(const std::string &str)
{
	static std::string sBuf;
	
	sBuf = str;
	if( !sBuf.empty() )
		sBuf[0] = toupper(sBuf[0]);
	return sBuf;
}
const char *StringUtil::cap(const char *str)
{
	return ( cap(std::string(str)).c_str() );
}
/* string manipulation fucntion originally by Darren Wilson */
/* (wilson@shark.cc.cc.ca.us) improved and bug fixed by Chris (zero@cnw.com) */
/* completely re-written again by M. Scott 10/15/96 (scottm@workcommn.net), */
/* substitute appearances of 'pattern' with 'replacement' in string */
/* and return the # of replacements */
int StringUtil::replace(char **str, char *pattern, char *replacement, int rep_all, int max_size)
{
	char *replace_buffer = NULL, *ptr = 0;
	int i;

	if ((strlen(*str) - strlen(pattern)) + strlen(replacement) > (unsigned int) max_size)
		return -1;

	replace_buffer = new char[max_size];
	*replace_buffer = 0;
	//ReplaceBuffer: Hithisiscool NO
	//String: *O canyoufixme?
	//Pattern: NO
	//Replace: Google

	char* front = (*str);
	for(i = 0;ptr = strstr( (front), pattern );++i)
	{
		(*ptr) = 0;
		strcat( replace_buffer, (front) );
		strcat( replace_buffer, replacement );

		(ptr) += strlen(pattern);
		front = ptr;

		if(!rep_all)
		{
			++i;
			break;
		}
	}
	strcat( replace_buffer, (front) );

	if (i > 0)
	{
		if (*str)
			delete[] (*str);

		int size = strlen(replace_buffer);
		*str = new char[size + 3];
		strcpy(*str, replace_buffer);
	}

	delete[] (replace_buffer);
	return i;
}
std::string StringUtil::stripCarriageReturn( const std::string &source )
{
	std::stringstream buffer;

	for(unsigned int index = 0;index < source.size();++index) {
		if( source[index] != '\r' )
			buffer << source[index];
	}
	return buffer.str();
}
std::string StringUtil::yesNo( bool truth )
{
	return truth ? "Yes" : "No";
}
bool StringUtil::parseBoolean( const std::string &boolString )
{
	if (boolString == "1")
		return true;
	else if (boolString == "0")
		return false;
	else if (boolString == " ")
		return false;
	else
		return true;
}

std::string StringUtil::truncateText(const std::string &text, int maxLength, const std::string &endString)
{
	maxLength = (maxLength < 3) ? 3 : maxLength;
	return (text.size() > maxLength)
		? text.substr(0, maxLength - 3) + endString
		: text;
}

std::string StringUtil::truncateWithEllipses(const std::string &text, int maxLength)
{
	return truncateText(text, maxLength, "...");
}
=======
#include <string>
#include <vector>
#include <list>
#include <cstring>

#include "StringUtil.h"
#include "MiscUtil.h"

std::string StringUtil::getRandomString(const int size)
{
	if(size <= 0) {
		return "";
	}

	std::string buffer;
	static char *symbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
	int numberOfSymbols = strlen(symbols);
	for(int i = 0;i < size;++i) {

		buffer += symbols[MiscUtil::random(0,numberOfSymbols-1)];
	}

	return buffer;
}

//2011-02-06 - Replaces string contents based on the value of toFind.
void StringUtil::replace(std::string &input, const std::string &toFind, const std::string &replaceWith)
{
	std::string::size_type pos = 0;
	while(pos < input.size())
	{
		if(input.substr(pos, toFind.size()) == toFind)
		{

			input.replace(pos, toFind.size(), replaceWith);

			pos += replaceWith.size();
		}
		else {

			++pos;
		}
	}
}


//Galnor: 2006-07-26: Rewrote this function.
void StringUtil::trim(std::string &s)
{
	std::string::size_type pos;

	for(pos = 0;pos < s.size() && isspace(s[pos]);++pos);
	s.erase(0, pos);
	for(pos = (0 > (int)s.size() ? 0 : (int)s.size());pos && s[pos - 1] == ' ';--pos);
	s.erase(pos, s.size());
}

/*** Add slashes to escape characters. ***/
void StringUtil::addSlashes( std::string &sBuffer, const std::string &EscChars )
{
	for(unsigned int i = 0;i < sBuffer.size();++i)
	{
		if( EscChars.find( sBuffer[ i ] ) != std::string::npos )
			sBuffer.insert(i++,"\\");
	}
}
void StringUtil::formatPlayername(char *name)
{
	if( name == NULL || !(*name) ) return;

	*name = toupper(*name);
	for(++name;*name;++name)
		*name = tolower(*name);
}
/* Galnor 09/11/2009 - Escape a va_arg formatted string(%s,%d,%lld) */
const std::string StringUtil::vaEscape( const std::string &str )
{
	std::string Buffer;
	for( std::string::size_type i = 0;i < str.size();++i )
	{
		if( str[i] == '%' )
			Buffer += '%';
		Buffer += str[i];
	}
	return Buffer;
}
const std::string StringUtil::allLower(const std::string &str)
{
	const int len = str.size();
	static std::string s;

	s.erase();

	for(int i = 0;i < len;++i)
		s += tolower(str[i]);
	return s;
}

const char *StringUtil::allLower(const char *str)
{
	const int len = strlen(str);
	static std::string s;

	s.erase();

	for(int i = 0;i < len;++i)
		s += tolower(str[i]);
	return s.c_str();
}

const std::string StringUtil::allUpper(const std::string &str)
{
	const int len = str.size();
	static std::string s;

	s.erase();

	for(int i = 0;i < len;++i)
		s += toupper(str[i]);
	return s;
}

const char *StringUtil::allUpper(const char *str)
{
	const int len = strlen(str);
	static std::string s;

	s.erase();

	for(int i = 0;i < len;++i)
		s += toupper(str[i]);
	return s.c_str();
}

//Return an altered copy capitalized
std::string &StringUtil::cap(const std::string &str)
{
	static std::string sBuf;
	
	sBuf = str;
	if( !sBuf.empty() )
		sBuf[0] = toupper(sBuf[0]);
	return sBuf;
}
const char *StringUtil::cap(const char *str)
{
	return ( cap(std::string(str)).c_str() );
}
/* string manipulation fucntion originally by Darren Wilson */
/* (wilson@shark.cc.cc.ca.us) improved and bug fixed by Chris (zero@cnw.com) */
/* completely re-written again by M. Scott 10/15/96 (scottm@workcommn.net), */
/* substitute appearances of 'pattern' with 'replacement' in string */
/* and return the # of replacements */
int StringUtil::replace(char **str, char *pattern, char *replacement, int rep_all, int max_size)
{
	char *replace_buffer = NULL, *ptr = 0;
	int i;

	if ((strlen(*str) - strlen(pattern)) + strlen(replacement) > (unsigned int) max_size)
		return -1;

	replace_buffer = new char[max_size];
	*replace_buffer = 0;
	//ReplaceBuffer: Hithisiscool NO
	//String: *O canyoufixme?
	//Pattern: NO
	//Replace: Google

	char* front = (*str);
	for(i = 0;ptr = strstr( (front), pattern );++i)
	{
		(*ptr) = 0;
		strcat( replace_buffer, (front) );
		strcat( replace_buffer, replacement );

		(ptr) += strlen(pattern);
		front = ptr;

		if(!rep_all)
		{
			++i;
			break;
		}
	}
	strcat( replace_buffer, (front) );

	if (i > 0)
	{
		if (*str)
			delete[] (*str);

		int size = strlen(replace_buffer);
		*str = new char[size + 3];
		strcpy(*str, replace_buffer);
	}

	delete[] (replace_buffer);
	return i;
}
std::string StringUtil::stripCarriageReturn( const std::string &source )
{
	std::stringstream buffer;

	for(unsigned int index = 0;index < source.size();++index) {
		if( source[index] != '\r' )
			buffer << source[index];
	}
	return buffer.str();
}
std::string StringUtil::yesNo( bool truth )
{
	return truth ? "Yes" : "No";
}
bool StringUtil::parseBoolean( const std::string &boolString )
{
	if (boolString == "1")
		return true;
	else if (boolString == "0")
		return false;
	else if (boolString == " ")
		return false;
	else
		return true;
}

std::string StringUtil::truncateText(const std::string &text, int maxLength, const std::string &endString)
{
	maxLength = (maxLength < 3) ? 3 : maxLength;
	return (text.size() > maxLength)
		? text.substr(0, maxLength - 3) + endString
		: text;
}

std::string StringUtil::truncateWithEllipses(const std::string &text, int maxLength)
{
	return truncateText(text, maxLength, "...");
}
>>>>>>> .r11
