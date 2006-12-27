import dbus

SESSION_BUS = dbus.SessionBus()
class DBusProxyProvider:
	def __init__(self):
		self.proxies = {}
	
	def create_proxy(self, service, obj_path):
		if obj_path in self.proxies:
			return
		self.proxies[obj_path] = {}
		self.proxies[obj_path]["__proxy__"] = SESSION_BUS.get_object(service, obj_path)
		self.proxies[obj_path]["__service__"] = service
		
	def get_iface(self, obj_path, iface):
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
