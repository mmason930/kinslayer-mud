#include "conf.h"
#include "sysdep.h"

#include <thread>
#include <httplib.h>

#include "jsoncpp/json.h"
#include "MailUtil.h"
#include "StringUtil.h"

std::optional<std::string> MailUtil::sendgridApiKey = std::optional<std::string>();

void MailUtil::send(std::string fromEmailAddress, std::string fromDisplayName, std::string toEmailAddress, std::string subject, std::string message)
{
	if(!sendgridApiKey.has_value())
	{// No API key configured, so email sending isn't possible.
		return;
	}

	Json::FastWriter writer;
	Json::Value postBodyObject;
	
	Json::Value replyToObject;
	replyToObject["email"] = fromEmailAddress;
	replyToObject["name"] = fromDisplayName;
	postBodyObject["reply_to"] = replyToObject;

	Json::Value fromObject;
	fromObject["email"] = fromEmailAddress;
	fromObject["name"] = fromDisplayName;
	postBodyObject["from"] = fromObject;

	Json::Value contentArray = Json::Value(Json::arrayValue);
	contentArray[0] = Json::Value();
	contentArray[0]["type"] = "text/html";
	contentArray[0]["value"] = message;
	postBodyObject["content"] = contentArray;

	Json::Value personalizationArray = Json::Value(Json::arrayValue);
	personalizationArray[0] = Json::Value();
	personalizationArray[0]["to"] = Json::Value(Json::arrayValue);
	personalizationArray[0]["to"][0] = Json::Value();
	personalizationArray[0]["to"][0]["email"] = toEmailAddress;
	personalizationArray[0]["subject"] = subject;
	postBodyObject["personalizations"] = personalizationArray;

	std::string postBody = writer.write(postBodyObject);
	std::string bearer = std::string("Bearer ") + (*sendgridApiKey);

	httplib::Headers headers = {
		{ "Authorization", bearer.c_str() }
	};
	httplib::Client cli("https://api.sendgrid.com");
	auto response = cli.Post("/v3/mail/send", headers, postBody, "application/json");
}

void MailUtil::sendEmail(const std::string &fromEmailAddress, const std::string &fromDisplayName, const std::string &toEmailAddress, const std::string &subject, const std::string &message)
{
	std::thread thread(&MailUtil::send, fromEmailAddress, fromDisplayName, toEmailAddress, subject, message);

	thread.detach();
}

void MailUtil::loadConfig(const Game *game)
{
	if(game->hasBasicConfiguration("Sendgrid API Key"))
	{
		sendgridApiKey = std::optional<std::string>(game->getBasicConfigValue("Sendgrid API Key"));
	}
}