#!/usr/bin/env python

import Image, sys

def writeHtmlFromImage(output, image, scaleFactor=1):
    output.write("""<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<style type="text/css">

table {
  border: inset 0em;
  border-spacing: 0em;
}

tr {
  margin: 0em;
  padding: 0em;
}

td {
  margin: 0em;
  padding: 0em;
  width: %dpx;
  height: %dpx;
}

</style>
</head>
<body>

<table>
""" % (scaleFactor, scaleFactor))

    for y in xrange(image.size[1]):
        output.write("<tr>")
        for x in xrange(image.size[0]):
            r, g, b, a = image.getpixel((x,y))
            if a:
                output.write('<td style="background:#%02X%02X%02X;"/>' % (r,g,b))
            else:
                output.write('<td style="background:clear;"/>')
        output.write("<tr>\n")

    output.write("""
</table>
</body>
</html>
""")

if __name__ == "__main__":
    imageFile, htmlFile, scaleFactor = sys.argv[1:]
    writeHtmlFromImage(open(htmlFile, "w"), Image.open(imageFile).convert("RGBA"), int(scaleFactor))
