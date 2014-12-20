#include "../conf.h"
#include "../sysdep.h"

#include "../structs.h"
#include "../screen.h"
#include "../utils.h"

#include "EditorInterfaceBase.h"
#include "EditorInterfaceMenu.h"
#include "EditorInterfaceData.h"
#include "EditorInterfaceInstance.h"
#include "EditorInterface.h"

EditorInterfaceBase::EditorInterfaceBase()
{
	mainMenu = nullptr;
}

void EditorInterfaceBase::setMainMenu(EditorInterfaceMenu *editorInterfaceMenu)
{
	this->mainMenu = editorInterfaceMenu;
}

EditorInterfaceMenu *EditorInterfaceBase::getMainMenu()
{
	return mainMenu;
}

EditorInterfaceMenu *EditorInterfaceBase::addMenu(EditorInterfaceMenu *menu)
{
	editorInterfaceMenues.push_back(menu);
	return menu;
}

EditorInterfaceMenu *EditorInterfaceBase::addNewMenu()
{
	EditorInterfaceMenu *menu = new EditorInterfaceMenu();
	addMenu(menu);
	return menu;
}

void EditorInterfaceBase::setupNewInstance(Character *ch) const
{
	if(!mainMenu)
	{
		MudLog(BRF, MAX(GET_INVIS_LEV(ch), LVL_APPR), TRUE, "%s attempted to enter an editor interface with no main menu. Aborting.", GET_NAME(ch));
		return;
	}

	EditorInterfaceInstance *editorInterfaceInstance = new EditorInterfaceInstance(ch, createData());
	editorInterfaceInstance->pushAndDisplay(mainMenu);

	//Handle the scenario where the user is not permitted to view the main menu. The stack size would be 0 in this case.

	if(editorInterfaceInstance->getMenuStackSize() == 0)
		editorInterfaceInstance->destroy();
}

EditorInterfaceMenu *EditorInterfaceBase::createValueInputMenu(
		const std::string &message,
		const std::function<boost::optional<std::string>(const std::string &)> &validationOperation,
		const std::function<void(EditorInterfaceInstance *i)> &dataSetOperation
)
{
	EditorInterfaceMenu *menu = new EditorInterfaceMenu();

	menu->setPrintOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu *
		{
			return i->send("%s", message.c_str());
		}
	);

	menu->setParseOperator(
		[=](EditorInterfaceInstance *i) -> EditorInterfaceMenu *
		{
			boost::optional<std::string> errorMessage = validationOperation(i->input);

			if(errorMessage.is_initialized())
				i->send("%s\r\n", errorMessage.get().c_str());
			
			dataSetOperation(i);

			return i->popAndDisplay();
		}
	);

	return menu;
}

ValidationFunction EditorInterfaceBase::validationChain(const std::initializer_list<ValidationFunction> &validationList)
{
	return [=](const std::string &input) -> boost::optional<std::string>
	{
		for(ValidationFunction validationFunction : validationList)
		{
			auto errorMessage = validationFunction(input);

			if(errorMessage.is_initialized())
				return errorMessage.get();
		}

		return boost::optional<std::string>();
	};
}

ValidationFunction EditorInterfaceBase::inputLengthValidator(const int minimumLength, const int maximumLength, const std::string &errorMessage)
{
	return [=](const std::string &input)
	{
		auto length = input.size();
		return length < minimumLength || length > maximumLength ? errorMessage : boost::optional<std::string>();
	};
}

ValidationFunction EditorInterfaceBase::integerValidator()
{
	return integerValidator(std::string("Invalid input.\r\nPlease enter a valid integer: "));
}

ValidationFunction EditorInterfaceBase::integerValidator(const std::string &errorMessage)
{
	return [=](const std::string &input) { return MiscUtil::isInt(input) ? boost::optional<std::string>() : errorMessage; };
}