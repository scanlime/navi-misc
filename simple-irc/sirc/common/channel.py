from telepathy import client
from telepathy.constants import *
from telepathy import CHANNEL_INTERFACE,CHANNEL_TYPE_TEXT,\
		      CHANNEL_TEXT_MESSAGE_TYPE_ACTION

class Channel:
    def __init__ (self, name, parent, path):
	self.name = name
	self.path = path
	self.parent = parent

	# Do the dbus+telepathy majick
	self.channel = client.Channel(parent.conn.service_name, path,
				      ready_handler = self.ready_handler,
				      error_handler = self.error_handler)
	
	# There must be a better way of doing this..
	self.channel.get_valid_interfaces().add(CHANNEL_TYPE_TEXT)
	self.channel[CHANNEL_TYPE_TEXT].connect_to_signal('Received', self.on_text_received)
	self.channel[CHANNEL_TYPE_TEXT].Send(CHANNEL_TEXT_MESSAGE_TYPE_ACTION, "waves hello")
	

    def ready_handler(self, *whatever):
	pass

    def error_handler(self, error):
	print 'Channel %s error: %s' % (self.name, error)

    def on_text_received(self, id, timestamp, sender, type, flags, text):
	print self.name, sender, text

