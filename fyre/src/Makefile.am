SUBDIRS = . Plugins

CSC = mcs -g

TARGET = Fyre.exe
WRAPPER = fyre

pluginsdir = $(pkglibdir)/Plugins
CSFLAGS =		\
	-debug		\
	-target:exe

CSFILES =			\
	CanvasElement.cs	\
	CommandManager.cs	\
	Data.cs			\
	Dialogs.cs		\
	Element.cs		\
	ElementFactory.cs	\
	ElementList.cs		\
	ElementTooltip.cs	\
	Layout.cs		\
	NavigationImage.cs	\
	Pipeline.cs		\
	PipelineDrawing.cs	\
	PipelineEditor.cs	\
	PluginManager.cs	\
	SerializationManager.cs	\
	Util.cs

ASSEMBLIES =			\
	$(FYRE_LIBS)		\
	-r:Mono.Posix		\
	-r:System.Drawing

RESOURCES =							\
	-resource:$(top_srcdir)/data/pipeline-editor.glade	\
	-resource:$(top_srcdir)/data/element-tooltip.glade	\
	-resource:$(top_srcdir)/data/alert-dialog.glade		\
	-resource:$(top_srcdir)/data/navigation.png		\
	-resource:$(top_srcdir)/data/hand-closed-data.png	\
	-resource:$(top_srcdir)/data/hand-closed-mask.png	\
	-resource:$(top_srcdir)/data/hand-open-data.png		\
	-resource:$(top_srcdir)/data/hand-open-mask.png		\
	-resource:$(top_srcdir)/data/flip.png

RESOURCEFILES =						\
	$(top_srcdir)/data/pipeline-editor.glade	\
	$(top_srcdir)/data/element-tooltip.glade	\
	$(top_srcdir)/data/alert-dialog.glade		\
	$(top_srcdir)/data/navigation.png		\
	$(top_srcdir)/data/hand-closed-data.png		\
	$(top_srcdir)/data/hand-closed-mask.png		\
	$(top_srcdir)/data/hand-open-data.png		\
	$(top_srcdir)/data/hand-open-mask.png		\
	$(top_srcdir)/data/flip.png

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
