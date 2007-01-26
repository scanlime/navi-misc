#!/usr/bin/env python
# -*- coding: utf-8 -*-
import pygtk
import pango
import gtk
import xmllangs
import sys
import notifier
import pynotify

class GmailPopupBox:

	POPUP_WIDTH = 200
	POPUP_HEIGHT = 120
	POPUP_IMAGE = sys.path[0]+"/gmail-notify-icon.png"
	MAX_CHARS = 29

	popup_timer=None

	def __init__(self, config, lang):
		self.notifier = notifier
		self.lang = lang

		# create image
		pixmap = gtk.Image()
		pixmap.set_from_file(self.POPUP_IMAGE)
		self.icon_pixbuf = pixmap.get_pixbuf()

		if not pynotify.is_initted():
		    pynotify.init('gmail-notify')
		self.notify = pynotify.Notification('No new mail', '')
		self.notify.set_icon_from_pixbuf(self.icon_pixbuf)

	def hide_popup(self):
		pass

	def show_popup(self):
		self.notify.show()

	def set_connection_failed(self):
		pass
#		self.text.set_markup("<span font_desc='Normal 8'><b>" 
#					+ self.lang.get_string(15) 
#					+ "</b>\n\n" + self.lang.get_string(16) + "</span>")

	def set_new_mail( self, author, subject, snippet):
		author = self.cut_string(author, self.MAX_CHARS-len(self.lang.get_string(35)))[1:]
		subject = self.cut_string(subject, self.MAX_CHARS-len(self.lang.get_string(36)))[1:]
		snippet = self.cut_string(snippet, self.MAX_CHARS*3-len(self.lang.get_string(37)))[1:]
		self.notify = pynotify.Notification("New gmail message",
						    "<b>%s: %s</b>\n%s" % (author, subject, snippet))
		self.notify.set_icon_from_pixbuf(self.icon_pixbuf)
#		self.text.set_markup("<span font_desc='Normal 8'><b>"
#					+ self.lang.get_string(35) + "</b>  " + author + "\n<b>"
#					+ self.lang.get_string(36) + "</b> " + subject + "\n<b>"
#					+ self.lang.get_string(37) + "</b> " + snippet + "</span>")

	def set_custom_text(self, title, text):
		pass
#		self.text.set_markup("<span font_desc='Normal 8'><b>" 
#					+ title
#					+ "</b>\n\n" + text + "</span>")

	def cut_string( self, s, n ):
		ret = ""
		for st in s.split():
			if( len(ret)<n and len(ret+st)<n):
				ret = ret + " " + st
		if( " " + s != ret):
			ret = ret + "..."

		return ret

	def popup_clicked(self, signal, event):
		if( event.button==1):
			self.notifier.gotourl()
			
