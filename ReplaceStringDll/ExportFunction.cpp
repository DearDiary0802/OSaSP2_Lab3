#include "pch.h"
#include <Windows.h>
#include <cstdlib>
#include "ExportFunction.h"

extern "C" _declspec(dllexport) void __stdcall ReplaceStringInMemory(DWORD PID, const char* srcString, const char* destString)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess)
	{
		size_t srcStrLen = strlen(srcString);
		SYSTEM_INFO systemInfo;
		MEMORY_BASIC_INFORMATION memoryInfo;
		GetSystemInfo(&systemInfo);
		char* pointer = 0;
		SIZE_T lpRead = 0;
		while (pointer < systemInfo.lpMaximumApplicationAddress)
		{
			int sz = VirtualQueryEx(hProcess, pointer, &memoryInfo, sizeof(MEMORY_BASIC_INFORMATION));
			if (sz == sizeof(MEMORY_BASIC_INFORMATION))
			{
				if ((memoryInfo.State == MEM_COMMIT) && (memoryInfo.Protect != PAGE_READONLY) && (memoryInfo.Protect != PAGE_EXECUTE_READ) && 
					(memoryInfo.Protect != PAGE_GUARD) && (memoryInfo.Protect != PAGE_NOACCESS))
				{
					pointer = (char *)memoryInfo.BaseAddress;
					if (memoryInfo.RegionSize != lpRead)
					{
						BYTE* lpData = (BYTE*)malloc(memoryInfo.RegionSize);
						if (ReadProcessMemory(hProcess, pointer, lpData, memoryInfo.RegionSize, &lpRead))
						{
							for (size_t i = 0; i < (lpRead - srcStrLen); ++i)
							{
								if (memcmp(srcString, &lpData[i], srcStrLen) == 0)
								{
									char* replaceMemory = pointer + i;
									for (int j = 0; j < strlen(destString); j++)
									{
										replaceMemory[j] = destString[j];
									}
									replaceMemory[strlen(destString)] = 0;
								}
							}
						}
						free(lpData);
					}
				}
			}
			pointer += memoryInfo.RegionSize;
		}

		CloseHandle(hProcess);
	}
}