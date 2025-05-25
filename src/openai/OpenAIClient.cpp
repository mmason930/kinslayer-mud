#include <httplib.h>
#include "OpenAIClient.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>   // wraps asio::ssl for Beast

#include <openssl/ssl.h>         // SSL_set_tlsext_host_name
#include <openssl/err.h>         // ERR_get_error
#include "../jsoncpp/json.h"

OpenAIChatCompletionResult OpenAIClient::chatCompletion(
		const std::string& model,
		const std::string& userPrompt,
		double temperature
		) const
{
	namespace asio  = boost::asio;
	namespace beast = boost::beast;
	namespace http  = beast::http;
	using     tcp   = asio::ip::tcp;

	const std::string body    = generateChatCompletionRequestBody(model, userPrompt, temperature);
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

		if (res.result() != http::status::ok) {
			return {false, ""};
		}

		Json::Reader reader;
		Json::Value  root;
		if (!reader.parse(res.body(), root)) {
			throw std::runtime_error(
					"JSON parse failed: " + reader.getFormattedErrorMessages());
		}
		std::cout << "...9" << std::endl;
		std::string completion = root["output"][1]["message"]["content"].asString();
		return {true, completion};
	}
	catch (const std::exception& ex) {
		std::cerr << "Network error: " << ex.what() << '\n';
		return {false, ""};
	}
}

std::string OpenAIClient::generateChatCompletionRequestBody(
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
