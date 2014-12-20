#ifndef COMMAND_UTIL_H
#define COMMAND_UTIL_H

#include <map>
#include <vector>
#include <string>
#include <functional>

#include <mysql/sqlDatabase.h>

typedef std::function<void(Character *ch, char *argument, int cmd, int subcmd)>		CommandHandler;

class Character;
class CommandInfo;
class Social;

class CommandUtil
{
private:

	static CommandUtil *self;

	CommandUtil();
	~CommandUtil();
protected:
	std::map<std::string, CommandInfo*> commandMap;
	std::vector<CommandInfo *> commandInfoVector;

	std::vector<Social *> socialVector;
	std::map<int, Social *> commandIndexToSocialMap;

public:

	static CommandUtil *get();
	void destroy();

	CommandInfo *getCommandByFullInput(const std::string &fullCommandInput);
	CommandInfo *getCommandByName(const std::string &commandName);
	CommandInfo *getCommandByIndex(const int index);
	int getCommandIndex(const std::string &command);
	int getCommandIndex();
	void interpretCommand(Character *ch, char *argument);
	void addCommand(CommandInfo *commandInfo);
	std::vector<CommandInfo *> getCommandVector();

	void addSocial(Social *social);
	void sortSocials();
	Social *getSocial(int index);
	std::vector<Social *> getSocialVector();
	Social *getSocialByCommandIndex(int commandIndex);
	int getNumberOfSocials();
	Social *getSocialByAbbreviation(const std::string &input);

	void bootSocials(sql::Connection connection);
	void saveSocials(sql::Connection connection);

	void sortCommands();
	void createCommandList();
	std::vector<CommandInfo *>::iterator removeAndDeleteCommand(int index);
};

#endif