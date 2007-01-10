#! /usr/bin/python

import dbus

bus = dbus.SessionBus()
proxy = bus.get_object('org.gnome.Xchat', '/org/gnome/Xchat/Remote')
remote = dbus.Interface(proxy, 'org.gnome.Xchat.Connection')
path = remote.Connect ("example.py",
		       "Python example",
		       "Example of a D-Bus client written in python",
		       "1.0")
proxy = bus.get_object('org.gnome.Xchat', path)
xchat = dbus.Interface(proxy, 'org.gnome.Xchat.Plugin')

channels = xchat.ListGet ("channels")
while xchat.ListNext (channels):
	name = xchat.ListStr (channels, "channel")
	print "------- " + name + " -------"
	xchat.SetContext (xchat.ListInt (channels, "context"))
	xchat.EmitPrint ("Channel Message", ["John", "Hi there", "@"])
	users = xchat.ListGet ("users")
	while xchat.ListNext (users):
		print "Nick: " + xchat.ListStr (users, "nick")
	xchat.ListFree (users)
xchat.ListFree (channels)

print xchat.Strip ("\00312Blue\003 \002Bold!\002", -1, 1|2)

