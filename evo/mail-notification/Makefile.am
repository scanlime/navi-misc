INCLUDES =				\
	-I$(top_srcdir)			\
	-I$(EVOLUTION_MAIL_CFLAGS)	\
	-DPLUGINDIR="\"$(plugindir)\""

@EVO_PLUGIN_RULE@

plugin_DATA =	\
	org-gnome-mail-notification.eplug	\
	mail-notification.glade

plugin_LTLIBRARIES = liborg-gnome-mail-notification.la

liborg_gnome_mail_notification_la_SOURCES =	\
	eggtrayicon.c				\
	eggtrayicon.h				\
	mail-notification.c			\
	notification-area-icon.c		\
	notification-area-icon.h

liborg_gnome_mail_notification_la_LDFLAGS = -module -avoid-version

EXTRA_DIST =	\
	org-gnome-mail-notification.eplug.in	\
	mail-notification.glade
