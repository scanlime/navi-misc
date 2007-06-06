CFLAGS += `pkg-config --cflags gobject-2.0` -g

LIBS += `pkg-config --libs gobject-2.0`
LIBS += -lusb -lm

ALSA_LIBS += -lasound

all:		calibrate filter tune theremin theremin2

field-sensor.o:	field-sensor.c field-sensor.h
	gcc -c -o $@ field-sensor.c $(CFLAGS)

calibrate.o:	calibrate.c field-sensor.h
	gcc -c -o $@ calibrate.c $(CFLAGS)

calibrate:	calibrate.o field-sensor.o
	gcc -o $@ calibrate.o field-sensor.o $(LIBS)

filter.o:	filter.c field-sensor.h
	gcc -c -o $@ filter.c $(CFLAGS)

filter:		filter.o field-sensor.o
	gcc -o $@ filter.o field-sensor.o $(LIBS)

tune.o:		tune.c field-sensor.h
	gcc -c -o $@ tune.c $(CFLAGS)

tune:		tune.o field-sensor.o
	gcc -o $@ tune.o field-sensor.o $(LIBS)

theremin.o:	theremin.c field-sensor.h
	gcc -c -o $@ theremin.c $(CFLAGS)

theremin:	theremin.o field-sensor.o
	gcc -o $@ theremin.o field-sensor.o $(LIBS) $(ALSA_LIBS)

theremin2.o:     theremin2.c field-sensor.h
	gcc -c -o $@ theremin2.c $(CFLAGS)

theremin2:       theremin2.o field-sensor.o
	gcc -o $@ theremin2.o field-sensor.o $(LIBS) $(ALSA_LIBS)


