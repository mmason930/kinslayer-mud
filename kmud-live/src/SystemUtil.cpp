#include "SystemUtil.h"
#include <cstdio>

#ifdef WIN32
#include <Windows.h>
#include <TlHelp32.h>
#else

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
	//TODO: Linux code
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