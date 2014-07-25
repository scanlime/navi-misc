#!/usr/bin/env python
#
# CGI to display disk status on navi
#

import os, re

class MountPoint:
    def __init__(self, device, used, available, path):
        """All sizes specified in 1K blocks, as returned by 'df'"""
        self.device = device
        self.used = used
        self.available = available
        self.path = path

def formatSize(size):
    """Reformat a size specified in 1K blocks into something more human-readable"""
    if size < 768:
        return "%0.2f kB" % size
    elif size < 786432:
        return "%0.2f MB" % (size / 1024.0)
    elif size < 805306368:
        return "%0.2f GB" % (size / 1048576.0)
    else:
        return "%0.2f TB" % (size / 1073741824.0)

def getMountPoints():
    """This makes a call to 'df' and converts the output to a list of MountPoints"""
    f = os.popen('df -P')
    f.readline()
    mounts = []
    for line in f.readlines():
        (device, total, used, available, percent, path, whitespace) = re.split("\s+", line)
        if device != "none":
            mounts.append(MountPoint(device, long(used), long(available), path))
    f.close()
    return mounts

class DiskChart:
    """Base class for a chart in the 'charts' section"""
    def __init__(self, page):
        self.page = page

    def output(self, write):
        write('<table class="chart"><tr class="chart"><td class="chartBody">')
        self.outputBody(write)
        write('</td><td class="chartLegend">')
        self.outputLegend(write)
        write('</td></tr></table>')

    def outputBody(self, write):
        write('<img width="%s" height="%s" src="%s"/>' %
        (self.page.width, self.page.height, self.page.linkURL({
            'image': 'disk_chart',
            'percentsUsed': "+".join(map(lambda x: "%0.2f" % x, self.getPercentsUsed())),
            'percentsFree': "+".join(map(lambda x: "%0.2f" % x, self.getPercentsFree())),
        })))

    def outputLegend(self, write):
        write('%0.2f%%<br/><br/>' % self.getTotalPercentUsed())
        write('%s total<br/>' % self.getTotal())
        write('%s used<br/>' % self.getUsed())
        write('%s available<br/>' % self.getAvailable())

    def getTotalPercentUsed(self):
        total = 0
        for percent in self.getPercentsUsed():
            total += percent
        return total

class MountPointChart(DiskChart):
    """Chart to display information about one mount point"""
    def __init__(self, page, mount):
        DiskChart.__init__(self, page)
        self.mount = mount

    def getName(self):
        return self.mount.path

    def getPercentsUsed(self):
        return [self.mount.used * 100.0 / (self.mount.used + self.mount.available)]

    def getPercentsFree(self):
        return [100 - self.getPercentsUsed()[0]]

    def getTotal(self):
        return formatSize(self.mount.used + self.mount.available)

    def getUsed(self):
        return formatSize(self.mount.used)

    def getAvailable(self):
        return formatSize(self.mount.available)

if __name__ == '__main__':
    mounts = getMountPoints()

    print 'Content-type: text/xml'
    print """
<?xml version="1.0"?>
<?xml-stylesheet type="text/xsl" href="/new/htdocs/style.xsl"?>

<page title="Navi disk status" subtitle="It's big. And stuff">
  <section title="charts">
    Yay, charts.
  </section>
  <section title="mounts">"""
    print """
  </section>
</page>"""
