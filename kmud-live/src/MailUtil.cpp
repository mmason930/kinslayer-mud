#include "conf.h"
#include "sysdep.h"

#include "MailUtil.h"
#include "StringUtil.h"

#include "kuSockets.h"
#include "kuClient.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>

const int WAITING_HELO_REPLY = 0;
const int WAITING_MAIL_FROM_REPLY = 1;
const int WAITING_RCPT_TO_REPLY = 2;
const int WAITING_DATA_REPLY = 3;
const int WAITING_MESSAGE_REPLY = 4;

void MailUtil::sendEmail(std::string smtpName, short int smtpPort, std::string fromEmailAddress, std::string fromDisplayName, std::string toEmailAddress, std::string subject, std::string message)
{
	std::stringstream ehloBuffer;
	std::stringstream mailFromBuffer;
	std::stringstream rcptToBuffer;
	std::stringstream messageBuffer;
	int state;
	std::list<std::string> lineResponsesFromSmtpServer;

	messageBuffer << "From: " << fromDisplayName << "<" << fromEmailAddress << ">\r\n"
//				  << "Reply-to: " << fromEmailAddress << "\r\n"
				  << "MIME-Version: 1.0\r\n"
				  << "To: " << toEmailAddress << "\r\n"
				  << "Subject: " << subject << "\r\n"
				  << "Content-Type: text/html; charset=iso-8859-1\r\n"
				  << "Content-Transfer-Encoding: 7bit\r\n"
				  << "\r\n"
				  << "<html>" << message << "</html>\r\n"
				  << "\r\n.\r\n";

	std::cout << "EMAIL MESSAGE: " << messageBuffer.str() << std::endl;

	unsigned int messageSize = messageBuffer.str().size();
		
	ehloBuffer << "EHLO [localhost]\r\n";
	mailFromBuffer << "mail FROM:<" << fromEmailAddress << "> size=" << messageSize << "\r\n";
	rcptToBuffer << "rcpt TO:<" << toEmailAddress << ">\r\n";
	
	std::cout << "EHLO: `" << ehloBuffer.str() << "`" << std::endl;
	std::cout << "MAIL FROM: `" << mailFromBuffer.str() << "`" << std::endl;
	std::cout << "RCPT TO: `" << rcptToBuffer.str() << "`" << std::endl;
	std::cout << "MESSAGE: `" << messageBuffer.str() << "`" << std::endl;

	std::stringstream inputStreamFromSmtpServer;
	std::string inputBufferFromSmtpServer;
	kuClient smtpServerConnection;
	smtpServerConnection.connect(smtpName, smtpPort);

	smtpServerConnection.send(ehloBuffer.str().c_str());
	state = WAITING_HELO_REPLY;
	std::string lineFromServer;

	while(true)
	{
		smtpServerConnection.receive(inputStreamFromSmtpServer);
		inputBufferFromSmtpServer += inputStreamFromSmtpServer.str();
		inputStreamFromSmtpServer.str("");

		lineResponsesFromSmtpServer.clear();

		do
		{
			lineFromServer = StringUtil::getLine(inputBufferFromSmtpServer, "\r\n");
			if(lineFromServer.empty() == false)
				lineResponsesFromSmtpServer.push_back(lineFromServer);
			
		} while(lineFromServer.empty() == false);

		if(lineResponsesFromSmtpServer.empty() == false)
		{
			if(!smtpServerConnection.isConnected())
			{
				std::cout << "Connection to SMTP server has been aborted." << std::endl;
				break;
			}
			if(state == WAITING_HELO_REPLY)
			{
				while(lineResponsesFromSmtpServer.empty() == false)
				{
					std::cout << "HELO REPLY: `" << lineResponsesFromSmtpServer.front() << "`" << std::endl;
					lineResponsesFromSmtpServer.pop_front();
				}
				while(lineFromServer.empty() == false);
				smtpServerConnection.send(mailFromBuffer.str().c_str());
				state = WAITING_MAIL_FROM_REPLY;
			}
			else if(state == WAITING_MAIL_FROM_REPLY)
			{
				while(lineResponsesFromSmtpServer.empty() == false)
				{
					std::cout << "MAIL FROM REPLY: `" << lineResponsesFromSmtpServer.front() << "`" << std::endl;
					lineResponsesFromSmtpServer.pop_front();
				}
				smtpServerConnection.send(rcptToBuffer.str().c_str());
				state = WAITING_RCPT_TO_REPLY;
			}
			else if(state == WAITING_RCPT_TO_REPLY)
			{
				while(lineResponsesFromSmtpServer.empty() == false)
				{
					std::cout << "MAIL RCPT TO: `" << lineResponsesFromSmtpServer.front() << "`" << std::endl;
					lineResponsesFromSmtpServer.pop_front();
				}
				smtpServerConnection.send("data\r\n");
				state = WAITING_DATA_REPLY;
			}
			else if(state == WAITING_DATA_REPLY)
			{
				while(lineResponsesFromSmtpServer.empty() == false)
				{
					std::cout << "DATA REPLY: `" << lineResponsesFromSmtpServer.front() << "`" << std::endl;
					lineResponsesFromSmtpServer.pop_front();
				}
				smtpServerConnection.send(messageBuffer.str().c_str());
				state = WAITING_MESSAGE_REPLY;
			}
			else if(state == WAITING_MESSAGE_REPLY)
			{
				while(lineResponsesFromSmtpServer.empty() == false)
				{
					std::cout << "MESSAGE REPLY: `" << lineResponsesFromSmtpServer.front() << "`" << std::endl;
					lineResponsesFromSmtpServer.pop_front();
				}
				smtpServerConnection.send("QUIT\r\n");
				break;
			}
		}

		boost::this_thread::yield();
	}

	smtpServerConnection.disconnect();
}

void MailUtil::sendEmail(const std::string &fromEmailAddress, const std::string &fromDisplayName, const std::string &toEmailAddress, const std::string &subject, const std::string &message)
{
	boost::thread thread( boost::bind( &MailUtil::sendEmail, "127.0.0.1", 25, fromEmailAddress, fromDisplayName, toEmailAddress, subject, message ) );
}
