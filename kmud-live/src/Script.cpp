#include "conf.h"

#include "Script.h"


Script::Script()
{
	id = -1;
}

Script::Script(const int id, const std::string &methodName)
{
	setId(id);
	setMethodName(methodName);
}

Script::~Script()
{}

std::string Script::getMethodName() const
{
	return methodName;
}

void Script::setMethodName(const std::string &methodName)
{
	this->methodName = methodName;
}
