



#include "cmdprocessor/cmdchecker.h"
#include "tools\tools.h"


int wmain()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        show_failure_resp(GetLastError());
        return -1;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        show_failure_resp(GetLastError());
        return -2;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        show_failure_resp(GetLastError());
        return -3;
    }

    start_shell();
    return 0;
}


