#include "pch.h"
#include "ExportFunction.h"
#include <cstdio>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    DWORD pid = GetCurrentProcessId();

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        char pathToDataFile[] = "C:\\Users\\User\\Desktop\\ОСиСП_ч2\\OSaSP_Lab3\\dataForInjection.txt";
        FILE* fp;
        char srcStr[255];
        char destStr[255];
        #pragma warning(suppress : 4996)
        if ((fp = fopen(pathToDataFile, "r+")) != NULL)
        {
            #pragma warning(suppress : 4996)
            fscanf(fp, "%s %s", &srcStr, &destStr);
            fclose(fp);
        }
        ReplaceStringInMemory(pid, srcStr, destStr);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

