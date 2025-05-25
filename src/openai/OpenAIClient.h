#ifndef KINSLAYER_MUD_OPENAICLIENT_H
#define KINSLAYER_MUD_OPENAICLIENT_H

#include <stdexcept>
#include <string>
#include <vector>
#include <httplib.h>

struct OpenAIChatCompletionResult {

	bool wasSuccessful;
	std::string completion;

	OpenAIChatCompletionResult(const bool wasSuccessful, const std::string &completion) {
		this->wasSuccessful = wasSuccessful;
		this->completion = completion;
	}
};

class OpenAIClient {
public:
	explicit OpenAIClient(std::string apiKey) : apiKey_(std::move(apiKey)) {
		this->timeout = 30;
	}

	OpenAIChatCompletionResult chatCompletion(const std::string &model,
							   const std::string &userPrompt,
							   double temperature = 0.7) const;

private:
	std::string apiKey_;
	int timeout;

	std::string generateChatCompletionRequestBody(
			const std::string &model,
			const std::string &userPrompt,
			double temperature
	) const;

	httplib::Headers generateRequestHeaders() const;
};

#endif //KINSLAYER_MUD_OPENAICLIENT_H
