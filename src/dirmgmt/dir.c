#include "dir.h"
#include "extern/tinydir.h"
#include "../tools/tools.h"
#include <stdio.h>
#include <tchar.h>

VOID print_directory(WCHAR* directory) {
    tinydir_dir dir;
    int i = 0;
    WCHAR path[512];
    if (directory) wcscpy_s(path, 512, directory);
    else {
        GetCurrentDirectory(512, path);
    }
    tinydir_open(&dir, path);
    while(dir.has_next){
        i++;
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        if(file.is_dir) _tprintf(L"[%s] ", file.name);
        else _tprintf(L"%s ", file.name);
        if(i % 7 == 0) _tprintf(L"\n");
        tinydir_next(&dir);
    }
    _tprintf(L"\n");
    tinydir_close(&dir);
}

VOID make_directory(WCHAR* directory) {
    if (!directory) return;
    BOOL result =  CreateDirectoryW(directory, NULL);
    if (!result) {
        show_failure_resp(GetLastError());
    }
}

VOID remove_directory(WCHAR* directory) {
    if (!directory) return;
    BOOL result = RemoveDirectoryW(directory, NULL);
    if (!result) {
        show_failure_resp(GetLastError());
    }
}