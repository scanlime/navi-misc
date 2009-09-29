#
# Makefile for Temporal Hex Dump.
#
# Library prerequisites:
#
#    boost
#    sqlite3
#    wxWidgets
#


PACKAGES := sqlite3
CXXFLAGS += $(shell pkg-config --cflags $(PACKAGES))
LDFLAGS += $(shell pkg-config --libs $(PACKAGES))

CXXFLAGS += $(shell wx-config --cflags)
LDFLAGS += $(shell wx-config --libs)

# Common definitions for the memtrace file format
CXXFLAGS += -I../include

BIN := thd
OBJS := \
	thd_app.o \
	thd_mainwindow.o \
	thd_timeline.o \
	progress_status_bar.o \
	log_reader.o \
	log_index.o \
	sqlite3x_command.o \
	sqlite3x_connection.o \
	sqlite3x_cursor.o \
	sqlite3x_exception.o \
	sqlite3x_transaction.o \


CXXFLAGS += -O2 -g

all: $(BIN)

$(BIN): $(OBJS)
	cc -o $(BIN) $(OBJS) $(LDFLAGS)

*.o: *.h Makefile

clean:
	rm -f $(BIN) $(OBJS)
