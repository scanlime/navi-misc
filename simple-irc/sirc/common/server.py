from telepathy import client
from telepathy.interfaces import CONN_MGR_INTERFACE,CONN_INTERFACE
import dbus

class Server:
    def __init__ (self, bus_name, path):
	self.conn = client.Connection(bus_name, path,
				      ready_handler = self.ready_handler,
				      error_handler = self.error_handler)
	self.conn[CONN_INTERFACE].connect_to_signal('NewChannel', self.on_new_channel)
	self.conn[CONN_INTERFACE].connect_to_signal('StatusChanged', self.on_status_changed)
	self.path = path

    def __getattr__ (self, attr):
	return getattr(self.conn[CONN_INTERFACE], attr)

    def on_status_changed(self, *whatever):
	print 'status changed: ', whatever

    def on_new_channel(self, path, channel_type, handle_type, handle, suppress):
	print path, channel_type, handle_type, handle, suppress

    def ready_handler(self, *whatever):
	print 'ready!', whatever

    def error_handler(self, *whatever):
	print 'error!', whatever

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

	params['account'] = 'drfoo'
        params['use-ssl'] = False
        params['server'] = 'irc.freenode.net'
	params['port'] = dbus.UInt32(6667)
        params['password'] = ''
	params['quit-message'] = 'as an environmentalist, i\'m against that'
	params['fullname'] = 'Al Gore'

	def callback(*whatever):
	    print 'callback!',whatever

	connection_manager = self.reg.GetManager(manager)[CONN_MGR_INTERFACE]
	print dir(connection_manager)
	bus_name,object_path = connection_manager.RequestConnection(protocol, params)
	connection_manager.connect_to_signal('NewConnection', callback)
    
        return Server(bus_name, object_path)
