#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <iostream>
#include <typeinfo>
#include <string>
#include <stdexcept>

class Exception : public std::runtime_error
{
public:

	Exception(const std::string &errorMessage) : std::runtime_error(errorMessage) {}
	Exception(const char *errorMessage) : std::runtime_error(errorMessage) {}
private:
};


/***
struct Exception : public std::exception
{
protected:
	char *s;

	void setWhat(const char *s)
	{
		if(!s)
			s = NULL;
		else
		{
			this->s = new char[ strlen(s) + 1 ];
			strcpy(this->s, s);
		}
	}

	void cleanup()
	{
		if(s)
			delete[] s;
	}

public:
	Exception()
	{
		s = NULL;
	}
	Exception(const char *s)
	{
		setWhat(s);
	}

	~Exception() throw()
	{
		cleanup();
	}
	const char* what() const throw() { return s; }
};
**/
#endif
