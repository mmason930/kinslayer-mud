#include "../conf.h"
#include "../sysdep.h"
#include "../structs.h"
#include "../Descriptor.h"
#include "../screen.h"
#include "../olc.h"
#include "../StringUtil.h"
#include "../utils.h"

#include <cstdarg>

#include "EditorInterfaceInstance.h"

EditorInterfaceMenu *EditorInterfaceInstance::push(EditorInterfaceMenu *menu)
{
	boost::optional<std::string> preReqMessage = menu->preReq(this);

	if(preReqMessage.is_initialized())
		send("%s\r\n\r\n", preReqMessage.get().c_str());
	else
		menuStack.push_back(menu);
	return menu;
}

EditorInterfaceMenu *EditorInterfaceInstance::pop()
{
	EditorInterfaceMenu *menu = menuStack.back();
	menuStack.pop_back();
	menu->cleanup(this);
	return menu;
}

EditorInterfaceMenu *EditorInterfaceInstance::terminate()
{
	isTerminated = true;
	return nullptr;
}

void EditorInterfaceInstance::destroy()
{
	ch->editorInterfaceInstance = nullptr;
}

EditorInterfaceMenu *EditorInterfaceInstance::parse(const std::string &input, const char firstCharacter, const std::vector<std::string> inputVector)
{
	this->input = input;
	this->firstLetter = firstCharacter;
	this->inputVector = inputVector;

	menuStack.back()->parse(this);

	this->input.clear();
	this->firstLetter = '\0';
	this->inputVector.clear();

	return nullptr;
}

EditorInterfaceMenu *EditorInterfaceInstance::print()
{
	get_char_cols(ch);
	EditorInterfaceMenu *menu = menuStack.back();

	menuStack.back()->print(this);
	return nullptr;
}

EditorInterfaceMenu *EditorInterfaceInstance::pushAndDisplay(EditorInterfaceMenu *menu)
{
	push(menu);
	return print();
}

EditorInterfaceMenu *EditorInterfaceInstance::popAndDisplay()
{
	EditorInterfaceMenu *poppedMenu = pop();

	if(menuStack.empty())
		terminate();
	else
		print();

	return poppedMenu;
}

EditorInterfaceMenu *EditorInterfaceInstance::invalidOption()
{
	return send("Invalid option.\r\nTry again:");
}

EditorInterfaceMenu *EditorInterfaceInstance::send(const std::string &message, ...)
{
	va_list args;

	va_start( args, message );
	send(message.c_str(), args);
	va_end( args );

	return nullptr;
}

EditorInterfaceMenu *EditorInterfaceInstance::send(const char *message, ...)
{
	va_list args;

	va_start( args, message );
	ch->send(message, args);
	va_end( args );

	return nullptr;
}

void EditorInterfaceInstance::parse(const std::string &command)
{
	std::string inputMutable = command;

	StringUtil::trim(inputMutable);

	char firstCharacter = inputMutable.empty() ? '\0' : (char)toupper(inputMutable[0]);

	std::vector<std::string> inputVector = StringUtil::SplitToVector(inputMutable, ' ');

	parse(inputMutable, firstCharacter, inputVector);

	if(isTerminated)
		destroy();
}

EditorInterfaceInstance::EditorInterfaceInstance(Character *ch, EditorInterfaceData *data)
{
	this->ch = ch;
	this->data = data;
	firstLetter = '\0';
	ch->editorInterfaceInstance.reset(this);
	isTerminated = false;
}

EditorInterfaceInstance::~EditorInterfaceInstance()
{
	if(data)
		delete data;
}

bool EditorInterfaceInstance::getIsTerminated() const
{
	return isTerminated;
}

int EditorInterfaceInstance::getUserId() const
{
	return ch->player.idnum;
}

std::list<EditorInterfaceMenu*>::size_type EditorInterfaceInstance::getMenuStackSize() const
{
	return menuStack.size();
}

EditorInterfaceData *EditorInterfaceInstance::getData() const
{
	return data;
}
