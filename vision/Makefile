CFLAGS = `opencv-config --cflags` -g -O3 -Ilib `sdl-config --cflags` -I/usr/src/linux/include
LDFLAGS = `opencv-config --libs` -lraw1394 -ldc1394_control `sdl-config --libs`
LD = g++
CC = g++
CPPFLAGS = $(CFLAGS)

BINS = cam_test face_tracker anaglyph duck_tracker camera_calibrator camshift_demo average hdr

all: $(BINS)

cam_test: src/cam_test.o lib/cv_dc1394.o lib/cv_sdl.o
	$(LD) -o $@ $+ $(LDFLAGS)

average: src/average.o lib/cv_dc1394.o lib/cv_sdl.o
	$(LD) -o $@ $+ $(LDFLAGS)

face_tracker: src/face_tracker.o lib/cv_dc1394.o
	$(LD) -o $@ $+ $(LDFLAGS)

hdr: src/hdr.o lib/cv_dc1394.o
	$(LD) -o $@ $+ $(LDFLAGS)

anaglyph: src/anaglyph.o lib/cv_dc1394.o lib/cv_sdl.o
	$(LD) -o $@ $+ $(LDFLAGS)

duck_tracker: src/duck_tracker.o lib/cv_dc1394.o lib/cv_sdl.o
	$(LD) -o $@ $+ $(LDFLAGS)

camshift_demo: src/camshift_demo.o lib/cv_dc1394.o lib/cv_sdl.o lib/uinput_mouse.o lib/camshift_frontend.o
	$(LD) -o $@ $+ $(LDFLAGS)

camera_calibrator: src/camera_calibrator.o lib/cv_dc1394.o lib/cv_sdl.o
	$(LD) -o $@ $+ $(LDFLAGS)

clean:
	rm -f $(BINS) src/*.o lib/*.o