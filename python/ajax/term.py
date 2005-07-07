from mod_python import apache
import time, random, os

def index(req):
    req.content_type = "text/html"
    return """<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<head>
    <title>Foo!</title>
    <script type="text/javascript">

function TextGrid(id, width, height) {
    self.container = document.getElementById(id);
    self.width = width;
    self.height = height;

    var line = "";
    for (var i=0;i<self.width;i++)
        line = line + " ";
    self.blankLine = line + "\\n";

    for (var i=0;i<self.height;i++)
        self.container.appendChild(
            document.createTextNode(self.blankLine));
}

TextGrid.prototype.draw = function(x, y, text) {
    var lineNode = self.container.childNodes.item(y|0);
    lineNode.replaceData(x|0, text.length, text);
}

TextGrid.prototype.clear = function() {
    for (var i=0;i<self.height;i++)
        self.container.childNodes.item(i).replaceData(0, self.width+1, self.blankLine);
}

TextGrid.prototype.scroll = function(direction) {
    if (direction) {
        for (var i=self.height-1;i>0;i--)
            self.container.childNodes.item(i).replaceData(0, self.width+1,
                self.container.childNodes.item(i-1).data);
        self.container.childNodes.item(i).replaceData(0, self.width+1, self.blankLine);
    }
    else {
        for (var i=0;i<self.height-1;i++)
            self.container.childNodes.item(i).replaceData(0, self.width+1,
                self.container.childNodes.item(i+1).data);
        self.container.childNodes.item(i).replaceData(0, self.width+1, self.blankLine);
    }
}

/* Asynchronously read from the given URL repeatedly, keeping track
 * of a position pointer returned by the server. The provided callback
 * is invoked every time a data block is read successfully. The server
 * must precede each data block with a position pointer and newline.
 * In the next request, this position pointer will be provided in a
 * query argument named 'p'.
 *
 * The 'pointer' argument is optional, leave it off if you don't
 * want to supply an argument for the first request.
 */
function readStream(url, callback, pointer)
{
    var request = new XMLHttpRequest();

    request.onreadystatechange = function() {
        if (request.readyState == 4) {
            if (request.status == 200) {
                /* Success- advance the pointer and process this data block */
                var s = request.responseText
                var i = s.indexOf("\\n");
                readStream(url, callback, s.substring(0, i));
                callback(s.substring(i+1, s.length));
            }
            else {
                /* Retry the same data block */
                readStream(url, callback, pointer);
            }
        }
    }

    pointer |= 0;
    request.open("GET", url + "?p=" + pointer, true);
    request.send(null)
}

function documentLoaded()
{
    var grid = new TextGrid("results", 80, 25);

    grid.draw(0, 0, "Poing.");

    readStream("read", function(data) {
        var lines = data.split("\\n");
        for (var i=0; i<lines.length; i++) {
            var tokens = lines[i].split("\\t");

            if (tokens[0] == "draw")
                grid.draw(tokens[1], tokens[2], tokens[3]);
            else if (tokens[0] == "clear")
                grid.clear();
            else if (tokens[0] == "scroll")
                grid.scroll(tokens[1] == "up")
        }
    });

    document.onkeydown = function(e) {
        var request = new XMLHttpRequest();
        request.open("GET", "write?k=" + e.keyCode, true);
        request.send(null);
    };
}

    </script>
</head>
<body onload="documentLoaded()">
    <pre style="border: 1px solid #888; background: #DDD; padding: 0.5em; margin: 1em;" id="results"/>
</body>
</html>
"""

_outputFile = "/tmp/term-output"
_inputFile = "/tmp/term-input"


def read(req, p=0):
    f = open(_outputFile)
    f.seek(int(p))
    while 1:
        data = f.read()
        if data:
            newPointer = f.tell()
            return "%d\n%s" % (newPointer, data)
        else:
            time.sleep(0.1)

def write(req, k):
    open(_inputFile, "a").write("%s\n" % k)
    return ""


