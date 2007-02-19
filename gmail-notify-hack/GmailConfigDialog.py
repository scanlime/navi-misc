#!/usr/bin/env python
# -*- coding: utf-8 -*-
import pygtk
pygtk.require('2.0')
import gtk
import GmailConfig
import os
import xmllangs
import sys
import threading

LANGSXML_PATH=sys.path[0]+"/langs.xml"

class Field:
    
    def attach(self, table, i):
        self.label.set_alignment(0,0.5)
        table.attach( self.label, 0, 1, i, i+1)
        table.attach( self.field, 1, 2, i, i+1)

class TextField(Field):

    def __init__(self, label):
        self.label = gtk.Label(label)
        self.label.set_alignment(0,0)
        self.field = gtk.Entry()
        self.field.set_activates_default(gtk.TRUE)

    def set_value(self, value):
        if( value!=None):
            self.field.set_text(str(value))
        else:
            self.field.set_text("")
                                
    def get_value(self):
        return self.field.get_text()
    
class PasswordField(TextField):
    
    def __init__(self, label):
        TextField.__init__(self, label)
        self.field.set_invisible_char("*")
        self.field.set_visibility(gtk.FALSE)

class ComboField(Field):
    
    values = {}
    
    def __init__(self, label, options):
        self.label = gtk.Label(label)
        self.label.set_alignment(0,0)
        self.field = gtk.combo_box_new_text()
        
        for option in options:
            self.field.append_text(option[0])
            self.values[option[0].encode()] = option[1]
    
    def set_value(self, value):
        iter = self.field.get_model().get_iter_first()
        while iter!=None:
            item = self.field.get_model().get_value(iter,0)
            if(self.values[item]==value):
                self.field.set_active_iter(iter)
            iter = self.field.get_model().iter_next(iter)
    
    def get_value(self):
        return self.values[self.field.get_active_text()]

class CheckField(Field):
    
    def __init__(self, label):
        self.align = gtk.Alignment( 0.5)
        self.field = gtk.CheckButton(label)
        self.align.add( self.field)

    def set_value(self, value):
        self.field.set_active(int(value))
    
    def get_value(self):
        return int(self.field.get_active())

    def attach(self, table, i):
        table.attach(self.align, 0, 2, i, i+1)

class GmailConfigDialog:
    
    DIALOG_WIDTH = 200
    DIALOG_HEIGHT = 500
    
    fields = {}
    config = GmailConfig.GmailConfig()
    result = 0
    
    def __init__(self):
        self.config.load_config()
        self.username = self.config.get_option("gmailusername")
        self.password = self.config.get_option("gmailpassword")
        parser = xmllangs.LangsParser(LANGSXML_PATH)
        lang = parser.find_lang(self.config.get_option("lang"))
        
        self.elements = [ ("gmailusername",TextField(lang.get_string(2))),
                     ("gmailpassword",PasswordField(lang.get_string(22))),
                     ("checkinterval",TextField(lang.get_string(31))),
                     ("lang",ComboField(lang.get_string(4), self.get_langs()))]
        
        self.wnd = gtk.Window()
        self.wnd.set_title(lang.get_string(1))
        self.wnd.set_resizable(gtk.FALSE)
        self.wnd.set_position( gtk.WIN_POS_CENTER)
        self.wnd.connect("hide", self.destroyed)
        self.table = gtk.Table()
        self.table.set_col_spacings(5)
        self.table.set_row_spacings(5)
        
        i=0
        
        for element in self.elements:
            field = element[1]
            field.set_value(self.config.get_option(element[0]))
            field.attach(self.table, i)
            i=i+1
        
        ok_button = gtk.Button( stock=gtk.STOCK_OK)
        ok_button.set_flags(gtk.CAN_DEFAULT)
        ok_button.connect( "clicked", self.on_ok)
        self.table.attach( ok_button, 0, 1, i, i+1)
                
        cancel_button = gtk.Button( stock=gtk.STOCK_CANCEL)
        cancel_button.connect( "clicked", self.on_cancel)
        self.table.attach( cancel_button, 1, 2, i, i+1)
                    
        self.wnd.add(self.table)
        ok_button.grab_default()
    
    def get_langs(self):
        langs = []
        parser = xmllangs.LangsParser(LANGSXML_PATH)
        for lang in parser.get_all_langs():
            langs.append( ( lang.get_name(), lang.get_name() ) )
        return langs
    
    def on_cancel(self, button):
        self.wnd.hide()
            
    def on_ok(self, button):
        for element in self.elements:
            self.config.set_option(element[0], element[1].get_value())
        
        self.username = self.config.get_option("gmailusername")
        self.password = self.config.get_option("gmailpassword")
        
        if( not self.config.get_int_option("savelogin")):
            self.config.remove_option("gmailusername")
            self.config.remove_option("gmailpassword")
        
        self.config.save_config()
        self.result = 1
        self.wnd.hide()
        
    def destroyed(self, widget):
        gtk.main_quit(0)
        
    def get_username(self):
        return self.username

    def get_password(self):
        return self.password
        
    def show(self):
        self.wnd.show_all()
        gtk.main()
        return self.result

# vim: ts=4:sw=4:et
