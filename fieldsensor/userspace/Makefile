CFLAGS += `pkg-config --cflags gobject-2.0`

LIBS += `pkg-config --libs gobject-2.0`
LIBS += -lusb

ALSA_LIBS += -lasound

all:		calibrate findchan tune theremin

field-sensor.o:	field-sensor.c field-sensor.h
	gcc -c -o $@ field-sensor.c $(CFLAGS)

calibrate.o:	calibrate.c field-sensor.h
	gcc -c -o $@ calibrate.c $(CFLAGS)

calibrate:	calibrate.o field-sensor.o
	gcc -o $@ calibrate.o field-sensor.o $(LIBS)

findchan.o:	findchan.c field-sensor.h
	gcc -c -o $@ findchan.c $(CFLAGS)

findchan:	findchan.o field-sensor.o
	gcc -o $@ findchan.o field-sensor.o $(LIBS)

tune.o:		tune.c field-sensor.h
	gcc -c -o $@ tune.c $(CFLAGS)

tune:		tune.o field-sensor.o
	gcc -o $@ tune.o field-sensor.o $(LIBS)

theremin.o:	theremin.c field-sensor.h
	gcc -c -o $@ theremin.c $(CFLAGS)

theremin:	theremin.o field-sensor.o
	gcc -o $@ theremin.o field-sensor.o $(LIBS) $(ALSA_LIBS)
