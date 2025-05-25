#ifndef KINSLAYER_MUD_OPENAICLIENT_H
#define KINSLAYER_MUD_OPENAICLIENT_H

#include <stdexcept>
#include <string>
#include <vector>
#include <httplib.h>

struct OpenAIResponsesResult {

	bool wasSuccessful;
	std::string responses;

	OpenAIResponsesResult(const bool wasSuccessful, const std::string &responses) {
		this->wasSuccessful = wasSuccessful;
		this->responses = responses;
	}
};

class OpenAIClient {
public:
	explicit OpenAIClient(std::string apiKey) : apiKey_(std::move(apiKey)) {
		this->timeout = 30;
	}

	OpenAIResponsesResult performResponses(const std::string &model,
							   const std::string &userPrompt,
							   double temperature = 0.7) const;

private:
	std::string apiKey_;
	int timeout;

	std::string generateResponsesRequestBody(
			const std::string &model,
			const std::string &userPrompt,
			double temperature
	) const;

	std::string extractAssistantText(const std::string& responseBody) const;

	httplib::Headers generateRequestHeaders() const;
};

#endif //KINSLAYER_MUD_OPENAICLIENT_H
