INCLUDES =				\
	-I$(top_srcdir)			\
	-I$(EVOLUTION_MAIL_CFLAGS)

@EVO_PLUGIN_RULE@

plugin_DATA = org-gnome-mail-notification.eplug
plugin_LTLIBRARIES = liborg-gnome-mail-notification.la

liborg_gnome_mail_notification_la_SOURCES =	\
	eggmarshalers.c				\
	eggmarshalers.h				\
	eggstatusicon.c				\
	eggstatusicon.h				\
	eggtrayicon.c				\
	eggtrayicon.h				\
	mail-notification.c

liborg_gnome_mail_notification_la_LDFLAGS = -module -avoid-version

EXTRA_DIST = org-gnome-mail-notification.eplug.in
