#include "logger.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define BUF_LEN 1024

LogCallback g_logger = NULL;

void SetLogCallback(LogCallback cb)
{
    g_logger = cb;
}

void Log(int _level, const char* msg, ...)
{
    if (!g_logger)return;
    char logBuf[BUF_LEN];
    va_list valist;
    va_start(valist, msg);
    vsprintf_s(logBuf, BUF_LEN, msg, valist);
    g_logger(logBuf, _level);
    va_end(valist);
}
