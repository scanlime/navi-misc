import pynotify
import time

def callback():
    print 'hi'

pynotify.init(' ')
n = pynotify.Notification('Test', 'Message')
n.connect_object_after(callback)
print dir(n)
n.show()

time.sleep(13513)
