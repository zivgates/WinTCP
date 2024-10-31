#include "tools.h"




void* alloc_mem(size_t size) {
    return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

BOOL free_mem(void* memory) {
    return VirtualFree(memory, 0, MEM_RELEASE);
}



VOID show_failure_resp(DWORD errorCode)
{
    LPVOID lpMsgBuf;

    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&lpMsgBuf,
        0, NULL);

    wprintf(L"%s\n", (LPWSTR)lpMsgBuf);

    LocalFree(lpMsgBuf);
}


BOOL check_if_perms() {
    OFSTRUCT st = { 0 };
    HFILE file = OpenFile("C:\\Windows\\System32\\idk.sys", &st, OF_CREATE);
    if (file == HFILE_ERROR) {
        return FALSE;
    }
    else {
        CloseHandle((HANDLE)file);
        DeleteFileW(L"C:\\Windows\\System32\\idk.sys");
        return TRUE;
    }
}


WCHAR* a_to_w(const char* str) {
    int wstrLength = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    WCHAR* wstr = (WCHAR*)alloc_mem((wstrLength + 1) * sizeof(WCHAR));
    if (MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, wstrLength) == 0) {
        free_mem(wstr);
        return NULL;
    }
    wstr[wstrLength] = L'\0';
    return wstr;

}

char* w_to_a(const wchar_t* str) {
    int size = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    char* buffer = (char*)alloc_mem(size);
    if (buffer != NULL) {
        WideCharToMultiByte(CP_UTF8, 0, str, -1, buffer, size, NULL, NULL);
    }
    return buffer;
}

static VOID cls(HANDLE hConsole)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    SMALL_RECT scrollRect;
    COORD scrollTarget;
    CHAR_INFO fill;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }
    scrollRect.Left = 0;
    scrollRect.Top = 0;
    scrollRect.Right = csbi.dwSize.X;
    scrollRect.Bottom = csbi.dwSize.Y;
    scrollTarget.X = 0;
    scrollTarget.Y = (SHORT)(0 - csbi.dwSize.Y);
    fill.Char.UnicodeChar = TEXT(' ');
    fill.Attributes = csbi.wAttributes;
    ScrollConsoleScreenBuffer(hConsole, &scrollRect, NULL, scrollTarget, &fill);
    csbi.dwCursorPosition.X = 0;
    csbi.dwCursorPosition.Y = 0;
    SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
}

VOID clrscr() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    cls(hConsole);
}