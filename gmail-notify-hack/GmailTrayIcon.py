#!/usr/bin/env python
# -*- coding: utf-8 -*-
import pytrayicon
import pygtk
pygtk.require('2.0')
import gtk

class GmailTrayIcon:

	imageicon=None

	def __init__(self, callback):
		# Create the tray icon object
		self.tray = pytrayicon.TrayIcon("");
		self.eventbox = gtk.EventBox()
		self.tray.add(self.eventbox)
		self.eventbox.connect("button_release_event", callback)
		self.tooltip = gtk.Tooltips()

	def set_icon(self, filename):
		if (self.imageicon!=None):
			self.eventbox.remove(self.imageicon)

		self.imageicon = gtk.Image()
		pixbuf = gtk.gdk.pixbuf_new_from_file( filename )
		scaled_buf = pixbuf.scale_simple(24,24,gtk.gdk.INTERP_BILINEAR)
		self.imageicon.set_from_pixbuf(scaled_buf)

		self.eventbox.add(self.imageicon)
		self.tray.show_all()

	def set_tooltip(self, text):
		self.tooltip.set_tip(self.tray, text)

	def hide(self):
		self.tray.hide()

# vim: ts=4:sw=4:et
