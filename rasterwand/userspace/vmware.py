#!/usr/bin/env python
#
# Sign with various information from VMware: build breakage info,
# and EMC stock price ;)
#
# -- Micah Dowty <micah@navi.cx>
#

import sys, time, rwand, feedreader, urllib2, re, csv


def countTinderboxBreakages(branch):
    """Returns the number of tinderbox brekages for a particular branch"""
    u = urllib2.urlopen("http://tinderbox.eng.vmware.com/%s/status.html" % branch)
    return len(re.findall(r"flames1\.gif", u.read()))


def getStockQuotes(symbols):
    """Query Yahoo's stock API for current prices on one or more stock symbols.
       Returns a CSV reader object (an interable) representing the returned table.
       """
    o = urllib2.build_opener(urllib2.ProxyHandler({'http': 'http://proxy.vmware.com:3128'}))
    u = o.open("http://finance.yahoo.com/d/quotes.csv?s=%s&f=sohgpv&e=.csv" %
               ' '.join(symbols))
    return csv.reader(u)


class VMwareMenu(rwand.AutoMenuList):
    branches = ('vmcore-main', 'bfg-main', 'hosted07', 'hosted07-rel')
    stockSymbols = ('EMC',)

    def __init__(self):
        self.brokenBuilds = {}
        self.stockQuotes = {}
        rwand.AutoMenuList.__init__(self, [], root=True)

	# Clock
        self.items.append(rwand.MenuItem(feedreader.ClockRenderer()))

        # Stock quotes
        for symbol in self.stockSymbols:
            item = rwand.TextMenuItem("Updating...")
            self.stockQuotes[symbol] = item
            self.items.append(item)

        # Broken builds
        self.items.append(rwand.TextMenuItem('Broken builds:'))
        for branch in self.branches:
            item = rwand.TextMenuItem("Updating...")
            self.brokenBuilds[branch] = item
            self.items.append(item)

    def press_select(self, rwdc):
        feedreader.SetupMenu(self).activate(rwdc)

    def updateStocks(self):
        try:
            for info in getStockQuotes(self.stockSymbols):
                symbol = info[0]
                current = info[1]
                self.stockQuotes[symbol].renderer = rwand.TextRenderer(
                    "%s %s" % (symbol, current)) 
        except Exception, e:
            print "Error updating stocks: %s" % e

    def updateTinderbox(self):
        try:
           for branch in self.branches:
               self.brokenBuilds[branch].renderer = rwand.TextRenderer(
                   "%s: %s" % (branch, countTinderboxBreakages(branch)))
        except Exception, e:
            print "Error updating tinderbox: %s" % e


if __name__ == "__main__":
    rwdc = rwand.RwdClient()
    rwdc.start()
    try:
        menu = VMwareMenu()
        menu.activate(rwdc)
        try:
            while True:

                # Update tinderbox every 10 minutes, stocks every hour
                menu.updateStocks()
                for i in range(6):
                    menu.updateTinderbox()
                    time.sleep(10*60)

        except KeyboardInterrupt:
            pass
    finally:
        rwdc.stop()
        rwdc.join()
