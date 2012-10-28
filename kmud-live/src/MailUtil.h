#ifndef MAIL_UTIL_H
#define MAIL_UTIL_H

class MailUtil
{
public:
	static void sendEmail(const std::string &fromEmailAddress, const std::string &fromDisplayName, const std::string &toEmailAddress, const std::string &subject, const std::string &message);
	static void sendEmail(std::string smtpName, short int smtpPort, std::string fromEmailAddress, std::string fromDisplayName, std::string toEmailAddress, std::string subject, std::string message);
};

#endif
