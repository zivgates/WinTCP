#pragma once


#include <Windows.h>
#include <wchar.h>



void* alloc_mem(size_t size);
BOOL free_mem(void* memory);
VOID show_failure_resp(DWORD errorCode);