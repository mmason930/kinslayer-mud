#ifndef SYSTEM_UTIL_H
#define SYSTEM_UTIL_H

#include <cstdio>

class SystemUtil {

protected:
private:
public:

	static bool processExists(const unsigned int processId);
	static FILE *pipeOpen(const char *command, const char *mode);
	static unsigned int getProcessId();
};

#endif
