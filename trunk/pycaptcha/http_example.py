#!/usr/bin/env python
#
#
#

from Captcha.Visual.Tests import PseudoGimpy
from Captcha import Factory
import BaseHTTPServer, urlparse, sys


class RequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def do_GET(self):
        scheme, host, path, parameters, query, fragment = urlparse.urlparse(self.path)

        # Split the path into segments
        pathSegments = path.split('/')[1:]

        # Split the query into key-value pairs
        args = {}
        for pair in query.split("&"):
            if pair.find("=") >= 0:
                key, value = pair.split("=", 1)
                args.setdefault(key, []).append(value)
            else:
                args[pair] = []

        # A hack so it works with a proxy configured for VHostMonster :)
        if pathSegments[0] == "vhost":
            pathSegments = pathSegments[3:]

        if pathSegments[0] == "":
            self.handleRootPage()

        elif pathSegments[0] == "images":
            self.handleImagePage(pathSegments[1])

        elif pathSegments[0] == "solutions":
            self.handleSolutionPage(pathSegments[1], args['word'][0])

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
<p><img src="/images/%s"/></p>
<p>
  <form action="/solutions/%s" method="get">
    Enter the word shown:
    <input type="text" name="word"/>
  </form>
</p>
</body>
</html>
""" % (test.__class__.__name__, test.id, test.id))

    def handleImagePage(self, id):
        test = self.captchaFactory.get(id)
        if not test:
            return self.handle404()

        self.send_response(200)
        self.send_header("Content-Type", "image/jpeg")
        self.end_headers()
        test.render().save(self.wfile, "JPEG")

    def handleSolutionPage(self, id, word):
        test = self.captchaFactory.get(id)
        if not test:
            return self.handle404()

        if not test.valid:
            # Invalid tests will always return False, to prevent
            # random trial-and-error attacks. This could be confusing to a user...
            result = "Test invalidated, try another test"
        elif test.testSolutions([word]):
            result = "Correct"
        else:
            result = "Incorrect"

        self.send_response(200)
        self.send_header("Content-Type", "text/html")
        self.end_headers()
        self.wfile.write("""<html>
<head>
<title>PyCAPTCHA Example</title>
</head>
<body>
<h1>PyCAPTCHA Example</h1>
<h2>%s</h2>
<p><img src="/images/%s"/></p>
<p><b>%s</b></p>
<p>You guessed: %s</p>
<p>Possible solutions: %s</p>
<p><a href="/">Try again</a></p>
</body>
</html>
""" % (test.__class__.__name__, test.id, result, word, ", ".join(test.solutions)))


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
