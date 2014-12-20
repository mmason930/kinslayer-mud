#ifndef COMMAND_INFO_H
#define COMMAND_INFO_H

#include <string>

typedef std::function<void(Character *ch, char *argument, int cmd, int subcmd)>		CommandHandler;

class CommandInfo
{
public:
	CommandInfo(const std::string &command, const std::string &sortAs, const short minimumPosition, const CommandHandler &commandPointer, const short minimumLevel, const short subCommand, const float timer, const short viewMinimumLevel)
	{
		this->command = command;
		this->sort_as = sortAs;
		this->minimum_position = minimumPosition;
		this->command_pointer = commandPointer;
		this->minimum_level = minimumLevel;
		this->subcmd = subCommand;
		this->timer = timer;
		this->view_minimum_level = viewMinimumLevel;
		this->is_social = false;
	}

	std::string command;
	std::string sort_as;
	short minimum_position;
	CommandHandler command_pointer;
	short minimum_level;
	short subcmd;
	float timer;
	short view_minimum_level;
	bool is_social;
};

#endif