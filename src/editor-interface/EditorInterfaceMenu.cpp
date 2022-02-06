#include "EditorInterfaceInstance.h"
#include "EditorInterfaceMenu.h"


void EditorInterfaceMenu::print(EditorInterfaceInstance *i)
{
	printOperator(i);
}

void EditorInterfaceMenu::parse(EditorInterfaceInstance *i)
{
	inputOperator(i);
}

std::optional<std::string> EditorInterfaceMenu::preReq(EditorInterfaceInstance *i)
{
	return preReqOperator.has_value() ? preReqOperator.value()(i) : std::optional<std::string>();
}

bool EditorInterfaceMenu::preReqNoPrint(EditorInterfaceInstance *i)
{
	return preReqOperator ? !preReqOperator.value()(i) : true;
}

void EditorInterfaceMenu::cleanup(EditorInterfaceInstance *i)
{
	if(cleanupOperator.has_value())
		cleanupOperator.value()(i);
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

		if(errorMessage.has_value())
		{
			i->send("%s", errorMessage.value().c_str());
			return nullptr;
		}

		inputOperator(i);
		return nullptr;
	});
}

void EditorInterfaceMenu::setPreReqOperator(std::function<std::optional<std::string>(EditorInterfaceInstance *)> preReqOperator)
{
	this->preReqOperator = preReqOperator;
}

void EditorInterfaceMenu::setCleanupOperator(std::function<void(EditorInterfaceInstance *)> cleanupOperator)
{
	this->cleanupOperator = cleanupOperator;
}
