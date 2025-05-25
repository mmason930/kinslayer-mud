#include "OpenAIUtil.h"
#include "../threaded_jobs.h"
#include <iostream>

class OpenAIThreadedJob : public Job
{
private:
	std::string systemPrompt;
	std::string userPrompt;

	OpenAIChatCompletionResult completionResult = {false, ""};
public:

	OpenAIThreadedJob(const std::string &systemPrompt, const std::string &userPrompt) {
		this->systemPrompt = systemPrompt;
		this->userPrompt = userPrompt;
	}

	void performRoutine() {

		std::string model = "o4-mini";
		this->completionResult = OpenAIUtil::get().getClient()->chatCompletion(model, userPrompt);
	}

	void performPostJobRoutine() {
		std::cout << "Chat Completion: " << this->completionResult.completion.c_str() << std::endl;
	}
};

OpenAIUtil* OpenAIUtil::self = nullptr;

void OpenAIUtil::performChatCompletion(const std::string &systemPrompt, const std::string &userPrompt) {
	OpenAIThreadedJob *job = new OpenAIThreadedJob(systemPrompt, userPrompt);
	ThreadedJobManager::get().addJob(job);
}

OpenAIUtil& OpenAIUtil::get() {
	if(!self) {
		self = new OpenAIUtil();
	}
	return (*self);
}

OpenAIUtil::OpenAIUtil() {
	this->client = nullptr;
}

OpenAIUtil::~OpenAIUtil() {
	self = nullptr;

	if(this->client) {
		delete this->client;
	}
}
void OpenAIUtil::free()
{
	if( self != nullptr ) {
		delete (self);
	}
}

void OpenAIUtil::loadClient(const std::string &apiKey) {
	this->client = new OpenAIClient(apiKey);
}

OpenAIClient *OpenAIUtil::getClient() {
	return this->client;
}