#include "contiki.h"
#include "stdio.h"

PROCESS(rsi_timer_process, "RSI timer Process");
AUTOSTART_PROCESSES(&rsi_timer_process);

PROCESS_THREAD(rsi_timer_process, ev, data)
{
    static struct etimer et;
    static uint16_t t = 0;

	PROCESS_BEGIN();

    etimer_set(&et, 2*CLOCK_SECOND);

    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        t+=2;
        printf("\rTimer expired, time since pexe: %ds", t);
        etimer_reset(&et);
    }

	PROCESS_END();
}
