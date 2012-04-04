#include "SystemUtil.h"
#include <cstdio>
#include <cstdlib>

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

FILE *pipeOpen(const char *command, const char *mode)
{
#ifdef WIN32
	return _popen(command, mode);
#else
	return popen(command, mode);
#endif
}