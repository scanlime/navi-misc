# From http://discussion.forum.nokia.com/forum/showthread.php?threadid=63694
# Very slightly modified to work with my HTTP proxy.

# s60_gmap version 0.4
from appuifw import *
from key_codes import *
from graphics import Image
from os.path import exists
from urllib import urlretrieve
import e32, urllib

urllib._urlopener = urllib.FancyURLopener(
    {'http': 'http://flapjack.navi.cx:143'})

app.screen = 'full'
app.body = c= Canvas()

fmt_file = u'E:\\system\\data\\maps\\%s-%s-%s'
x, y, z = 10*256, 24*256, 11
mode = 'map'
cache = {'map': {}, 'sat': {} }
ext = {'map':'.png', 'sat':'.jpg'}

def draw():
    gx, ox = divmod(x, 256)
    gy, oy = divmod(y, 256)
    tile(gx, gy, -ox, -oy)  # top-left tile and 3 optional tiles
    if ox > 80:
        tile(gx+1, gy, 256-ox, -oy)
    if oy > 48:
        tile(gx, gy+1, -ox, 256-oy)
    if ox > 80 and oy > 48:
        tile(gx+1, gy+1, 256-ox, 256-oy)

def tile(tx, ty, left, top):
    t = (tx, ty, z)
    rect = (left, top, left+256, top+256)
    # here we use 2 level cache
    if t in cache[mode]:
        im = cache[mode][t]
    else:
        f = fmt_file % t + ext[mode]
        if not exists(f):
            c.rectangle(rect, fill=0xffffff)
            c.text((left+5, top+20), u'Loading')
            urlretrieve(tile_url(t), f)   # url cache
        cache[mode][t] = im = Image.open(f)   # object cache
    c.blit(im, target=rect)

def tile_url(t):
    if mode=='map':
        return 'http://mt.google.com/mt?v=w2.5&x=%s&y=%s&zoom=%s' % t
    quadtree = []  # calculate satellite quadtree url
    m = {(0,0):'q', (0,1):'t', (1,0):'r', (1,1):'s'}
    _x, _y, z = t
    for i in range(17-z):
        _x, rx = divmod(_x, 2)
        _y, ry = divmod(_y, 2)
        quadtree.insert(0, m[(rx,ry)])
    return 'http://kh.google.com/kh?v=3&t=t' + ''.join(quadtree)

def move(dx,dy):
    global x, y
    x += dx * 50
    y += dy * 50
    draw()

def zoomin():
    global x,y,z
    x = x*2 + 88
    y = y*2 + 104
    z = z-1
    draw()

def zoomout():
    global x,y,z
    x = x/2 - 44
    y = y/2 - 52
    z = z+1
    draw()

def toggle_mode():
    global mode
    if mode == 'map':
        mode = 'sat'
    else:
        mode = 'map'
    draw()

c.bind(EKeyRightArrow,lambda:move(1, 0))
c.bind(EKeyLeftArrow,lambda:move(-1, 0))
c.bind(EKeyUpArrow,lambda:move(0, -1))
c.bind(EKeyDownArrow,lambda:move(0, 1))
c.bind(EKeySelect, zoomin)
c.bind(EKeyStar, zoomout)
c.bind(EKeyHash, toggle_mode)

running = 1
def quit():
    global running
    running = 0
app.exit_key_handler= quit

draw()
while running:
    e32.ao_sleep(0.1)
