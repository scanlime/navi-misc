if VALID_VIDEO
camserv_subdir = camserv
endif

datadir = @datadir@/camserv

SUBDIRS = 	libltdl \
		common \
		${camserv_subdir} \
		include \
		macros  \
		relay

data_DATA = camserv.cfg defpage.html
EXTRA_DIST = camserv.cfg.in javascript.txt BUGS camserv.spec defpage.html

camserv.cfg: camserv.cfg.in
	@sed -e s%CAMSERV_MODDIR%$(CAMSERV_MODDIR)%g < camserv.cfg.in > $@ 
