

EXAMPLE = demo


#
# These may be overriden by the user with environment varibles
#

CC ?= gcc
COPTS ?= -g -Wall

INC := -I../common -I../../include -DCWD="\"$(shell pwd)/\""

ifeq ("$(shell uname)", "Linux")
LIBS := -L../../../../lib -lCgGL -lCg -lGL -lGLU -lglut -lpthread
INC += -I../../../../include
BUILD_APP := :
else
ifeq ("$(shell uname)", "Darwin")
LIBS := -F../../../../../System/Library/Frameworks -framework Cg -framework OpenGL -framework GLUT -framework Foundation -lstdc++
BUILD_APP := perl -w ../scripts/build_app.pl
endif
endif

all : $(EXAMPLE)

clean :
	rm -f $(EXAMPLE)

run : $(EXAMPLE)
	./$(EXAMPLE)

$(EXAMPLE) : $(EXAMPLE).cpp
	$(CC) -o $@ $(INC) $(COPTS) $< $(LIBS)
	$(BUILD_APP) $@
