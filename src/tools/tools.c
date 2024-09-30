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