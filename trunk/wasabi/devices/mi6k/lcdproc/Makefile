LCDPROC_DIR = /home/micah/software/lcdproc-cvs

CFLAGS += -Wall -O3 -DHAVE_CONFIG_H
CFLAGS += -I$(LCDPROC_DIR) -I$(LCDPROC_DIR)/server/drivers
LDFLAGS += -shared

SRC = mi6k.c $(LCDPROC_DIR)/server/drivers/lcd_lib.c

mi6k.so: mi6k.h $(SRC)
	gcc $(CFLAGS) $(LDFLAGS) -o mi6k.so $(SRC)

clean:
	rm -f mi6k.so