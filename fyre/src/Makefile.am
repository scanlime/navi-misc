SUBDIRS = . Plugins

CSC = mcs -g

TARGET = Fyre.exe
WRAPPER = fyre

pluginsdir = $(pkglibdir)/Plugins
CSFLAGS =		\
	-target:exe

CSFILES =			\
	Element.cs		\
	ElementTooltip.cs	\
	Pipeline.cs		\
	PipelineDrawing.cs	\
	PipelineEditor.cs	\
	PluginManager.cs

ASSEMBLIES =			\
	$(FYRE_LIBS)		\
	-r:Mono.Posix

RESOURCES =							\
	-resource:$(top_srcdir)/data/pipeline-editor.glade	\
	-resource:$(top_srcdir)/data/element-tooltip.glade

RESOURCEFILES =						\
	$(top_srcdir)/data/pipeline-editor.glade	\
	$(top_srcdir)/data/element-tooltip.glade

$(TARGET): $(CSFILES) $(srcdir)/Defines.cs $(RESOURCEFILES)
	$(CSC) -o $@ $(CSFLAGS) $(CSFILES) $(srcdir)/Defines.cs $(ASSEMBLIES) $(RESOURCES)

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

$(srcdir)/Defines.cs: $(srcdir)/Defines.cs.in Makefile
	sed -e "s|\@version\@|$(VERSION)|"     \
	    -e "s|\@datadir\@|$(datadir)|"     \
	    -e "s|\@pkglibdir\@|$(pkglibdir)|" \
	    < $< > $@

EXTRA_DIST = 			\
	$(CSFILES)		\
	$(srcdir)/Defines.cs.in	\
	$(WRAPPER).in

CLEANFILES = 		\
	$(TARGET)	\
	$(TARGET).mdb	\
	$(WRAPPER)	\
	Defines.cs
