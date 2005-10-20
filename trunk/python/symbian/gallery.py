import httplib, time

class ProxyConnection(httplib.HTTPConnection):
    proxy = "flapjack.navi.cx:143"

    def __init__(self, host):
        self.realHost = host
        httplib.HTTPConnection.__init__(self, host)
    
    def connect(self):
        self._set_hostport(self.proxy, None)
        httplib.HTTPConnection.connect(self)
        self._set_hostport(self.realHost, None)
        self._output('CONNECT %s:%d' % (self.host, self.port))
        self._send_output()

        response = httplib.HTTPResponse(self.sock)
        response.begin()
        if response.status != 200:
            raise GalleryError("HTTP Proxy %d: %s" % (
                response.status, response.reason))

        print "Proxy Connected"

class Gallery:
    def __init__(self, host, username, password,
                 path='/gallery/gallery_remote2.php'):
        self.host = host
        self.path = path
        self.cookies = {}
        
        self.http = ProxyConnection(host)
        self.login(username, password)

    def command(self, name, **kw):
        kw['cmd'] = name
        kw['protocol_version'] = '2.0'
        contentType, data = encodeMultipartFormdata(kw)

        self.http.putrequest('POST', self.path)
        self.http.putheader('content-type', contentType)
        self.http.putheader('content-length', str(len(data)))
        if self.cookies:
            self.http.putheader('Cookie', "; ".join(
                ["%s=%s" % kv for kv in self.cookies.iteritems()]))
        self.http.endheaders()
        self.sendData(data)

        response = self.http.getresponse()
        if response.status != 200:
            raise GalleryError("HTTP %d: %s" % (
                response.status, response.reason))

        newCookie = response.getheader("Set-Cookie", None)
        if newCookie:
            key, value = newCookie.split(";", 2)[0].split("=", 2)
            self.cookies[key] = value

        results = {}
        for line in response.read().split("\n"):
            line = line.strip()
            if not line:
                continue
            if line[0] == '#':
                continue
            try:
                key, value = line.split("=", 2)
            except ValueError:
                continue
            results[key] = value

        if results['status'] != '0':
            raise GalleryError(results['status_text'])
        return results

    def sendData(self, data, blocksize=1024):
        """Send HTTP POST data in small pieces"""
        i = 0
        while i < len(data):
            print "%d / %d" % (i, len(data))
            self.http.send(data[i:i+blocksize])
            i += blocksize

    def login(self, username, password):
        self.command('login', uname=username, password=password)

    def add(self, album, name, data):
        self.command('add-item', set_albumName=album,
                     userfile=(name + ".jpeg", "image/jpeg", data))

def encodeMultipartFormdata(kw):
    """This is from http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/146306,
       slightly modified. Encode a MIME multipart/form-data message from a
       dictionary of keywords. Normal fields have a string value, filenames
       should have a (filename, mime-type, data) tuple.
       """
    BOUNDARY = '----------ThIs_Is_tHe_bouNdaRY_$'
    L = []

    for key, value in kw.iteritems():
        if type(value) is tuple:
            filename, mimetype, data = value
            L.append('--' + BOUNDARY)
            L.append('Content-Disposition: form-data; name="%s"; filename="%s"' % (key, filename))
            L.append('Content-Type: %s' % mimetype)
            L.append('')
            L.append(data)
        else:
            L.append('--' + BOUNDARY)
            L.append('Content-Disposition: form-data; name="%s"' % key)
            L.append('')
            L.append(value)

    L.append('--' + BOUNDARY + '--')
    L.append('')
    contentType = 'multipart/form-data; boundary=%s' % BOUNDARY
    body = '\r\n'.join(L)
    return contentType, body

class GalleryError(Exception):
    pass

if 1:
    import camera, appuifw, e32

    #print "Preview"
    #previewing = True
    #def handler(event):
    #    if event['type'] == appuifw.EEventKeyDown:
    #        previewing = False
    #c = appuifw.Canvas(event_callback=handler)
    #while previewing:
    #    c.blit(camera.take_photo(size=(160,120)))
    #    e32.ao_yield()

    print "Taking photo..."
    photo = camera.take_photo()
    
    print "Resizing and saving"
    savedRes = (320, 240)
    name = time.strftime("%Y%m%d-%H%M%S")
    path = "E:\\Images\\gallery-%s.jpg" % name
    photo.resize(savedRes).save(path)
    del photo

    print "Logging in..."
    g = Gallery("zero.navi.cx", "admin", "password-goes-here")
    print "Gallery connected"
    g.add('micah-mobile', name, open(path).read())
    print "Done"

