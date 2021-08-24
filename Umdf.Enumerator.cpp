#include <windows.h>
#include <strsafe.h>
#include <stdio.h>
#include "wdfdevice.h"
#include "Wdf.h"

int main(int argc, char** argv)
{
    char* pszDosDeviceName, * pszNtDeviceName;
    bool  fRemoveDeviceName = false;
    bool  fResult;

    if (argc != 3)
    {
        printf("\nusage: %s <DOS device name> <NT device name> to add\n", argv[0]);
        printf("usage: %s [-r] <DOS device name>  to remove\n", argv[0]);
        printf("\n\texample: %s d: \\device\\cdrom0\n", argv[0]);
        return 0;
    }

    fRemoveDeviceName = !lstrcmpi(argv[1], "-r");
    if (fRemoveDeviceName)
    {
        pszDosDeviceName = argv[2];
        pszNtDeviceName = (char*)LocalAlloc(LPTR, MAX_PATH);
        fResult = QueryDosDevice(pszDosDeviceName, pszNtDeviceName, MAX_PATH);
        if (fResult)
        {
            fResult = DefineDosDevice(DDD_RAW_TARGET_PATH | DDD_REMOVE_DEFINITION | DDD_EXACT_MATCH_ON_REMOVE, pszDosDeviceName, pszNtDeviceName);
        }
        if (!fResult)
            printf("error %lu: could not remove %s\n", GetLastError(), pszDosDeviceName);
        LocalFree(pszNtDeviceName);
    }

    else
    {
        pszDosDeviceName    = argv[1];
        pszNtDeviceName     = argv[2];

        fResult = DefineDosDevice(DDD_RAW_TARGET_PATH, pszDosDeviceName,pszNtDeviceName);
        if (!fResult)
            ErrorExit((LPTSTR)"Problem Detail: ");
    }

    return 0;

}

void ErrorExit(LPTSTR lpszFunction)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw);
}
