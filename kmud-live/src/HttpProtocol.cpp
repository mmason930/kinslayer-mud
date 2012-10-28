#include "HttpProtocol.h"

std::list<HttpProtocol*> HttpProtocol::enums;

HttpProtocol *HttpProtocol::options = new HttpProtocol(0, "OPTIONS");
HttpProtocol *HttpProtocol::get = new HttpProtocol(1, "GET");
HttpProtocol *HttpProtocol::head = new HttpProtocol(3, "HEAD");
HttpProtocol *HttpProtocol::post = new HttpProtocol(4, "POST");
HttpProtocol *HttpProtocol::deletion = new HttpProtocol(5, "DELETE");
HttpProtocol *HttpProtocol::trace = new HttpProtocol(6, "TRACE");
HttpProtocol *HttpProtocol::connect = new HttpProtocol(7, "CONNECT");
