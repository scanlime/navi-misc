CFLAGS = `opencv-config --cflags` -g -O3 -Ilib
LDFLAGS = `opencv-config --libs` -lraw1394 -ldc1394_control
CC = g++
BINS = cam_test face_tracker
LIB_OBJS = lib/cv_dc1394.o

all: $(BINS)

cam_test: src/cam_test.o $(LIB_OBJS)
	$(CC) -o $@ $+ $(LDFLAGS)

face_tracker: src/face_tracker.o $(LIB_OBJS)
	$(CC) -o $@ $+ $(LDFLAGS)
