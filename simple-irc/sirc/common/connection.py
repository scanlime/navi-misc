from telepathy import interfaces, client

class Connection(client.Connection):
	pass

class Parameters():
	def __init__ (self, ):
		self.dict = defaults.copy()

		if self.dict 

class ConnectionFactory:
	''' TODO: Make this a singleton
	'''
	def __init__ (self, protocol="irc", manager="idle"):
		self.protocol = protocol
		self.manager = manager
		self.reg = client.ManagerRegistry()
		self.reg.LoadManagers()

	def __call__ (self, params=None):
		return 
