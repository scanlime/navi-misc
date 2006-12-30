# This borrowed from cohoba

import dbus

session_bus = dbus.SessionBus()

class TelepathyInterface:
    def __init__ (self, service, path, interface_name):
	proxy = session_bus.get_object(c.service_name, object_path)
	self.interface = dbus.Interface(proxy, interface_name)
	self.path = path
	
    def __call__ (self):
	return self.interface
    
class DBusProxyProvider:
	def __init__(self):
		self.proxies = {}
	
	def create_proxy(self, service, obj_path):
		if obj_path in self.proxies:
			return
		self.proxies[obj_path] = {}
		self.proxies[obj_path]["__proxy__"] = SESSION_BUS.get_object(service, obj_path)
		self.proxies[obj_path]["__service__"] = service
		
	def get_interface(self, obj_path, iface):
		if obj_path not in self.proxies:
			print '%% Warning:asking for an unexisting proxy:', obj_path, iface, self.proxies 
			return None
			
		if iface not in self.proxies[obj_path]:
			self.proxies[obj_path][iface] = dbus.Interface(self.proxies[obj_path]["__proxy__"], iface)
		
		return self.proxies[obj_path][iface]

	def get_service(self, obj_path):
		return self.proxies[obj_path]["__service__"]
		
provider = DBusProxyProvider()
def get_proxy_provider():
	return provider
