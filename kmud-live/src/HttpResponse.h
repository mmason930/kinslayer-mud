#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <map>

class HttpResponse
{
private:
	int responseCode;
	unsigned int id;
	std::map<std::string, std::string> headers;
	std::string responseBody;
public:

	void setResponseCode(int responseCode);
	int getResponseCode() const;

	bool hasHeader(const std::string &headerName) const;
	std::string getHeader(const std::string &headerName) const;
	void setHeader(const std::string &headerName, const std::string &headerValue);

	std::string getResponseBody() const;
	void setResponseBody(const std::string &responseBody);

	std::string generatePacket();

	unsigned int getId();
	void setId(unsigned int id);
};

#endif
