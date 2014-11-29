#include "EditorInterfaceInstance.h"
#include "EditorInterfaceMenu.h"

#include "../utils.h"
#include "../StringUtil.h"

void EditorInterfaceMenu::print(EditorInterfaceInstance *i)
{
	printOperator(i);
}

void EditorInterfaceMenu::parse(EditorInterfaceInstance *i)
{
	inputOperator(i);
}

boost::optional<std::string> EditorInterfaceMenu::preReq(EditorInterfaceInstance *i)
{
	return preReqOperator.is_initialized() ? preReqOperator.get()(i) : boost::optional<std::string>();
}

void EditorInterfaceMenu::cleanup(EditorInterfaceInstance *i)
{
	if(cleanupOperator.is_initialized())
		cleanupOperator.get()(i);
}

void EditorInterfaceMenu::setPrintOperator(std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> printOperator)
{
	this->printOperator = printOperator;
}

void EditorInterfaceMenu::setParseOperator(std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> inputOperator)
{
	this->inputOperator = inputOperator;
}

void EditorInterfaceMenu::setParseOperator(ValidationFunction validationFunction, std::function<EditorInterfaceMenu *(EditorInterfaceInstance *)> inputOperator)
{
	setParseOperator([=](EditorInterfaceInstance *i) {
		auto errorMessage = validationFunction(i->input);

		if(errorMessage.is_initialized())
		{
			i->send("%s", errorMessage.get().c_str());
			return nullptr;
		}

		inputOperator(i);
		return nullptr;
	});
}

void EditorInterfaceMenu::setPreReqOperator(std::function<boost::optional<std::string>(EditorInterfaceInstance *)> preReqOperator)
{
	this->preReqOperator = preReqOperator;
}

void EditorInterfaceMenu::setCleanupOperator(std::function<void(EditorInterfaceInstance *)> cleanupOperator)
{
	this->cleanupOperator = cleanupOperator;
}
