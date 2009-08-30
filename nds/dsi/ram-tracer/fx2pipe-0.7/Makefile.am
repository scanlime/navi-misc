#CFLAGS = @CFLAGS@ -W -Wall -D_GNU_SOURCE
#CXXFLAGS = @CXXFLAGS@ -W -Wall -D_GNU_SOURCE

OPTCXXFLAGS = -W -Wall -Wformat -Woverloaded-virtual \
	-O2 -march=i586 -fno-rtti -fno-exceptions -fomit-frame-pointer \
	-D_GNU_SOURCE
OPTCFLAGS = -W -Wall -Wformat \
	-O2 -march=i586 -fomit-frame-pointer \
	-D_GNU_SOURCE

# Pass ADDFLAGS if you want to set -fmessage-length or the like. 
ADDFLAGS=""

SUBDIRS = lib usb_io fx2pipe

EXTRA_DIST = oconfig.h firmware


opt: FORCE
	$(MAKE) CFLAGS="$(OPTCFLAGS) $(ADDFLAGS)" CXXFLAGS="$(OPTCXXFLAGS) $(ADDFLAGS)" all 

checkopt: FORCE
	$(MAKE) CFLAGS="$(OPTCFLAGS) $(ADDFLAGS)" CXXFLAGS="$(OPTCXXFLAGS) $(ADDFLAGS)" check

FORCE:
