#include "PlayerPortalCommandProcessor.h"
#include "utils.h"
#include "js.h"
#include "jsoncpp/json.h"

PlayerPortalCommandProcessor::PlayerPortalCommandProcessor() {}
PlayerPortalCommandProcessor::~PlayerPortalCommandProcessor() {}

PlayerPortalJavaScriptCommandProcessor::PlayerPortalJavaScriptCommandProcessor() {}
PlayerPortalJavaScriptCommandProcessor::~PlayerPortalJavaScriptCommandProcessor() {}

void PlayerPortalJavaScriptCommandProcessor::process(PlayerPortalDescriptor *descriptor, Json::Value command)
{
	try
	{
		Json::FastWriter writer;
		std::stringstream jsStream;

		flusspferd::object commandProcessor = JSManager::get()->executeExpression("global.webSocketCommandProcessor").to_object();
		flusspferd::value returnValue = commandProcessor.call("processCommand", writer.write(command, true), descriptor->getUserId());
		
		//flusspferd::value returnValue = JSManager::get()->executeExpression("global.webSocketCommandProcessor.processCommand('" + writer.write(command, true) + "', " + MiscUtil::toString(descriptor->getUserId()) + ");");
		
		if (returnValue.is_string())
			descriptor->send(returnValue.to_std_string());
	}
	catch (flusspferd::exception e)
	{
		MudLog(BRF, 100, TRUE, "Could not execute player portal command `%s`: %s", command["method"].asString().c_str(), e.what());
	}
}
