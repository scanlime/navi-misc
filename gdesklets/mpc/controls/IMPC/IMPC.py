from libdesklets.controls import Interface, Permission

class IMPC(Interface):
    
  connect   = Permission.WRITE
  prev      = Permission.WRITE
  playpause = Permission.WRITE
  stop      = Permission.WRITE
  next      = Permission.WRITE
  progress  = Permission.READ
  length    = Permission.READ
  percent   = Permission.READ
  artist    = Permission.READ
  title     = Permission.READ
  state     = Permission.READ
