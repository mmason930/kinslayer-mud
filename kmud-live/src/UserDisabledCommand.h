#ifndef USER_DISABLED_COMMAND_H
#define USER_DISABLED_COMMAND_H

#include "DateTime.h"
#include "DataObjectWithIntId.h"
#include <string>

class UserDisabledCommand : public DataObjectWithIntId
{
private:
	int userId;
	int disabledByUserId;
	std::string command;
	DateTime createdDatetime;
public:

	UserDisabledCommand() {
		id = -1;
	}

	int getId() {
		return this->id;
	}

	void setId(int id) {

		this->id = id;
	}

	int getUserId() {

		return this->userId;
	}

	void setUserId(int userId) {

		this->userId = userId;
	}

	int getDisabledByUserId() {

		return this->disabledByUserId;
	}

	void setDisabledByUserId(int disabledByUserId) {

		this->disabledByUserId = disabledByUserId;
	}

	std::string getCommand() {

		return command;
	}

	void setCommand(const std::string &command) {

		this->command = command;
	}

	DateTime getCreatedDatetime() {

		return createdDatetime;
	}

	void setCreatedDatetime(const DateTime &createdDatetime) {

		this->createdDatetime = createdDatetime;
	}
};

#endif