SUBDIRS = data .

CSC = mcs

CSFLAGS =					\
	-debug					\
	-target:library

ASSEMBLIES =					\
	$(FYRE_LIBS)				\
	-r:Mono.Posix				\
	-r:System.Drawing			\
	-r:$(top_builddir)/src/Fyre.exe

TARGET = Basic.dll
TARGET_CSFILES = 				\
	CartesianProduct.cs			\
	Equation.cs				\
	Gradient.cs				\
	Input.cs				\
	Iterative.cs				\
	MatrixMultiply.cs			\
	Pixmap.cs				\
	StochasticBlur.cs			\
	Transform.cs
TARGET_RESOURCES =				\
	-resource:data/CartesianProduct.png	\
	-resource:data/Equation.png		\
	-resource:data/Gradient.png		\
	-resource:data/Input.png		\
	-resource:data/Iterative.png		\
	-resource:data/MatrixMultiply.png	\
	-resource:data/Pixmap.png		\
	-resource:data/StochasticBlur.png	\
	-resource:data/TwoDTransform.png

$(TARGET): $(TARGET_CSFILES) $(top_builddir)/src/Fyre.exe
	$(CSC) -out:$@ $(CSFLAGS) $(ASSEMBLIES) $(TARGET_CSFILES) $(TARGET_RESOURCES)

pluginsdir = $(pkglibdir)/Plugins
plugins_DATA =					\
	$(TARGET)

EXTRA_DIST =					\
	$(TARGET_CSFILES)

CLEANFILES =					\
	$(TARGET)				\
	$(TARGET).mdb
