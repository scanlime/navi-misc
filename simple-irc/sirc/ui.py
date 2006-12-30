#!/usr/bin/env python

import gtk

DEFAULT_LEFT_MARGIN = 48

class TextDisplay:
    def __init__ (self):
	self.widget = gtk.TextView()
	self.widget.set_editable(False)
	self.widget.set_wrap_mode(gtk.WRAP_WORD)
	self.buffer = self.widget.get_buffer()
	self.widget.set_property('left-margin', DEFAULT_LEFT_MARGIN)

    def append(self, text):
	iter = self.buffer.get_end_iter()
	self.buffer.insert(iter, text+'\n')

    def attach_input_events_to(self, input_view):
	self.widget.connect('key-press-event', self.on_key_press_event, input_view.widget)

    def on_key_press_event(self, widget, event, input_view):
	''' Forward display box keystrokes to the input box.. this will need to be more selective.
	'''
	if event.type == gtk.gdk.KEY_PRESS:
	    input_view.emit('key-press-event', event)
	    input_view.grab_focus()

class TextInput:
    def __init__ (self):
	self.__init_widget__()
	self.__init_default_tag__()

    def __init_widget__ (self):
	self.widget = gtk.TextView()
	self.widget.set_property('can-default', True)
	self.widget.set_left_margin(DEFAULT_LEFT_MARGIN)
	self.widget.set_wrap_mode(gtk.WRAP_WORD)
	self.buffer = self.widget.get_buffer()

    def attach_send_events_to (self, display_view):
	self.display_view = display_view
	self.widget.connect('key-press-event', self.on_key_press_event)

    def __init_default_tag__ (self):
	tag = gtk.TextTag('default')
	tag.set_property('background', 'Gray')
	tag.set_property('indent', DEFAULT_LEFT_MARGIN)

	self.buffer.get_tag_table().add(tag)
	self.buffer.apply_tag(tag, self.buffer.get_start_iter(),
				   self.buffer.get_end_iter())
    
    def on_key_press_event (self, widget, event):
	if event.type == gtk.gdk.KEY_PRESS:
	    key = gtk.gdk.keyval_name(event.keyval)
	    if key == 'Return' or key == 'KP_Enter':
		if not event.state & gtk.gdk.CONTROL_MASK:
		    start = self.buffer.get_start_iter()
		    end = self.buffer.get_end_iter()
		    self.display_view.append(self.buffer.get_text(start, end))
		    self.buffer.delete(start, end)
		    return True

class Window(gtk.Window):
    def __init__ (self):
	gtk.Window.__init__(self)

	vbox = gtk.VBox()
	self.input_view = TextInput()
	self.display_view = TextDisplay()
	self.input_view.attach_send_events_to(self.display_view)
	self.display_view.attach_input_events_to(self.input_view)

	vbox.pack_start(self.display_view.widget, expand=True, fill=True)
	vbox.pack_end(self.input_view.widget, expand=False, fill=True)
	self.add(vbox)
	
	self.set_title('Chat box test')
	self.set_default_size(350,200)
	self.connect('destroy', self.on_close)

    def on_close (self, *whatever):
	print 'quit'
	gtk.main_quit()
