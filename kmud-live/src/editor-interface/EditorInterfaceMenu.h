#ifndef EDITOR_INTERFACE_MENU_H
#define EDITOR_INTERFACE_MENU_H

#include <boost/optional.hpp>
#include <functional>
#include <string>
#include <vector>

class Character;
class Descriptor;
class EditorInterfaceInstance;

typedef std::function<boost::optional<std::string>(const std::string &)> ValidationFunction;

class EditorInterfaceMenu
{
private:
protected:
	std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> printOperator;
	std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> inputOperator;
	boost::optional<std::function<boost::optional<std::string>(EditorInterfaceInstance *)>> preReqOperator;
	boost::optional<std::function<void(EditorInterfaceInstance *)>> cleanupOperator;
public:

	void print(EditorInterfaceInstance *i);
	void parse(EditorInterfaceInstance *i);
	boost::optional<std::string> preReq(EditorInterfaceInstance *i);
	bool preReqNoPrint(EditorInterfaceInstance *i);
	void cleanup(EditorInterfaceInstance *i);

	void setPrintOperator(std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> printOperator);
	void setParseOperator(std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> inputOperator);
	void setParseOperator(ValidationFunction validationFunction, std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> inputOperator);
	void setPreReqOperator(std::function<boost::optional<std::string>(EditorInterfaceInstance *)> preReqOperator);
	void setCleanupOperator(std::function<void(EditorInterfaceInstance *)> cleanupOperator);

	
};

#endif