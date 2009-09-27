PACKAGES := sqlite3
CXXFLAGS += $(shell pkg-config --cflags $(PACKAGES))
LDFLAGS += $(shell pkg-config --libs $(PACKAGES))

CXXFLAGS += $(shell wx-config --cflags)
LDFLAGS += $(shell wx-config --libs)

# Common definitions for the memtrace file format
CXXFLAGS += -I../include

BIN := thd
OBJS := main.o log_reader.o

CXXFLAGS += -O3 -g

all: $(BIN)

$(BIN): $(OBJS)
	cc -o $(BIN) $(OBJS) $(LDFLAGS)

*.o: *.h Makefile

clean:
	rm -f $(BIN) $(OBJS)
