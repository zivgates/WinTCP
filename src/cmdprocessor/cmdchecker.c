#include "cmdchecker.h"
#include "../tools/tools.h"
#include "crashhandler.h"
#include "../dirmgmt/dir.h"
#define BUFSIZE 4096

BOOL running = TRUE;

WCHAR* cmds[] = { L"echo" , L"crash", L"cd", L"ls", L"exit", L"clear", L"mkdir", L"rmdir", L"heaptest", L"start",
                  L"sudo"};


 DWORD WINAPI cmd_executor(void* datavoid) {
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
                case 3:
                    print_directory(dta->args);
                    break;
                case 4:
                    running = FALSE;
                    break;
                case 5:
                    clrscr();
                    break;
                case 6:
                    make_directory(dta->args);
                    break;
                case 7:
                    remove_directory(dta->args);
                    break;
                case 8:
                    wprintf(L"Allocating Memory....\n");
                    WCHAR* idk;
                    idk = alloc_mem(30);
                    if (idk == NULL) {
                        show_failure_resp(GetLastError());
                        running = FALSE;
                    }
                    wcscpy_s(idk, 30, L"Mem Allocated And Works!\n\0");
                    wprintf(L"%s\n", idk);
                    wprintf(L"Freeing Memory.....\n");
                    free_mem(idk);
                    wprintf(L"Passed\n");
                    break;
                case 9:
                {
                    WCHAR* context;
                    WCHAR* cmd = wcstok_s(dta->args, L" ", &context);
                    WCHAR* args = wcstok_s(NULL, L"\n", &context);
                    int reslt = ShellExecuteW(NULL, L"open", cmd, args, NULL, SW_SHOW);
                    if (reslt <= 32) {
                        show_failure_resp(GetLastError());
                    }
                    break;
                }
                case 10:
                {
                    WCHAR* context;
                    WCHAR* cmd = wcstok_s(dta->args, L" ", &context);
                    WCHAR* args = wcstok_s(NULL, L"\n", &context);
                    int reslt = ShellExecuteW(NULL, L"runas", cmd, args, NULL, SW_SHOW);
                    if (reslt <= 32) {
                        show_failure_resp(GetLastError());
                    }
                    break;
                }
;               default:
                    wprintf(L"Switch Overflow, Please Report this Problem to zvqle\n");
                    break;
            }
    } _except(EXCEPTION_EXECUTE_HANDLER){
        wprintf(L"Internal Error\n");
    }
    
    return 0;

}

static inline void cmd_checker(WCHAR* command, WCHAR* args, WCHAR* buffer) {
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
    STARTUPINFOW sp;
    ZeroMemory(&sp, sizeof(STARTUPINFOW));
    PROCESS_INFORMATION p;
    ZeroMemory(&p, sizeof(PROCESS_INFORMATION));
    BOOL result = CreateProcessW(NULL,
                                buffer,
                                NULL, NULL,
                                FALSE, BELOW_NORMAL_PRIORITY_CLASS,
                                NULL,
                                NULL,
                                &sp,
                                &p);
    WaitForSingleObject(p.hProcess, INFINITE);
    if (!result) {
        wprintf(L"Couldn't find %s\n", command);
        return;
    }    
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

        WCHAR* cmd = wcstok_s(buffer, L" ", &context);
        WCHAR* arg = wcstok_s(NULL, L"\0", &context); 

        if (cmd == NULL) continue;
        cmd_checker(cmd, arg, buffer);


        free_mem(buffer);
    }
    return TRUE;
}