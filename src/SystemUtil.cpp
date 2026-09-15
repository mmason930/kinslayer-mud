#include "SystemUtil.h"


#ifdef WIN32
#else
#include <unistd.h>
#include <fstream>
#endif

bool SystemUtil::processExists(const unsigned int processId)
{
#ifdef WIN32
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize=sizeof(PROCESSENTRY32);
		if(Process32First(hSnapshot,&pe32))
		{
			do
			{
				if(pe32.th32ProcessID == processId)
					return true;
			} while(Process32Next(hSnapshot,&pe32));
		}
		CloseHandle(hSnapshot);
	}
	return false;
#else
	if (processId == 0)
		return false;

	// A zombie still has a /proc entry, but has already finished shutting down.
	// Read only this PID instead of scanning every process on the host.
	std::ifstream status("/proc/" + std::to_string(processId) + "/status");
	std::string line;
	while (std::getline(status, line))
	{
		if (line.compare(0, 6, "State:") == 0)
		{
			const auto state = line.find_first_not_of(" \t", 6);
			return state != std::string::npos && line[state] != 'Z'
				&& line[state] != 'X' && line[state] != 'x';
		}
	}
	return false;
#endif
}

FILE *SystemUtil::pipeOpen(const char *command, const char *mode)
{
#ifdef WIN32
	return _popen(command, mode);
#else
	return popen(command, mode);
#endif
}

unsigned int SystemUtil::getProcessId()
{
#ifdef WIN32
	return (unsigned int)GetCurrentProcessId();
#else
	return (unsigned int)getpid();
#endif
}

std::string SystemUtil::processCommand(const std::string &command)
{
	FILE *pipe = pipeOpen(command.c_str(), "r");
	char temporaryBuffer[ 1024 * 1024 ];
	std::string buffer;

	if(!pipe)
	{
		return "";
	}

	size_t bytesRead;
	while ((bytesRead = fread(temporaryBuffer, sizeof(char), sizeof(temporaryBuffer), pipe)) > 0)
		buffer.append(temporaryBuffer, bytesRead);

	// popen owns a child process as well as a stream. fclose leaks that child.
#ifdef WIN32
	_pclose(pipe);
#else
	pclose(pipe);
#endif

	return buffer;
}
