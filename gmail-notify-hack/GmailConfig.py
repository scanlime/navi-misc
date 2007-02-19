#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import ConfigParser
import xmllangs
import sys

LANGSXML_PATH=sys.path[0]+"/langs.xml"

class GmailConfig:
	
	section = "options"

	files = [os.path.expanduser( "~/.notifier.conf" ), 
			sys.path[0]+"/notifier.conf", 
			"/etc/notifier.conf"]

	actual_file = os.path.expanduser( "~/.notifier.conf" )

	options = { "gmailusername":None, 
			    "gmailpassword":None, 
			    "browserpath":"firefox", 
			    "lang":"English", 
			    "voffset":0, 
			    "hoffset":0, 
			    "checkinterval":20000,
			    "popuptimespan":5000,
			    "vfrom":-1, 
			    "speed":2,
			    "savelogin":0 }

	def load_config(self):
		config = ConfigParser.RawConfigParser()
		readFiles = config.read( self.files )

		if( config.has_section(self.section)):
			self.actual_file = readFiles[0]
			items = config.items(self.section)
			for item in items:
				self.set_option(item[0], item[1])
			
	def save_config(self):
		config = ConfigParser.RawConfigParser()
		config.add_section("options")
		for item in self.options:
			if( self.options[item]!=None):
				config.set(self.section, item,
						    self.options[item])
		config.write(open(self.actual_file,"w"))

	def get_option(self, key):
		return self.options[key]
	
	def get_int_option(self, key):
		return int(self.options[key])

	def set_option(self, key, value):
		if( value!=None and value!=""):
				self.options[key] = str(value)
				
	def remove_option(self, key):
		self.options[key] = None
# vim: ts=4:sw=4:et
