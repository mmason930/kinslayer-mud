#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <iostream>
#include <typeinfo>
#include <string>

class Exception : public std::exception
{
public:

   explicit Exception(const std::string& msg)
      : msg_(msg)
   {}

   virtual ~Exception() throw() {}

   virtual const char* what() const throw()
   {
      return msg_.c_str();
   }

private:
   std::string msg_;
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
