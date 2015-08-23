#ifndef __RSI_DBG_H__
#define __RSI_DBG_H__

#include "stdarg.h"
#include "stdio.h"

// RSI_DBG must be defined before including this file
// will printf only if RSI_DBG is defined

#ifndef RSI_DBG
    #define dbg(x)
#else
    #define dbg(x) {do {dbg_printf x;} while (0);}
    static void dbg_printf(const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
#endif // ndef RSI_DBG

#endif // __RSI_DBG_H__
