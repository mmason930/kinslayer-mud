#ifndef SYSTEM_UTIL_H
#define SYSTEM_UTIL_H

#include <cstdio>
#include <map>
#include <string>

class SystemUtil {

protected:
private:
public:

	static bool processExists(const unsigned int processId);
	static FILE *pipeOpen(const char *command, const char *mode);
	static unsigned int getProcessId();
	static std::map<std::string, std::string> getSubversionInfoMap(const std::string &url);
	static std::string processCommand(const std::string &command);
//	static void printStackTrace();
};

#endif
