#include "contiki.h"
#include "stdio.h"

//#include "dev/button-sensor.h"
//#include "dev/leds.h"

#define RSI_DBG
#include "rsi-dbg.h"

//// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//// cfg section
//// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//// a - a is bla
//#define RSI_A
//// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//
//typedef enum {
//    A,
//    B
//} enum_name_t;

///////////////////////////////////////////////////////////

PROCESS(rsi_main, "rsi-main");
AUTOSTART_PROCESSES(&rsi_main);
PROCESS_THREAD(rsi_main, ev, data)
{
	PROCESS_BEGIN();

//    // enable buttons
//    dbg(("rsi:activating button sensor\n"));
//    SENSORS_ACTIVATE(button_sensor);
//    dbg(("rsi:button sensor activated\n"));
//
//    // start other processes
//    process_start(&rsi_led_toggle_process, NULL);
//
//    while(1)
//    {
//        PROCESS_WAIT_EVENT_UNTIL(
//            ev == sensors_event &&
//            data == &button_sensor);
//        button_counter++;
//        printf("New button press. Total: %d", button_counter);
//    }

	PROCESS_END();
}

