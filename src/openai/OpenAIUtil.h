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
	void performResponses(
			const std::string &model,
			const std::string &prompt,
			std::function<void(const OpenAIResponsesResult&)> onCompletionCallback);

private:
	OpenAIUtil();
	~OpenAIUtil();

	OpenAIClient *client;

	static OpenAIUtil* self;
};

#endif //KINSLAYER_MUD_OPENAIUTIL_H
