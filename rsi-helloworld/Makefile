CONTIKI_PROYECT = helloworld
all: $(CONTIKI_PROYECT)

CONTIKI = ../../..
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

include $(CONTIKI)/Makefile.include

CONTIKIDIRS += funciones
CONTIKI_SOURCEFILES += aux.c

