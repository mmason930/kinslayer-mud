#include "PlayerPortalDescriptor.h"
#include "WebSocketClientHeader.h"
#include "WebSocketException.h"
#include "WebSocketDataFrame.h"
#include "StringUtil.h"
#include "jsoncpp/json.h"
#include "CharacterUtil.h"
#include "utils.h"

PlayerPortalDescriptor::PlayerPortalDescriptor(kuDescriptor *descriptor, PlayerPortalServer *server)
{
	this->server = server;
	this->descriptor = descriptor;
	this->uid = descriptor->getUid();

	setStatus(PlayerPortalDescriptorStatus::handshaking);
}

PlayerPortalDescriptor::~PlayerPortalDescriptor()
{
	if(!this->descriptor->socketIsClosed())
		this->descriptor->disconnect();

	delete this->descriptor;
}

PlayerPortalServer *PlayerPortalDescriptor::getServer()
{
	return this->server;
}

PlayerPortalDescriptorStatus *PlayerPortalDescriptor::getStatus()
{
	return this->status;
}

void PlayerPortalDescriptor::setStatus(PlayerPortalDescriptorStatus *status)
{
	this->status = status;
}

int PlayerPortalDescriptor::getUserId()
{
	return this->userId;
}

void PlayerPortalDescriptor::setUserId(int userId)
{
	this->userId = userId;
}

int PlayerPortalDescriptor::getUid()
{
	return this->uid;
}

void PlayerPortalDescriptor::close()
{
	if(!this->descriptor->socketIsClosed())
		this->descriptor->socketClose();
}

void PlayerPortalDescriptor::processInput()
{
	if(this->getStatus() == PlayerPortalDescriptorStatus::handshaking)
	{
		try
		{
			std::string inputBuffer = descriptor->getInputBuffer();
			if (WebSocketClientHeader::isComplete(inputBuffer))
			{
				std::auto_ptr<WebSocketClientHeader> webSocketClientHeader(WebSocketClientHeader::allocateByInitialClientPacket(inputBuffer));
				webSocketClientHeader->read(inputBuffer);
				std::string response = webSocketClientHeader->generateResponse(this->server->getPort(), "player-portal-protocol");

				this->descriptor->send(response);
				this->descriptor->clearInput();

				this->setStatus(PlayerPortalDescriptorStatus::authenticating);
			}
		}
		catch (WebSocketException webSocketException)
		{
			Log("Exception while processing websocket input for player portal descriptor: %s", webSocketException.what().c_str());
			this->close();
		}
	}
	else
	{
		if( *(this->descriptor->getInputDataBuffer()) )
		{
			unsigned int bytesRead = 0;
			WebSocketDataFrame *webSocketDataFrame = WebSocketDataFrame::parse(this->descriptor->getInputBuffer(), bytesRead);

			if (webSocketDataFrame != NULL)
			{
				if (webSocketDataFrame->getOpCode() == 0x8)
				{
					delete webSocketDataFrame;
					Log("Web Socket already closed while reading data frames in PlayerPortalDescriptor.cpp");
					return;
				}
				else
				{
					descriptor->eraseInput(0, bytesRead);
					inputBuffer += webSocketDataFrame->getPayloadData();
					delete webSocketDataFrame;
				}
			}
		}

		if(inputBuffer.size() > 0)
		{
			const char *inputBufferData = inputBuffer.c_str(), *commandEnd, *commandStart = inputBufferData;

			while ((commandEnd = strchr(commandStart, 0x06)) != NULL)
			{
				std::string jsonCommand = std::string(commandStart, commandEnd - commandStart);
				Json::Value command;
				Json::Reader reader;

				if (!reader.parse(jsonCommand, command, false))
				{
					MudLog(BRF, LVL_APPR, TRUE, "Could not process player portal websocket command. Input: %s", StringUtil::vaEscape(jsonCommand).c_str());
					
				}
				else
				{
					processCommand(command);
				}

				commandStart = commandEnd + 1;
			}

			if (commandStart != inputBufferData)
				inputBuffer.erase(0, commandStart - inputBufferData);
		}
	}
}

void PlayerPortalDescriptor::processCommand(const Json::Value &command)
{
	Json::Value methodValue = command["method"];

	if (methodValue.isNull() || !methodValue.isString())
	{
		Log("Invalid player portal JSON command. No method found.");
		return;
	}

	std::string method = methodValue.asString();
	Json::Value response;

	if (getStatus() == PlayerPortalDescriptorStatus::authenticating)
	{
		if (method != "SessionID")
		{
			Log("Invalid command sent to player portal descriptor while in authentication state. Method: %s", method.c_str());
			return;
		}

		Json::Value sessionIdValue = command["sessionId"];

		if (sessionIdValue.isNull() || !sessionIdValue.isString())
		{
			Log("Invalid or null session ID passed to player portal descriptor command parser for `SessionID` command.");
			this->close();
			return;
		}

		std::string sessionId = sessionIdValue.asString();

		Json::Value userIdValue = command["userId"];

		if (userIdValue.isNull() || !userIdValue.isInt())
		{
			Log("Invalid or null user ID passed to player portal descriptor command parser for `SessionID` command.");
			this->close();
			return;
		}

		int userId = userIdValue.asInt();

		if (CharacterUtil::authenticateUserWebSession(userId, sessionId))
		{
			response["WasSuccessful"] = true;
			response["method"] = command["method"].asString();
			setStatus(PlayerPortalDescriptorStatus::connected);
		}
		else
		{
			close();
			return;
		}

		setUserId(userId);

		Json::FastWriter writer;
		this->send(writer.write(response));
	}
	else if (getStatus() == PlayerPortalDescriptorStatus::connected)
	{
		getServer()->processCommand(this, command);
	}
}

void PlayerPortalDescriptor::send(const std::string &output)
{
	WebSocketDataFrame dataFrame;

	dataFrame.setFin(true);
	dataFrame.setIsMasked(false);
	dataFrame.setRsv1(false);
	dataFrame.setRsv2(false);
	dataFrame.setRsv3(false);
	dataFrame.setOpCode(0x01);

	char *cleansedPacket = new char[output.size() + 1];
	std::string::size_type writeIndex = 0;
	std::string::size_type readIndex = 0;
	for (std::string::size_type readIndex = 0; readIndex < output.size(); ++readIndex)
	{
		if (((int)(output[readIndex])) > 0)
		{
			cleansedPacket[writeIndex] = output[readIndex];
			++writeIndex;
		}
	}
	cleansedPacket[writeIndex] = '\0';
	dataFrame.setPayloadData(cleansedPacket);

	delete[] cleansedPacket;

	descriptor->socketWriteInstant(dataFrame.prepareNetworkPacket());
}
