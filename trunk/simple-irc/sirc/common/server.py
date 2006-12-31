from telepathy import client
from telepathy.constants import *
from telepathy.interfaces import CONN_MGR_INTERFACE,CONN_INTERFACE,CHANNEL_TYPE_TEXT
import dbus
from sirc.common import channel handles

class TelepathyInterface:
    interface = None

    def __getattr__ (self, attr):
	return getattr(self.get_proxy()[self.interface], attr)

class Server(TelepathyInterface):
    def __init__ (self, bus_name, path, params):
	self.conn = client.Connection(bus_name, path,
				      ready_handler = self.ready_handler,
				      error_handler = self.error_handler)

	self.conn[CONN_INTERFACE].connect_to_signal('NewChannel', self.on_new_channel)
	self.conn[CONN_INTERFACE].connect_to_signal('StatusChanged', self.on_status_changed)
	self.path = path
	self.params = params
	self.ready
	self.channels = {}
	self.bus_name = bus_name

    def on_status_changed(self, state, reason):
	if state != CONNECTION_STATUS_CONNECTED:
	    print 'error state %i caused by %i' % (state, reason)
	    return
	self.join('#buckets')

    def join(self, where):
	handle = self.RequestHandles(CONNECTION_HANDLE_TYPE_ROOM, [where])[0]
	path = self.RequestChannel(CHANNEL_TYPE_TEXT, CONNECTION_HANDLE_TYPE_ROOM, handle, True)
	self.channels[handle] = channel.Channel(where, self, path)

    def on_new_channel(self, path, channel_type, handle_type, handle, suppress):
	print path, channel_type, handle_type, handle, suppress

    def ready_handler(self, *whatever):
	print 'ready!', whatever

    def error_handler(self, error):
	print 'Error: ', str(error[0])

class AccountParameters:
	def __init__ (self, defaults):
		copy = defaults.copy()

class ServerFactory:
    def __init__ (self):
	self.reg = client.ManagerRegistry()
	self.reg.LoadManagers()

    def __call__(self, protocol, manager=None):
	''' Generate a connection to a server.
	    TODO:   Shouldn't hard-code all those parameters.. some sort of 
		    configuration system would be good..
	'''
	if not manager:
	    managers = self.reg.GetManagers(protocol)
	    if not len(managers):
		raise ValueError('Could not find IRC connection manager')   
	    print managers

	    manager = managers[0]

        params = self.reg.GetParams(manager, protocol).copy()

	for key in params.iterkeys():    
	    params[key] = dbus.String(params[key][1])

	params['account'] = 'drfoo2'
        params['use-ssl'] = False
        params['server'] = 'irc.freenode.net'
	params['port'] = dbus.UInt32(6667)
	params['password'] = ''
	params['quit-message'] = 'as an environmentalist, i\'m against that'
	params['fullname'] = 'Al Gore'

	def callback(*whatever):
	    print 'new connection callback',whatever

	connection_manager = self.reg.GetManager(manager)[CONN_MGR_INTERFACE]
	bus_name,object_path = connection_manager.RequestConnection(protocol, params)
	connection_manager.connect_to_signal('NewConnection', callback)
    
        return Server(bus_name, object_path, params)
