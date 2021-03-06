#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <sstream>
#include <vector>
#include <list>
#include <map>

class StringUtil
{
protected:
private:
public:

	static void trim(std::string &s);
	/*** Galnor 03/13/2010 - Join a container of string-convertible elements into a single buffer ***/
	template< typename _T >
	static std::string implode( const _T &_Container, const std::string &_Separator )
	{
		std::string sBuffer;
		std::stringstream stream;
		for( typename _T::const_iterator i = _Container.begin(); i != _Container.end();++i )
		{
			if( sBuffer.empty() == false )
				sBuffer += _Separator;
			stream.str("");
			stream << (*i);
			sBuffer += stream.str();
		}
		return sBuffer;
	}

	static bool startsWith(const std::string &stringBuffer, const std::string &str);
	static std::string SHA1(const std::string &inputBuffer);
	static std::string SHA1Hex(const std::string &inputBuffer);
	static std::string base64Encode(const std::string &inputBuffer);
	static std::string base64Decode(const std::string &inputBuffer);
	static void addSlashes( std::string &sBuffer, const std::string &EscChars );
	static void formatPlayername(char *name);
	static const std::string vaEscape( const std::string &str );
	static const std::string allLower(const std::string &str);
	static const char *allLower(const char *str);
	static const std::string allUpper(const std::string &str);
	static const char *allUpper(const char *str);
	static std::string &cap(const std::string &str);
	static const char *cap(const char *str);
	static int replace(char **str, char *pattern, char *replacement, int rep_all, int max_size);
	static std::string stripCarriageReturn( const std::string &source );
	static std::string yesNo( bool truth );
	static std::string truncateText(const std::string &text, int maxLength, const std::string &endString);
	static std::string truncateWithEllipses(const std::string &text, int maxLength);
	static bool parseBoolean( const std::string &boolString );
	static void replace(std::string &input, const std::string &toFind, const std::string &replaceWith);
	static std::string getRandomString(const int size);
	static std::string getLine(std::string &inputBuffer, const std::string &delim);
	static int strlenIgnoreColors( const char* str );
	static std::string repeat(const std::string str, int numberOfTimes);

	template<typename T>
	static std::string plural(const T &t)
	{
		return t == 1 ? std::string("") : std::string("s");
	}

	//Galnor: 2006-12-11
	//Re-wrote the split_to_vector and split_to_list to allow for all std containers
	//to use this using a template as its representation.
	template< class _Container >
	static _Container splitToContainer( const char *s, const char delim)
	{
		_Container v;
		std::string currentWord;

		for(const char *currentToken = s;*currentToken;++currentToken)
		{
			if((*currentToken) == delim)
			{
				if (!currentWord.empty())
				{
					v.push_back(currentWord);
					currentWord.clear();
				}
			}
			else
				currentWord.append(currentToken, 1);
		}
		
		if (!currentWord.empty())
			v.push_back(currentWord);

		return v;
	}
	
	template< class _Container >
	static _Container splitToContainer( const std::string &s, const char delim)
	{
		return splitToContainer<_Container>(s.c_str(), delim);
	}

	//Galnor: 2010-06-01
	//Pass a string and a delimiter and have a vector returned with the string split by that delim.
	//Each splicing point is entered into the vector as individual nodes.
	static std::vector< std::string > splitToVector( const std::string &s, const char delim = ',')
	{
		return splitToContainer< std::vector< std::string > >( s, delim );
	}

	static std::list< std::string > splitToList( const std::string &s, const char delim = ',' )
	{
		return splitToContainer< std::list< std::string > >( s, delim );
	}

	static std::vector< std::string > getArgVector( const std::string &s )
	{
		std::vector< std::string > vArgs;

		std::string::const_iterator iter;
		for(iter = s.begin();iter != s.end();++iter)
		{
			const char ch = (*iter);
			if( ch == ' ' )
			{
				if( vArgs.empty() == false && vArgs.back().size() > 0 )
				{
					vArgs.resize( vArgs.size() + 1 );
				}
			}
			else
			{
				if( vArgs.empty() == true )
					vArgs.resize( vArgs.size() + 1 );
				vArgs.back() += ch;
			}
		}

		if( vArgs.empty() == false && vArgs.back().empty() == true )
			vArgs.pop_back();

		return vArgs;
	}

};

#endif
