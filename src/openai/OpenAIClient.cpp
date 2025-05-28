#include <httplib.h>
#include "OpenAIClient.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../jsoncpp/json.h"

OpenAIResponsesResult OpenAIClient::performResponses(
		const std::string& model,
		const std::string& userPrompt,
		double temperature
		) const
{
	namespace asio  = boost::asio;
	namespace beast = boost::beast;
	namespace http  = beast::http;
	using     tcp   = asio::ip::tcp;

	const std::string body    = generateResponsesRequestBody(model, userPrompt, temperature);
	auto              headers = generateRequestHeaders();

	try {
		asio::io_context ioc;
		asio::ssl::context ssl_ctx{asio::ssl::context::tls_client};
		beast::ssl_stream <beast::tcp_stream> stream(ioc, ssl_ctx);

		SSL *ssl = stream.native_handle();
		if (!SSL_set_tlsext_host_name(ssl, "api.openai.com")) {
			throw boost::system::system_error(
					boost::system::error_code(static_cast<int>(::ERR_get_error()),
											  asio::error::get_ssl_category()),
					"Failed to set SNI host name");
		}

		tcp::resolver resolver{ioc};
		beast::get_lowest_layer(stream).connect(resolver.resolve("api.openai.com", "443"));
		stream.handshake(asio::ssl::stream_base::client);

		http::request <http::string_body> req{http::verb::post, "/v1/responses", 11};
		req.body() = body;
		req.prepare_payload();
		for (auto &[k, v]: headers) {
			req.set(k, v);
		}
		beast::get_lowest_layer(stream).expires_after(std::chrono::seconds(timeout));
		http::write(stream, req);

		beast::flat_buffer buffer;
		http::response <http::string_body> res;
		http::read(stream, buffer, res);

		beast::error_code ec;
		stream.shutdown(ec);
		if (ec == asio::ssl::error::stream_truncated) {
			ec.clear();
		}

		if (ec && ec != beast::errc::not_connected) {
			throw boost::beast::system_error{ec};
		}

		std::cout << "Response: " << res << std::endl;

		if (res.result() != http::status::ok) {
			return {false, ""};
		}

		std::string responseBody = res.body();
		std::string responses = this->extractAssistantText(responseBody);
		return {true, responses};
	}
	catch (const std::exception& ex) {
		std::cerr << "Network error: " << ex.what() << '\n';
		return {false, ""};
	}
}

std::string OpenAIClient::extractAssistantText(const std::string& responseBody) const
{
	Json::Value  root;
	Json::Reader reader;

	if (!reader.parse(responseBody, root))
		throw std::runtime_error("JSON parse failed: " +
								 reader.getFormattedErrorMessages());

	const Json::Value output = root["output"];
	if (!output.isArray())
		throw std::runtime_error("\"output\" is missing or not an array");

	for (const auto& item : output)
	{
		if (item["type"].asString() != "message")
			continue;

		const Json::Value content = item["content"];
		if (!content.isArray())
			continue;

		for (const auto& chunk : content)
		{
			if (chunk["type"].asString() == "output_text" && chunk.isMember("text"))
				return chunk["text"].asString();
		}
	}
	throw std::runtime_error("No assistant text found in response");
}

std::string OpenAIClient::generateResponsesRequestBody(
		const std::string &model,
		const std::string &userPrompt,
		double temperature
) const {
	Json::Value payload;
	payload["model"]        = model;
	//payload["temperature"] = temperature;
	payload["input"] = userPrompt;
	Json::FastWriter fastWriter;
	return fastWriter.write(payload);
}

httplib::Headers OpenAIClient::generateRequestHeaders() const {
	httplib::Headers headers{
			{"Authorization", "Bearer " + apiKey_},
			{"Content-Type",  "application/json"},
			{"Host", "api.openai.com"}
	};
	return headers;
}
