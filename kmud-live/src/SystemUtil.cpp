#include "SystemUtil.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <list>

#include "StringUtil.h"
#include "MiscUtil.h"

#ifdef WIN32
#include <Windows.h>
#include <TlHelp32.h>
#else
#include <unistd.h>
#include <dirent.h>
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
	DIR* dir;
	struct dirent* ent;
	char* endptr;
	char buf[512];

	if (!(dir = opendir("/proc"))) {
		perror("Can't open /proc");
		return false;
	}

	while((ent = readdir(dir)) != NULL) {
		// if endptr is not a null character, the directory is not
		// entirely numeric, so ignore it
		long lpid = strtol(ent->d_name, &endptr, 10);
		if (*endptr != '\0') {
			continue;
		}

		// try to open the cmdline file
		snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
		if(lpid == processId)
		{
			closedir(dir);
			return true;
		}
		/***
		FILE* fp = fopen(buf, "r");

		if (fp) {
			if (fgets(buf, sizeof(buf), fp) != NULL) {
				// check the first token in the file, the program name
				char* first = strtok(buf, " ");
				if (!strcmp(first, name)) {
					fclose(fp);
					closedir(dir);
					return (pid_t)lpid;
				}
			}
			fclose(fp);
		}
		***/
	}
	
	closedir(dir);
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
	return (unsigned int)getCurrentProcessId();
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

	while(!feof(pipe))
	{
		size_t bytesRead = fread(temporaryBuffer, sizeof(char), sizeof(temporaryBuffer) - 1, pipe);

		temporaryBuffer[ bytesRead ] = '\0';

		buffer += temporaryBuffer;
	}

	fclose(pipe);

	return buffer;
}

std::map<std::string, std::string> SystemUtil::getSubversionInfoMap(const std::string &url)
{
	std::map<std::string, std::string> map;

	std::string escapedUrl = url;
	StringUtil::replace(escapedUrl, "\\", "\\\\");
	StringUtil::replace(escapedUrl, "'", "\\'");

	std::string subversionInfo = processCommand(std::string("svn info '") + escapedUrl + "'");
	std::vector<std::string> outputLines = StringUtil::SplitToVector(subversionInfo, '\n');

	for(std::string outputLine : outputLines)
	{
		std::string::size_type colonPosition = outputLine.find(':');

		if(colonPosition == std::string::npos)
			continue;

		std::string key = outputLine.substr(0, colonPosition);
		std::string value = outputLine.substr(colonPosition + 1);

		StringUtil::trim(value);

		map[key] = value;
	}

	return map;
}