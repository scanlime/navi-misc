#
# Gallery uploader
# -- Micah Dowty <micah@navi.cx>
#
# This was designed mostly for Python on the Symbian Series 60,
# where urllib2 is not generally available. I also had to work
# around limitations in tinyproxy by using CONNECT rather than
# using the HTTP proxy normally. Still, this code doesn't use
# anything symbian-specific so it should be applicable in other
# situations.
#
# To use this, you'll need to write a short config file that
# this looks for at "E:\system\data\gallery.conf.txt". This
# path can be changed below, if needed. My config file looks
# something like this:
#
#  inpath = E:\Images\Outbox
#  outpath = E:\Images\Uploaded
#  host = gallery.example.com
#  username = mobileuser
#  password = mypassword
#  gallery = mobilegallery
#  proxy = proxy.example.com:8080
#
# When you run the script, it uploads every image it finds in
# the "Outbox" directory, moving the files to "Uploaded"
# once they're finished.
#

import httplib

class ProxyConnection(httplib.HTTPConnection):
    def __init__(self, host, proxy):
        self.realHost = host
        self.proxy = proxy
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
                 path='/gallery/gallery_remote2.php', proxy=None):
        self.host = host
        self.path = path
        self.cookies = {}

        if proxy:
            self.http = ProxyConnection(host, proxy)
        else:
            self.http = httplib.HTTPConnection(host)
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
                     userfile=(name, "image/jpeg", data))

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

def upload(inpath, outpath, host, username, password, gallery, proxy=None):
    print "Logging in..."
    g = Gallery(host, username, password, proxy=proxy)
    print "Gallery connected"

    for file in os.listdir(inpath):
        print "Sending %r" % file
        g.add(gallery, file, open(os.path.join(inpath, file)).read())
        print "Moving %r to %r" % (file, outpath)
        os.rename(os.path.join(inpath, file), os.path.join(outpath, file))
    print "Done"

def readDict(filename):
    """Read a key=value file into a dictionary"""
    d = {}
    for line in open(filename):
        line = line.strip()
        if not line:
            continue
        k, v = line.split("=")
        d[k.strip()] = v.strip()
    return d

if __name__ == "__main__":
    upload(**readDict("E:\\system\\data\\gallery.conf.txt"))
