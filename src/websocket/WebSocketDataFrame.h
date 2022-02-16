#ifndef WEB_SOCKET_DATA_FRAME_H
#define WEB_SOCKET_DATA_FRAME_H

#include <string>
#include <vector>

class WebSocketDataFrame
{
protected:
	bool fin;
	bool rsv1, rsv2, rsv3;
	bool isMasked;
	unsigned char opcode;
	std::vector<unsigned char> maskCode;
	std::string payloadData;
public:

	WebSocketDataFrame();
	~WebSocketDataFrame();

	static WebSocketDataFrame *parse(const std::string &input, unsigned int &bytesRead);
	std::string prepareNetworkPacket();

	bool getFin() const;
	void setFin(const bool fin);

	bool getRsv1() const;
	void setRsv1(const bool rsv1);

	bool getRsv2() const;
	void setRsv2(const bool rsv2);

	bool getRsv3() const;
	void setRsv3(const bool rsv3);

	unsigned char getOpCode() const;
	void setOpCode(const unsigned char opcode);

	std::string getPayloadData() const;
	void setPayloadData(const std::string payloadData);

	bool getIsMasked() const;
	void setIsMasked(bool isMasked);

	std::vector<unsigned char> getMaskCode() const;
	void setMaskCode(const std::vector<unsigned char> &maskCode);
};

#endif
