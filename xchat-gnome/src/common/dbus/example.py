#! /usr/bin/python

import dbus

bus = dbus.SessionBus()
proxy_obj = bus.get_object('org.xchatgnome.service', '/org/xchatgnome/RemoteObject')
xchat = dbus.Interface(proxy_obj, 'org.xchatgnome.interface')

lst = xchat.ListGet ("users")
while xchat.ListNext (lst):
	print xchat.ListStr (lst, "nick")
xchat.ListFree (lst)
