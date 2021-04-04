#ifndef EDITOR_INTERFACE_MENU_H
#define EDITOR_INTERFACE_MENU_H

#include <optional>
#include <functional>
#include <string>
#include <vector>

class Character;
class Descriptor;
class EditorInterfaceInstance;

typedef std::function<std::optional<std::string>(const std::string &)> ValidationFunction;

#define DEFINE_EI_PRINT_OPERATOR	[=, this](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
#define DEFINE_EI_PARSE_OPERATOR	[=, this](EditorInterfaceInstance *i) -> EditorInterfaceMenu*
#define DEFINE_EI_PRE_REQ_OPERATOR	[=, this](EditorInterfaceInstance *i) -> std::optional<std::string>
#define DEFINE_EI_CLEANUP_OPERATOR	[=, this](EditorInterfaceInstance *i) -> void

class EditorInterfaceMenu
{
private:
protected:
	std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> printOperator;
	std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> inputOperator;
	std::optional<std::function<std::optional<std::string>(EditorInterfaceInstance *)>> preReqOperator;
	std::optional<std::function<void(EditorInterfaceInstance *)>> cleanupOperator;
public:

	void print(EditorInterfaceInstance *i);
	void parse(EditorInterfaceInstance *i);
	std::optional<std::string> preReq(EditorInterfaceInstance *i);
	bool preReqNoPrint(EditorInterfaceInstance *i);
	void cleanup(EditorInterfaceInstance *i);

	void setPrintOperator(std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> printOperator);
	void setParseOperator(std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> inputOperator);
	void setParseOperator(ValidationFunction validationFunction, std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> inputOperator);
	void setPreReqOperator(std::function<std::optional<std::string>(EditorInterfaceInstance *)> preReqOperator);
	void setCleanupOperator(std::function<void(EditorInterfaceInstance *)> cleanupOperator);

	
};

#endif