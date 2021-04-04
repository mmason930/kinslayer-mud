#ifndef EDITOR_INTERFACE_BASE_H
#define EDITOR_INTERFACE_BASE_H

/*****************************************************************
 *                                                               *
 * EditorInterfaceBase : Holds the bulk of the functionality     *
 * common to all editor interfaces. It is necessary to keep the  *
 * functionality here instead of EditorInterface itself because  *
 * EditorInterface is a template class. Were we to move the      *
 * common functionality there, we would need to define it all in *
 * the header file. Instead, declare it here, define it in       *
 * the .cpp file, and have EditorInterface extend this class.    *
 *                                                               *
 *****************************************************************/

#include <list>
#include <initializer_list>
#include <optional>
#include <functional>

typedef std::function<std::optional<std::string>(const std::string &)> ValidationFunction;

class EditorInterfaceInstance;
class EditorInterfaceData;
class EditorInterfaceMenu;

class EditorInterfaceBase
{
private:
protected:
	std::list< EditorInterfaceMenu * > editorInterfaceMenues;
	EditorInterfaceMenu *mainMenu;
public:
	EditorInterfaceBase();
	
	virtual EditorInterfaceData *createData() const = 0;

	void setMainMenu(EditorInterfaceMenu *editorInterfaceMenu);
	EditorInterfaceMenu *getMainMenu();

	EditorInterfaceMenu *addMenu(EditorInterfaceMenu *menu);
	EditorInterfaceMenu *addNewMenu();

	void setupNewInstance(Character *ch) const;
	EditorInterfaceMenu *createValueInputMenu(
		const std::string &message,
		const std::function<std::optional<std::string>(const std::string &)> &validationOperation,
		const std::function<void(EditorInterfaceInstance *i)> &dataSetOperation
	);
	
	ValidationFunction validationChain(const std::initializer_list<ValidationFunction> &validationList);
	ValidationFunction inputLengthValidator(const int minimumLength, const int maximumLength, const std::string &errorMessage);
	ValidationFunction integerValidator();
	ValidationFunction integerValidator(const std::string &errorMessage);
};

#endif
