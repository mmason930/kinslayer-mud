#ifndef EDITOR_INTERFACE_INSTANCE_H
#define EDITOR_INTERFACE_INSTANCE_H

#include <list>
#include <vector>
#include <string>
#include "EditorInterfaceData.h"

class EditorInterfaceMenu;
class EditorInterfaceData;

class Character;

class EditorInterfaceInstance
{
private:
protected:
	std::list< EditorInterfaceMenu * > menuStack;
	bool isTerminated;
	EditorInterfaceData *data;
	std::string invalidOptionMessage;
public:

	EditorInterfaceInstance(Character *ch, EditorInterfaceData *data);
	~EditorInterfaceInstance();
	
	Character *ch;

	std::string input;
	char firstLetter;
	std::vector<std::string> inputVector;

	EditorInterfaceMenu *push(EditorInterfaceMenu *menu);
	EditorInterfaceMenu *pushAndDisplay(EditorInterfaceMenu *menu);

	EditorInterfaceMenu *pop();
	EditorInterfaceMenu *popAndDisplay();

	EditorInterfaceMenu *terminate();
	EditorInterfaceMenu *terminateWithError();

	EditorInterfaceMenu *invalidOption();

	std::string getInvalidOptionMessage() const;

	EditorInterfaceMenu *parse(const std::string &input, const char firstCharacter, const std::vector<std::string> inputVector);
	EditorInterfaceMenu *print();
	EditorInterfaceMenu *send(const std::string &message, ...);
	EditorInterfaceMenu *send(const char *message, ...);

	bool getIsTerminated() const;

	void parse(const std::string &command);
	void destroy();

	int getUserId() const;

	std::list<EditorInterfaceMenu*>::size_type getMenuStackSize() const;

	EditorInterfaceData *getData() const;
};

#endif