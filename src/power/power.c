#include "power.h"
#include <winternl.h>
#include <powrprof.h>


#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "powrprof.lib")

extern NTSTATUS NTAPI NtShutdownSystem(SHUTDOWN_ACTION);



typedef enum _SHUTDOWN_ACTION
{
    ShutdownNoReboot,
    ShutdownReboot,
    ShutdownPowerOff,
    ShutdownRebootForRecovery // only works on windows 11, we won't use this
} SHUTDOWN_ACTION;

static inline BOOL giveShutDownPerms() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        show_failure_resp(GetLastError());
        return FALSE;
    }
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, (PLUID)&tkp.Privileges);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0)) {
        show_failure_resp(GetLastError());
        CloseHandle(hToken);
        return FALSE;
    }
    CloseHandle(hToken);
    return TRUE;
}


static VOID forceShutdown(WCHAR* type, WCHAR* isValid) {
    if (wcscmp(isValid, L"1") != 0) return;
    if (!giveShutDownPerms()) {
        wprintf(L"Failed to obtain shutdown privileges\n");
        return;
    }

    switch (type[1]) {
    case L'r':
    {
        NTSTATUS restartStatus = NtShutdownSystem(1);
        if (!NT_SUCCESS(restartStatus)) {
            wprintf(L"Failed Shutting Down Windows, Error Code %lx\n", restartStatus);
        }
        break;
    }
    case L's':
    {
        NTSTATUS shutdownStatus = NtShutdownSystem(0);
        if (!NT_SUCCESS(shutdownStatus)) {
            wprintf(L"Failed Shutting Down Windows, Error Code %lx\n", shutdownStatus);
        }
        break;
    }
    case L'y':
    {
        NTSTATUS recStatus = NtShutdownSystem(3);
        if (!NT_SUCCESS(recStatus)) {
            wprintf(L"Failed Shutting Down Windows, Error Code %lx\n", recStatus);
        }
        break;
    }
    case L'?':
    {
        wprintf(L"power [type] [isImmediate?]\ntype can be -r to restart, -s to shutdown.\nisImmediate can be 1 if true, else 0 or leave empty\n");
        break;
    }
    case L'h':
        SetSuspendState(TRUE, TRUE, FALSE);
        break;
    case L'q':
        SetSuspendState(FALSE, FALSE, FALSE);
        break;
    default:
        wprintf(L"Incorrect Argument, Use -s to shutdown or -r to restart\n");
        break;
    }

}

VOID power_management(WCHAR* args) {
    if (!args) {
        wprintf(L"power [type] [isImmediate?]\ntype can be -r to restart, -s to shutdown, -l to logoff, -h to hibernate, and -q to suspend the machine (if isImmediate isn't 1).\nisImmediate can be 1 if true, else 0 or empty\n");
        return;
    }
    WCHAR* token;
    WCHAR* type = wcstok_s(args, L" ", &token);
    WCHAR* isImmediate = wcstok_s(NULL, L"\n", &token);
    if (!giveShutDownPerms()) {
        return;
    }
    if (!type) {
        wprintf(L"power [type] [isImmediate?]\ntype can be -r to restart, -s to shutdown, -l to logoff (if isImmediate isn't 1).\nisImmediate can be 1 if true, else 0 or empty\n");
    }
    if (isImmediate) {
        forceShutdown(type, isImmediate);
        return;
    }

    switch (type[1]) {
    case L'r':
        if (!InitiateSystemShutdownExW(NULL, L"System is Restarting NOW", 5, FALSE, TRUE, SHTDN_REASON_MAJOR_OTHER)) {
            show_failure_resp(GetLastError());
        }
        break;
    case L's':
        if (!InitiateSystemShutdownExW(NULL, L"System is Shutting Down NOW", 5, FALSE, FALSE, SHTDN_REASON_MAJOR_OTHER)) {
            show_failure_resp(GetLastError());
        }
        break;
    case L'l':
        if (!ExitWindowsEx(EWX_RESTARTAPPS, SHTDN_REASON_MAJOR_SYSTEM)) {
            show_failure_resp(GetLastError());
        }
        break;
    case L'?':
        wprintf(L"power [type] [isImmediate?]\ntype can be -r to restart, -s to shutdown, -l to logoff (if isImmediate isn't 1).\nisImmediate can be 1 if true, else 0 or empty\n");
        break;
    case L'h':
        SetSuspendState(TRUE, FALSE, FALSE);
        break;
    case L'q':
        SetSuspendState(FALSE, FALSE, FALSE);
        break;
    default:
        wprintf(L"Incorrect Argument, Use -s to shutdown or -r to restart\n");
        break;
    }
}