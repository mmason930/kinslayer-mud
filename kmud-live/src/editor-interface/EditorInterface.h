#ifndef EDITOR_INTERFACE_H
#define EDITOR_INTERFACE_H

#include <list>
#include <initializer_list>
#include <boost/optional.hpp>
#include <functional>

typedef std::function<boost::optional<std::string>(const std::string &)> ValidationFunction;

#include "EditorInterfaceMenu.h"
#include "EditorInterfaceInstance.h"
#include "EditorInterfaceData.h"

class EditorInterface
{
private:
protected:
	std::list< EditorInterfaceMenu * > editorInterfaceMenues;
	EditorInterfaceMenu *mainMenu;
public:

	EditorInterface();

	void setMainMenu(EditorInterfaceMenu *editorInterfaceMenu);
	EditorInterfaceMenu *getMainMenu();

	EditorInterfaceMenu *addMenu(EditorInterfaceMenu *menu);
	EditorInterfaceMenu *addNewMenu();

	void setupNewInstance(Character *ch) const;

	virtual EditorInterfaceData *createData() const = 0;

	EditorInterfaceMenu *createValueInputMenu(
		const std::string &message,
		const std::function<boost::optional<std::string>(const std::string &)> &validationOperation,
		const std::function<void(EditorInterfaceInstance *i)> &dataSetOperation
	);
	
	ValidationFunction validationChain(const std::initializer_list<ValidationFunction> &validationList);
	ValidationFunction inputLengthValidator(const int minimumLength, const int maximumLength, const std::string &errorMessage);
	ValidationFunction integerValidator();
	ValidationFunction integerValidator(const std::string &errorMessage);
};

#endif