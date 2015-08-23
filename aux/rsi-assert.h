#ifndef _RSI_DIE_H_
#define _RSI_DIE_H_

#include "stdio.h"
#include "dev/leds.h"
#include "dev/watchdog.h"

// if RSI_NRESET is defined then wdt will be disabled and the code will be
// stuck forever in the assert.
// this should *never* be enabled in a production node.

#ifdef RSI_NRESET
#define rsi_stop_wdt() {watchdog_stop(); printf("\tstopping wdt...\n");}
#else  // RSI_NRESET
#define rsi_stop_wdt()
#endif // RSI_NRESET

#define rsi_assert(x,msg) {                                 \
    if(!x)                                                  \
    {                                                       \
        printf("\n\n----\nassert failed:%s\n----\n", msg);  \
        leds_init();                                        \
        leds_on(LEDS_ALL);                                  \
        rsi_stop_wdt();                                     \
        while(1);                                           \
    }                                                       \
}

#endif // _RSI_DIE_H_
