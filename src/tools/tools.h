#pragma once


#include <Windows.h>
#include <wchar.h>



void* alloc_mem(size_t size);
BOOL free_mem(void* memory);
VOID show_failure_resp(DWORD errorCode);
WCHAR* a_to_w(const char* str);
char* w_to_a(const wchar_t* str);
VOID clrscr();