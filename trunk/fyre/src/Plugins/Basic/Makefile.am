CSC = mcs -g

CSFLAGS =	\
	-target:library

ASSEMBLIES =		\
	$(FYRE_LIBS)	\
	-r:Mono.Posix	\
	-r:$(top_builddir)/src/Fyre.exe

TARGET = Basic.dll
TARGET_CSFILES = 		\
	MatrixMultiply.cs	\
	StochasticBlur.cs	\
	Transform.cs
TARGET_RESOURCES =					\
	-resource:$(top_srcdir)/data/MatrixMultiply.png	\
	-resource:$(top_srcdir)/data/StochasticBlur.png	\
	-resource:$(top_srcdir)/data/TwoDTransform.png

$(TARGET): $(TARGET_CSFILES) $(top_builddir)/src/Fyre.exe
	$(CSC) -o $@ $(CSFLAGS) $(ASSEMBLIES) $(TARGET_CSFILES) $(TARGET_RESOURCES)

pluginsdir = $(pkglibdir)/Plugins
plugins_DATA =	\
	$(TARGET)

EXTRA_DIST = \
	$(TARGET_CSFILES)

CLEANFILES =		\
	$(TARGET)	\
	$(TARGET).mdb
