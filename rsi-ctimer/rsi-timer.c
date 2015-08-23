#include "contiki.h"
#include "stdio.h"
#include "sys/ctimer.h"

PROCESS(rsi_timer_process, "RSI timer Process");
AUTOSTART_PROCESSES(&rsi_timer_process);

// globals
static struct ctimer ct;
static uint16_t t = 0;

static void callback(void* ct)
{
    t++;
    printf("\rTimer expired, time since pexe: %ds", t);
    ctimer_reset(ct);
}

PROCESS_THREAD(rsi_timer_process, ev, data)
{
	PROCESS_BEGIN();
    printf("Starting timer test...\n");
    ctimer_set(&ct, CLOCK_SECOND, &callback, (void*)&ct);

	PROCESS_END();
}
