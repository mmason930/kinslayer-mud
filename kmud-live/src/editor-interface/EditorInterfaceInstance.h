#ifndef EDITOR_INTERFACE_INSTANCE_H
#define EDITOR_INTERFACE_INSTANCE_H

#include <list>
#include <vector>

#include "EditorInterface.h"
#include "EditorInterfaceMenu.h"
#include "EditorInterfaceData.h"

class Character;

class EditorInterfaceInstance
{
private:
protected:
	std::list< EditorInterfaceMenu * > menuStack;
	bool isTerminated;
	EditorInterfaceData *data;
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

	EditorInterfaceMenu *invalidOption();

	EditorInterfaceMenu *parse(const std::string &input, const char firstCharacter, const std::vector<std::string> inputVector);
	EditorInterfaceMenu *print();
	EditorInterfaceMenu *send(const std::string &message, ...);
	EditorInterfaceMenu *send(const char *message, ...);

	bool getIsTerminated();

	void parse(const std::string &command);
	void destroy();

	std::list<EditorInterfaceMenu*>::size_type getMenuStackSize() const;

	EditorInterfaceData *getData() const;
};

#endif