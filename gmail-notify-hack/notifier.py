#!/usr/bin/env python
# -*- coding: utf-8 -*-
import pygtk
pygtk.require('2.0')
import gtk
import time
import os
import sys
import warnings
import ConfigParser
import xmllangs
import GmailConfig
import GmailConfigDialog
import GmailPopupMenu
import GmailPopupBox
import GmailTrayIcon
import gmailatom

ICON_PATH=sys.path[0]+"/icon.png"
ICON2_PATH=sys.path[0]+"/icon2.png"
ICON_DISCONNECTED_PATH=sys.path[0]+"/icon-disconnected.png"
LANGSXML_PATH=sys.path[0]+"/langs.xml"

class GmailNotify:

	maintimer = None
	popup = None
	mailcheck=0
	config_dialog_visible=0
	unreadmsgcount=0
	langs = xmllangs.LangsParser(LANGSXML_PATH)

	def __init__(self):
		self.init=0
		print "Gmail Notifier v1.6.2b ("+time.strftime("%Y/%m/%d %H:%M:%S", time.localtime())+")"
		print "----------"
		# Put gmail-notify icon to all application windows
		icon = gtk.gdk.pixbuf_new_from_file(ICON_DISCONNECTED_PATH)
		gtk.window_set_default_icon_list( (icon) )
       	# Load configuration
		self.config = GmailConfig.GmailConfig()
		self.config.load_config()
		# Load language
		self.lang = self.langs.find_lang(self.config.get_option("lang"))
		print "selected language: "+self.lang.get_name()
		# Create the tray icon
		self.tray_icon = GmailTrayIcon.GmailTrayIcon(self.tray_icon_clicked)
		self.tray_icon.set_icon(ICON_PATH)
		# Create the popup menu
		self.popup_menu = GmailPopupMenu.GmailPopupMenu( self)
		# Create the popup box
		self.popup = GmailPopupBox.GmailPopupBox(self.config, self.lang)
		# Create the connection
		self.connection = gmailatom.GmailAtom(self.config.get_option("gmailusername"), self.config.get_option("gmailpassword"))
        # Check if there is a user and password, if not, show config window
		if ( (self.config.get_option("gmailusername") == None) or (self.config.get_option("gmailpassword") == None )):
			self.update_config()
		# Initialization finished
		self.init=1
		# Check mail for first time
		self.mail_check()
		# Schedule mail check
		self.maintimer=gtk.timeout_add(self.config.get_int_option('checkinterval'), self.mail_check)

	def mail_check(self, event=None):
		# If checking, cancel mail check
		if self.mailcheck==1:
			print "self.mailcheck=1"
			return gtk.TRUE
		# If popup is up, destroy it
		self.popup.hide_popup()
		self.mailcheck=1
		print "----------"
		print "checking for new mail ("+time.strftime("%Y/%m/%d %H:%M:%S", time.localtime())+")"
		while gtk.events_pending():
			gtk.main_iteration( gtk.TRUE)

		# Get new messages count
		attrs = self.has_new_messages()

		# If mail check was unsuccessful
		if attrs[0]==-1:
			self.mailcheck=0
			self.tray_icon.set_icon(ICON_DISCONNECTED_PATH)
			self.tray_icon.set_tooltip(self.lang.get_string(15))
			return gtk.TRUE
		if attrs[1]>0:
			print str(attrs[1])+" new messages"
			sender = attrs[2]
			subject= attrs[3]
			snippet= attrs[4]
			self.popup.set_new_mail( sender, subject, snippet)
			self.popup.show_popup()
		if attrs[0]>0:
			print str(attrs[0])+" unread messages"
			s = ' ' 
			if attrs[0]>1: s='s '
			self.tray_icon.set_tooltip((self.lang.get_string(19))%{'u':attrs[0],'s':s})
			self.tray_icon.set_icon(ICON2_PATH)
		else:
			print "no new messages"
			self.popup.set_custom_text(self.lang.get_string(18),"")
			self.tray_icon.set_tooltip(self.lang.get_string(18))
			self.tray_icon.set_icon(ICON_PATH)
#			self.tray_icon.hide()
		
		self.unreadmsgcount=attrs[0]
		self.mailcheck=0

		return gtk.TRUE
	
	def has_new_messages( self):
		unreadmsgcount=0
		# Get total messages in inbox
		try:
			self.connection.refreshInfo()
			unreadmsgcount=self.connection.getUnreadMsgCount()
		except:
			# If an error ocurred, cancel mail check
			print "getUnreadMsgCount() failed, will try again soon"
			return (-1,)

		sender=''
		subject=''
		snippet=''
		finalsnippet=''
		if unreadmsgcount>0:
			# Get latest message data
			sender = self.connection.getMsgAuthorName(0)
			subject = self.connection.getMsgTitle(0)
			snippet = self.connection.getMsgSummary(0)
		# Really new messages? Or just repeating...
		newmsgcount=unreadmsgcount-self.unreadmsgcount
		self.unreadmsgcount=unreadmsgcount
		if unreadmsgcount>0:
			return (unreadmsgcount, newmsgcount, sender, subject, snippet)
		else:
			return (unreadmsgcount,0, sender, subject, snippet)

	def exit(self, event):
		gtk.main_quit(0)

	def tray_icon_clicked(self, signal, event):
		if event.button==3:
			self.popup_menu.show_menu(event)
		else:
			self.popup.show_popup()

	def gotourl( self, wg=None):
		print "----------"
		print "launching browser "+self.config.get_option('browserpath')+" http://gmail.google.com"
		gnome.url_show('http://gmail.google.com/')

	def show_quota_info( self, event):
		print "Not available"
		#if self.popup==1:self.destroy_popup()
		#print "----------"
		#print "retrieving quota info"
		#while gtk.events_pending()!=0:
		#	gtk.main_iteration(gtk.TRUE)
		#try:
		#	usage=self.connection.getQuotaInfo()
		#except:
		#	if self.connect()==0:
		#		return
		#	else:
		#		usage=self.connection.getQuotaInfo()
		#self.label.set_markup("<span size='large' ><u><i>"+self.lang.get_string(6)+"</i></u></span>\n\n"+self.lang.get_string(24)%{'u':usage[0],'t':usage[1],'p':usage[2]})
		#self.show_popup()

	def update_config(self, event=None):
		if(self.config_dialog_visible):
			return
		self.config_dialog_visible=1
		# Kill all timers
		if (self.maintimer!=None):
			gtk.timeout_remove(self.maintimer)
		# Hide the popup
		self.popup.hide_popup()
		# Run the configuration dialog
		dialog = GmailConfigDialog.GmailConfigDialog()
		if( dialog.show()):
			# Load new config
			self.config.load_config()
			# Update language
			self.lang = self.langs.find_lang(self.config.get_option("lang"))
			print "language changed: "+self.lang.get_name()
			# Update popup menu
			self.popup_menu = GmailPopupMenu.GmailPopupMenu(self)
			# Update popup box
			self.popup = GmailPopupBox.GmailPopupBox(self.config, self.lang)
			# Update user/pass
			self.connection=gmailatom.GmailAtom(dialog.get_username(),dialog.get_password())
			# Check for new mail
			self.unreadmsgcount=0
			self.mail_check()
			
		# Reset timeout
		self.maintimer = gtk.timeout_add(self.config.get_int_option("checkinterval"), self.mail_check )

		self.config_dialog_visible = 0
		
	def main(self):
		gtk.main()

if __name__ == "__main__":
	warnings.filterwarnings( action="ignore", category=DeprecationWarning)
	gmailnotifier = GmailNotify()
	gmailnotifier.main()
