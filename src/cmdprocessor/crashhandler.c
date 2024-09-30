#include "crashhandler.h"





void crash_handler(EXCEPTION_POINTERS* exceptionInfo) {
    wprintf(L"Exception Occured! Exception code: 0x%x\n",
        exceptionInfo->ExceptionRecord->ExceptionCode);
    wprintf(L"Please Report this to zvqle!\n");
}