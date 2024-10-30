#pragma once



#include "../WinTSP.h"


BOOL start_shell();


typedef struct _data {
	LPWSTR cmd;
	LPWSTR args;
	int index;
} data;