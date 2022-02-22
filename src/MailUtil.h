#ifndef MAIL_UTIL_H
#define MAIL_UTIL_H

#include "Game.h"

class MailUtil
{
private:
	static std::optional<std::string> sendgridApiKey;
public:
	static void sendEmail(const std::string &fromEmailAddress, const std::string &fromDisplayName, const std::string &toEmailAddress, const std::string &subject, const std::string &message);
	static void send(std::string fromEmailAddress, std::string fromDisplayName, std::string toEmailAddress, std::string subject, std::string message);
	static void loadConfig(const Game *game);
};

#endif
