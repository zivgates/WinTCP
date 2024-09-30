#pragma once



#include "../WinTSP.h"


BOOL start_shell();


typedef struct _data {
	WCHAR* cmd;
	WCHAR* args;
	int index;
} data;