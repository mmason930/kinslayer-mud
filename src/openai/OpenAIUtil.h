#ifndef KINSLAYER_MUD_OPENAIUTIL_H
#define KINSLAYER_MUD_OPENAIUTIL_H

#include "OpenAIClient.h"

class OpenAIUtil
{
public:
	static OpenAIUtil& get();

	void loadClient(const std::string &apiKey);
	OpenAIClient *getClient();
	void free();
	void performChatCompletion(const std::string &systemPrompt, const std::string &userPrompt);

private:
	OpenAIUtil();
	~OpenAIUtil();

	OpenAIClient *client;

	static OpenAIUtil* self;
};

#endif //KINSLAYER_MUD_OPENAIUTIL_H
