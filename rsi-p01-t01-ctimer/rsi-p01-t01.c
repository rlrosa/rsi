#include "contiki.h"
#include "stdio.h"

#include "dev/button-sensor.h"
#include "dev/leds.h"

#define RSI_DBG
#include "rsi-dbg.h"

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// cfg section
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// timer mode. if defined will use etimer, else ctimer
//#define RSI_LED_TOGGLE_ETIMER
// Led Toggle Seconds - toggle LEDs every this may seconds
#define LTS 1
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

typedef enum {
    USE_ETIMER,
    USE_CTIMER
} rsi_timer_mode_t;

static void rsi_toggle_leds(void)
{
    static uint16_t toggle_t_s = 0;
    leds_invert(LEDS_RED | LEDS_GREEN);
    dbg(("leds:invert[%ds]\n", toggle_t_s));
    toggle_t_s += LTS;
};

static void rsi_toggle_leds_cb(void* ct)
{
    rsi_toggle_leds();
    ctimer_reset(ct);
}

PROCESS(rsi_led_toggle_process, "rsi-led-toggle-process");
PROCESS_THREAD(rsi_led_toggle_process, ev, data)
{
    static struct etimer et;
    static struct ctimer ct;

    PROCESS_BEGIN();

    leds_init();
    // start red off, green on.
    leds_off(LEDS_RED);
    leds_on (LEDS_GREEN);
    dbg(("leds:init\n"));

    if (*(rsi_timer_mode_t*)data == USE_CTIMER)
    {
        dbg(("leds:using ctimer\n"));
        ctimer_set(&ct, CLOCK_SECOND*LTS, &rsi_toggle_leds_cb, (void*)&ct);
    }
    else
    {
        dbg(("leds:using etimer\n"));
        etimer_set(&et, CLOCK_SECOND*LTS);
        while(1)
        {
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
            rsi_toggle_leds();
            etimer_reset(&et);
        }
    }

    PROCESS_END();
}

///////////////////////////////////////////////////////////

PROCESS(rsi_main_process, "rsi_main_process");
AUTOSTART_PROCESSES(&rsi_main_process);
PROCESS_THREAD(rsi_main_process, ev, data)
{
    static uint16_t button_counter = 0;

	PROCESS_BEGIN();

    // enable buttons
    dbg(("rsi:activating button sensor\n"));
    SENSORS_ACTIVATE(button_sensor);
    dbg(("rsi:button sensor activated\n"));

    // start LED toggle process
    static rsi_timer_mode_t rsi_timer_mode;
#ifdef RSI_LED_TOGGLE_ETIMER
    rsi_timer_mode = USE_ETIMER;
#else
    rsi_timer_mode = USE_CTIMER;
#endif
    process_start(&rsi_led_toggle_process, (void*)&rsi_timer_mode);

    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(
            ev == sensors_event &&
            data == &button_sensor);
        button_counter++;
        printf("New button press. Total: %d", button_counter);
    }

	PROCESS_END();
}

