#!/usr/bin/env python
#
#
#

import sys
from Captcha.Visual.Tests import PseudoGimpy
from Captcha import Factory
import BaseHTTPServer


class RequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def do_GET(self):
        pathSegments = self.path.split('/')[1:]

        # A hack so it works with a proxy configured for VHostMonster :)
        if pathSegments[0] == "vhost":
            pathSegments = pathSegments[3:]

        if pathSegments[0] == "":
            self.handleRootPage()

        elif pathSegments[0] == "images":
            self.handleImagePage(pathSegments[1])

        elif pathSegments[0] == "solutions":
            self.handleSolutionPage(pathSegments[1])

        else:
            self.handle404()

    def handle404(self):
        self.send_response(404)
        self.send_header("Content-Type", "text/html")
        self.end_headers()
        self.wfile.write("<html><body><h1>No such resource</h1></body></html>")

    def handleRootPage(self):
        self.send_response(200)
        self.send_header("Content-Type", "text/html")
        self.end_headers()

        test = self.captchaFactory.new()
        self.wfile.write("""<html>
<head>
<title>PyCAPTCHA Example</title>
</head>
<body>

<h1>PyCAPTCHA Example</h1>
<h2>%s</h2>

<img src="/images/%s"/>

</body>
</html>
""" % (test.__class__.__name__, test.id))

    def handleImagePage(self, id):
        self.send_response(200)
        self.send_header("Content-Type", "image/jpeg")
        self.end_headers()
        test = self.captchaFactory.get(id)
        test.render().save(self.wfile, "JPEG")


def main(port, captchaClass=PseudoGimpy):
    print "Starting server at http://localhost:%d/" % port
    handler = RequestHandler
    handler.captchaFactory = Factory(captchaClass)
    BaseHTTPServer.HTTPServer(('', port), RequestHandler).serve_forever()

if __name__ == "__main__":
    # The port number can be specified on the command line, default is 8080
    if len(sys.argv) >= 2:
        port = int(sys.argv[1])
    else:
        port = 8080
    main(port)

### The End ###
