SUBDIRS = .

CSC = mcs -g

TARGET = Fyre.exe
WRAPPER = fyre

CSFLAGS =		\
	-target:exe

CSFILES =			\
	Element.cs		\
	ElementTooltip.cs	\
	Main.cs			\
	PluginManager.cs

ASSEMBLIES =			\
	$(FYRE_LIBS)		\
	-r:Mono.Posix

RESOURCES =							\
	-resource:$(top_srcdir)/data/pipeline-editor.glade	\
	-resource:$(top_srcdir)/data/element-tooltip.glade

$(TARGET): $(CSFILES)
	$(CSC) -o $@ $^ $(ASSEMBLIES) $(RESOURCES)

tomboylibdir = $(pkglibdir)
tomboylib_DATA = $(TARGET)

bin_SCRIPTS = $(WRAPPER)
$(WRAPPER): $(srcdir)/$(WRAPPER).in Makefile
	sed -e "s|\@prefix\@|$(prefix)|g"               \
	    -e "s|\@pkglibdir\@|$(pkglibdir)|g"         \
	    -e "s|\@bindir\@|$(bindir)|g"               \
	    -e "s|\@target\@|$(TARGET)|g"               \
	    -e "s|\@wrapper\@|$(WRAPPER)|g"             \
	    -e "s|\@extragac\@|$(EXTRA_GAC_PREFIX)|g"   \
	    < $< > $@
	chmod +x $(WRAPPER)

EXTRA_DIST = 		\
	$(CSFILES)	\
	$(WRAPPER).in

CLEANFILES = 		\
	$(TARGET)	\
	$(TARGET).mdb	\
	$(WRAPPER)
