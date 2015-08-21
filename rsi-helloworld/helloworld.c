#include "contiki.h"
#include "stdio.h"

PROCESS(helloworld_process, "Hello World Process");
AUTOSTART_PROCESSES(&helloworld_process);

PROCESS_THREAD(helloworld_process, ev, data)
{
	PROCESS_BEGIN();

	printf("============\n");
	printf("Hello World!\n");
	printf("============\n");

	PROCESS_END();
}
