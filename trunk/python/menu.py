import sys


def menu(title, items):
    while True:
        print title
        itemNumber = 0
        itemMap = {}
        for item, value in items.iteritems():
            itemNumber += 1
            print "%d. %s" % (itemNumber, item)
            itemMap[itemNumber] = value
        try:
            choice = int(sys.stdin.readline())
            return itemMap[choice]
        except ValueError:
            print "Not a number, you dork"
        except IndexError:
            print "Choose something on the menu!"
        print


if __name__ == '__main__':
    def runCheeseWidgets():
        print "Yay, cheese"

    def moreBeef():
        print "moo"

    r = menu("Boing", {
        "Cheese": runCheeseWidgets,
        "Beef": moreBeef,
        })
    print r
    r()
