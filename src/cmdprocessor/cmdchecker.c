#include "cmdchecker.h"
#include "../tools/tools.h"
#include "crashhandler.h"

#define BUFSIZE 4096

BOOL running = TRUE;

WCHAR* cmds[] = { L"echo" , L"crash", L"cd"};






static inline DWORD WINAPI cmd_executor(void* datavoid) {
    data* dta = (data*)datavoid;
    _try{
            switch (dta->index) {
            case 0:
                wprintf(L"%s\n", dta->args);
                break;
            case 1:
            {
                int* p = NULL;
                *p = 42;
                break;
            }
            case 2:
            {
                if (!dta->args) {
                    WCHAR dir[512];
                    GetCurrentDirectory(512, dir);
                    wprintf(L"%s\n", dir);
                    return;
                }
                BOOL result = SetCurrentDirectoryW(dta->args);
                if (!result) show_failure_resp(GetLastError());
                break;
            }
            default:
                wprintf(L"Switch Overflow, Please Report this Problem to zvqle\n");
                break;
            }
    }
    _except(EXCEPTION_EXECUTE_HANDLER){
        EXCEPTION_POINTERS* p = GetExceptionInformation();
        crash_handler(p);
    }
    
    return 0;

}

static inline void cmd_checker(WCHAR* command, WCHAR* args) {
    data dta = { command, args , 0};
    int cmdsize = sizeof(cmds) / sizeof(cmds[0]);
    for (int i = 0; i < cmdsize; i++) {
        if (wcscmp(command, cmds[i]) == 0) {
            dta.index = i;
            HANDLE hThread = CreateThread(NULL, 0, cmd_executor, (LPVOID)&dta, 0, NULL);
            if (!hThread) {
                wprintf(L"Failure to Create Command Processing Thread\n");
                running = FALSE;
                return;
            }
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
            return;
        }
    }
    wprintf(L"Couldn't find %s\n", command);
}   



BOOL start_shell() {
    wprintf(L"Zivgates WinTCP Build %f\n"
        L"Copyright realzvqle, All Rights Reserved\n\n", VER);
    LPWSTR context = NULL;

    while (running) {
        LPWSTR buffer = alloc_mem(BUFSIZE);
        WCHAR dir[512];
        GetCurrentDirectory(512, dir);
        wprintf(L"%ls> ", dir); 
        if (fgetws(buffer, BUFSIZE, stdin) == NULL) {
            free_mem(buffer);
            continue; 
        }
        size_t len = wcslen(buffer);
        if (len > 0 && buffer[len - 1] == L'\n') {
            buffer[len - 1] = L'\0';
        }

        WCHAR* cmd = wcstok(buffer, L" ", &context);
        WCHAR* arg = wcstok(NULL, L"\0", &context); 

        if (cmd == NULL) continue;
        cmd_checker(cmd, arg);


        free_mem(buffer);
    }
    return TRUE;
}