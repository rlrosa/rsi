#include "contiki.h"
#include "stdio.h"

#include "dev/leds.h"

#define RSI_DBG
#include "rsi-dbg.h"

#define RSI_NRESET
#include "rsi-assert.h"

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// cfg section
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// temperature sensor: if defined then sht11, else internal microcontroller
//#define RSI_USE_SHT11

// temperature sensors sampling frequency (seconds)
#define RSI_TEMPERATURE_READ_INTERVAL_S 1
// number of samples to use for computing avg
#define RSI_AVG_COUNT                   16

// events for reader-printer process comm
#define RSI_EV_NEW_READING 1
#define RSI_EV_NEW_AVG     2
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

#ifdef RSI_USE_SHT11
#include "dev/sht11-sensor.h"
#else
#include "../platform/sky/dev/temperature-sensor.h"
#endif // RSI_USE_SHT11

// to convert avg or .buf sample to SI use the following equations:
//   - sth11    : celsius = -39.60 + 0.01*SOt
//   - internal : *not implemented*
typedef struct {
    int buf[RSI_AVG_COUNT]; // temperature readings    [ADC counts]
    uint16_t ind;           // current index for .buf
    uint8_t  full;          // 1 if .buf is complete
    float avg;              // avg                     [ADC counts]
} rsi_td_t;
static rsi_td_t td;

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

// fwd declaration
PROCESS(rsi_temperature, "rsi_temperature");

PROCESS(rsi_main, "rsi_main");
AUTOSTART_PROCESSES(&rsi_main);
PROCESS_THREAD(rsi_main, ev, data)
{
	PROCESS_BEGIN();

    static float s = 0, frac;
    static int dec;
    dbg(("m:init\n"));

    leds_init();
    leds_on(LEDS_GREEN);

    // start temperature reading thread
    dbg(("m:starting tr\n"));
    process_start(&rsi_temperature, NULL);

    dbg(("m:looping\n"));
    while(1)
    {
        //dbg(("m:ev[%d]\n", ev));
        PROCESS_WAIT_EVENT();
        switch (ev)
        {
            case RSI_EV_NEW_READING:
                dbg(("m:ev[%d] - dat: %d\n", ev, *(int*)data));
                break;
            case RSI_EV_NEW_AVG:
#ifdef RSI_USE_SHT11
                // convert raw data into SI
                s = ((0.01*((rsi_td_t*)data)->avg) - 39.60);
#else // RSI_USE_SHT11
                // from datasheet:
                //   12 bit ADC - 2.5v ref
                //   VTEMP=0.00355(TEMPC)+0.986
                s = 2.5 * ((rsi_td_t*)data)->avg / (1<<12);
                s = (s - 0.986)/0.00355;
#endif // RSI_USE_SHT11
                dec  = s;
                frac = s - dec;
                printf("m:ev[%d] - avg: %d.%02u C\n",
                       ev,
                       (unsigned)(frac*100), dec);
                break;
            default:
                // ignore
                break;
        }
    }
	PROCESS_END();
}

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

PROCESS_THREAD(rsi_temperature, ev, data)
{
    PROCESS_BEGIN();

    // init td
    td.ind  = 0;
    td.full = 0;

    static struct etimer et;
    etimer_set(&et, CLOCK_SECOND*RSI_TEMPERATURE_READ_INTERVAL_S);

    int   tr;  // temperature reading
    float avg; // tmp avg
    static uint8_t processing = 0; // sanity check
    uint16_t i;

    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        // restart time immediately to take processing loop delay into account
        etimer_reset(&et);
        rsi_assert(!processing, "processing loop overrun");
        processing = 1;
        leds_on(LEDS_YELLOW);

        // pwr up for as little time as possible
        dbg(("tr:will read\n"));
#ifdef RSI_USE_SHT11
        SENSORS_ACTIVATE(sht11_sensor);
        tr = sht11_sensor.value(0);
        SENSORS_DEACTIVATE(sht11_sensor);
#else // RSI_USE_SHT11
        SENSORS_ACTIVATE(temperature_sensor);
        tr = temperature_sensor.value(0);
        SENSORS_DEACTIVATE(temperature_sensor);
#endif // RSI_USE_SHT11

        if (tr == -1)
        {
            dbg(("tr:err:failed to read from temperature sensor\n"));
            continue;
        }

        // save new data
        td.buf[td.ind] = tr;
        dbg(("tr:new reading:%d\n",tr));
        process_post(&rsi_main, RSI_EV_NEW_READING,
                     (void*) (td.buf+(td.ind)));
        td.ind = (td.ind + 1) % RSI_AVG_COUNT;

        // avg, if enough samples
        if(td.ind == 0 && !td.full)
        {
            td.full = 1;
            dbg(("tr:buf complete\n"));
        }
        if(td.full)
        {
            dbg(("tr:will avg\n"));
            avg = 0;
            for (i = 0; i < RSI_AVG_COUNT; ++i)
                avg += td.buf[i];
            avg /= RSI_AVG_COUNT;
            td.avg = avg;

            // post to printer thread
            process_post(&rsi_main, RSI_EV_NEW_AVG, (void*)&td);
        }

        // done
        processing = 0;
        leds_off(LEDS_YELLOW);
    }
    PROCESS_END();
}

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --


