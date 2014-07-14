#ifndef JS_SCRIPT_H
#define JS_SCRIPT_H

#include <string>

#include "DataObjectWithIntId.h"

class Script : public DataObjectWithIntId
{
private:
protected:

	std::string methodName;

public:

	Script();
	Script(const int id, const std::string &methodName);
	~Script();

	std::string getMethodName() const;
	void setMethodName(const std::string &methodName);
};

#endif