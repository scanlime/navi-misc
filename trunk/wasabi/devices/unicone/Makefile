PY_DIR = PyUnicone
FW_DIR = firmware/universal

BINS = \
	loader_tool \
	$(PY_DIR)/_libunicone.so \
	firmware.bin

COMMON_OBJS = \
	src/bit_file.o \
	src/progress.o \
	src/progress_console.o \
	src/unicone_device.o \
	src/sha1.o

LOADER_TOOL_OBJS = $(COMMON_OBJS) \
	src/loader_tool.o

PYUNICONE_OBJS = $(COMMON_OBJS) \
	src/progress_python.o \
	src/genesis_device.o \
	$(PY_DIR)/libunicone_wrap.o

TUSB_DIR = ../../../tusb

FIRMWARE_OBJS = \
	$(FW_DIR)/main.rel \
	$(FW_DIR)/descript.rel \
	$(FW_DIR)/fpga.rel \
	$(FW_DIR)/i2c.rel \
	$(TUSB_DIR)/src/uart_driver.rel \
	$(TUSB_DIR)/src/usb_driver.rel \
	$(TUSB_DIR)/src/util.rel

CFLAGS    += -I include -I src -I /usr/include/python2.3
LDFLAGS   += -lusb
SWIGFLAGS += -python -Isrc -Iinclude

FW_CFLAGS = -I $(TUSB_DIR)/include -I include
FW_LDFLAGS = --xram-loc 0xF800 --xram-size 0x500
FW_CC = sdcc

all: $(BINS)

loader_tool: $(LOADER_TOOL_OBJS)
	$(CC) -o $@ $(LOADER_TOOL_OBJS) $(LDFLAGS)

$(PY_DIR)/_libunicone.so: $(PYUNICONE_OBJS)
	$(CC) -o $@ $(PYUNICONE_OBJS) $(LDFLAGS) -shared

$(FW_DIR)/firmware.ihx: $(FIRMWARE_OBJS)
	$(FW_CC) -o $@ $(FIRMWARE_OBJS) $(FW_LDFLAGS)

firmware.bin: $(FW_DIR)/firmware.ihx
	objcopy -I ihex -O binary $< $@

%.rel: %.c include/*.h $(FW_DIR)/*.h
	$(FW_CC) -o $@ -c $< $(FW_CFLAGS)

%.o: %.c include/*.h src/*.h
	$(CC) -o $@ -c $< $(CFLAGS)

$(PY_DIR)/libunicone_wrap.c: $(PY_DIR)/libunicone.i include/*.h src/*.h
	swig $(SWIGFLAGS) $<

clean:
	rm -f $(BINS)
	rm -f src/*.o $(PY_DIR)/libunicone_wrap.c
	rm -f $(FW_DIR)/*.ihx $(FW_DIR)/*.lnk $(FW_DIR)/*.lst $(FW_DIR)/*.map
	rm -f $(FW_DIR)/*.mem $(FW_DIR)/*.rel $(FW_DIR)/*.rst $(FW_DIR)/*.sym
	rm -f $(FW_DIR)/*.asm $(FW_DIR)/*.bin $(FW_DIR)/*.rel
