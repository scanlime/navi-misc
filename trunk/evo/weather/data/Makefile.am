weatherdata_in_files = Locations.xml.in
weatherdata_DATA = $(weatherdata_in_files:.xml.in=.xml)
# FIXME - should probably do some sort of intltool merge here
%.xml:	%.xml.in
	cp $< $@

EXTRA_DIST = Locations.xml.in
