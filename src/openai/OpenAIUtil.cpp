#include "OpenAIUtil.h"
#include "../threaded_jobs.h"
#include <iostream>

class OpenAIThreadedJob : public Job
{
private:
	std::string model;
	std::string prompt;

	OpenAIResponsesResult responsesResult = {false, ""};
	std::function<void(const OpenAIResponsesResult&)> onCompletionCallback;
public:

	OpenAIThreadedJob(
			const std::string &model,
			const std::string &prompt,
			std::function<void(const OpenAIResponsesResult&)> onCompletionCallback)
			: model(model),
			prompt(prompt),
			onCompletionCallback(std::move(onCompletionCallback)) {
	}

	void performRoutine() {
		this->responsesResult = OpenAIUtil::get().getClient()->performResponses(model, prompt);
	}

	void performPostJobRoutine() {
		if(onCompletionCallback) {
			onCompletionCallback(this->responsesResult);
		}
	}
};

OpenAIUtil* OpenAIUtil::self = nullptr;

void OpenAIUtil::performResponses(
		const std::string &model,
		const std::string &userPrompt,
		std::function<void(const OpenAIResponsesResult&)> onCompletionCallback) {
	OpenAIThreadedJob *job = new OpenAIThreadedJob(model, userPrompt, onCompletionCallback);
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