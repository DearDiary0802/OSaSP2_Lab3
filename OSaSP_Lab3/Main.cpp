#define _CRT_SECURE_NO_WARNINGS
#include <locale.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <tlhelp32.h>

#pragma comment(lib, "C:\\Users\\User\\Desktop\\ÎÑèÑÏ_÷2\\OSaSP_Lab3\\Debug\\ReplaceStringDll.lib")

typedef HMODULE(WINAPI* LPLoadLibrary)(LPCSTR);
typedef HMODULE(WINAPI* LPGetProcAddress)(HMODULE, LPCSTR);

#define SOURCE_STRING "Simple_string_without_sense"
#define DLL_PATH "C:\\Users\\User\\Desktop\\ÎÑèÑÏ_÷2\\OSaSP_Lab3\\Debug\\ReplaceStringDll.dll"

extern "C" _declspec(dllimport) void __stdcall ReplaceStringInMemory(DWORD PID, const char* srcString, const char* destString);
typedef void __stdcall TReplaceStringInMemory(DWORD PID, const char* srcString, const char* destString);

void StaticImport(DWORD pid);
void DynamicImport(DWORD pid);
void Injection(DWORD pid);

const char SOURCE_STR[] = "Simple_string_without_sense";
const char STR_FOR_REPLACING[] = "Other_string";
const char FUNCTION_NAME[] = "_ReplaceStringInMemory@12";

int main()
{
	setlocale(LC_ALL, "Russian");
	const char data1[] = SOURCE_STRING;
	const char data2[] = SOURCE_STRING;
	DWORD pid = GetCurrentProcessId();

	std::cout << "PID: " << pid << std::endl;

	char key;
	std::cout << "Choose the type: 1) static import; 2) dynamic import; 3) injection" << std::endl;
	std::cin >> key;

	switch (key)
	{
	case '1':
		StaticImport(pid);
		break;
	case '2':
		DynamicImport(pid);
		break;
	case '3':
		DWORD attachPID = 0;
		std::cout << "Enter attachPID: " << std::endl;
		std::cin >> attachPID;
		Injection(attachPID);
		break;
	}

	std::cout << "Source string: " << SOURCE_STRING << "; changed string: " << data1 << std::endl;
	std::cout << "Source string: " << SOURCE_STRING << "; changed string: " << data2 << std::endl;

	system("pause");
	return 0;
}

void StaticImport(DWORD pid)
{
	ReplaceStringInMemory(pid, SOURCE_STR, STR_FOR_REPLACING);
}

void DynamicImport(DWORD pid)
{
	HMODULE hDll = LoadLibraryA(DLL_PATH);
	if (hDll == NULL)
		return;
	TReplaceStringInMemory* function = (TReplaceStringInMemory*)GetProcAddress(hDll, FUNCTION_NAME);
	function(pid, SOURCE_STR, STR_FOR_REPLACING);
	FreeLibrary(hDll);
}


void Injection(DWORD pid)
{
	HANDLE hProc = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_VM_WRITE |
		PROCESS_CREATE_THREAD | PROCESS_CREATE_PROCESS, FALSE, pid);

	if (hProc)
	{
		char pathToDataFile[] = "C:\\Users\\User\\Desktop\\ÎÑèÑÏ_÷2\\OSaSP_Lab3\\dataForInjection.txt";
		FILE* fp;
		if ((fp = fopen(pathToDataFile, "w")) != NULL)
		{
			fprintf(fp, "%s %s", SOURCE_STR, STR_FOR_REPLACING);
			fclose(fp);
		}

		LPVOID baseAddress = VirtualAllocEx(hProc, NULL, strlen("ReplaceStringInMemory.dll") + 1, MEM_COMMIT, PAGE_READWRITE);
		int i = WriteProcessMemory(hProc, baseAddress, "ReplaceStringInMemory.dll", strlen("ReplaceStringInMemory.dll") + 1, NULL);
		LPVOID addr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
		HANDLE threadID = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)addr, baseAddress, NULL, NULL);

		if (threadID != NULL) {
			WaitForSingleObject(threadID, INFINITE);

			DWORD hLibrary = 0;
			GetExitCodeThread(hProc, &hLibrary);
			CloseHandle(hProc);

			FreeLibrary((HMODULE)hLibrary);
		}
	}
}