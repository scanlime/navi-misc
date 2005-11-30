#!/usr/bin/env python

from xml.sax import make_parser
from xml.sax.handler import feature_namespaces
from xml.sax import saxutils
import sys, os
import ConfigParser
import StringIO

DataPath = "../data"
BoxesFilename = os.path.join(DataPath, "boxes.xml")
LessonsFilename = os.path.join(DataPath, "lessons.xml")

class GetBoxes(saxutils.DefaultHandler):
    def __init__(self):
        self.boxes = {}
        self.lessons = []

    def startElement(self, name, attrs):
        if name == 'box':
            title = attrs.get('title', None)
            desc = attrs.get('desc', None)
            id = attrs.get('id', None)
            self.boxes[id] = {'title': title,
                              'desc' : desc,
                              'lessons': []}
            self.currentId = id
        elif name == 'lesson':
            id = attrs.get('id', None)
            self.boxes[self.currentId]['lessons'].append(id)

    def writeXML(self, filename):
        file = open(filename, "w")
        file.write ('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
        file.write ('<boxes>\n')

        for boxID, data in self.boxes.items():
            file.write ('   <box id="%s" title="%s" desc="%s">\n' %
                (boxID, data['title'], data['desc']))
            for lessonID in data['lessons']:
                file.write('      <lesson id="%s"/>\n' % lessonID)
            file.write ('   </box>\n')

        file.write ('</boxes>\n')
        file.close()


class GetLessons(saxutils.DefaultHandler):
    def __init__(self):
        self.lessons = {}

    def startElement(self, name, attrs):
        if name == 'lesson':
            title = attrs.get('title', None)
            desc = attrs.get('desc', None)
            id = attrs.get('id', None)
            self.lessons[id] = {
                'title': title,
                'desc' : desc}

    def writeXML(self, filename):
        file = open(filename, "w")
        file.write ('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
        file.write ('<lessons>\n')

        for lessonID, data in self.lessons.items():
            file.write ('   <lesson id="%s" title="%s" desc="%s"/>\n' %
                (lessonID, data['title'], data['desc']))

        file.write ('</lessons>\n')
        file.close()

def getAllBoxes():
    parser = make_parser()
    parser.setFeature(feature_namespaces, 0)
    dh = GetBoxes()
    parser.setContentHandler(dh)

    print "Content-type: text/plain"
    print

    file = open(BoxesFilename, "r")
    parser.parse(file)
    file.close()

    for element in dh.boxes.keys():
        print "%s|%s" % (dh.boxes[element]['title'],
                         dh.boxes[element]['desc'])

def getAllLessons():
    parser = make_parser()
    parser.setFeature(feature_namespaces, 0)
    dh = GetLessons()
    parser.setContentHandler(dh)

    print "Content-type: text/plain"
    print

    file = open(LessonsFilename, "r")
    parser.parse(file)
    file.close()

    for id, data in dh.lessons.items():
        print "%s|%s|%s" % (id, data['title'], data['desc'])

def saveLesson():
    # Input is simple.  id|title|desc
    data = sys.__stdin__.read()
    id, title, desc = data.split('|')
    
    # Load up our database of lessons and add this one (or
    # modify the existing one if the ID is valid and
    # already exists).
    parser = make_parser()
    parser.setFeature(feature_namespaces, 0)
    dh = GetLessons()
    parser.setContentHandler(dh)

    file = open(LessonsFilename, "r")
    parser.parse(file)
    file.close()

    f = open ("/home/darkstar62/output.log", "w")
    f.write(str(dh.lessons))
    f.close ()

    # alright, we need to know the largest ID in the database
    # if the ID isn't valid.
    id = int(id)
    if int(id) == -1:
        maxID = -1
        for newID in dh.lessons:
            if int(newID) > maxID:
                maxID = int(newID)

        id = maxID + 1

    # Make the change.
    dh.lessons[str(id)] = {
        'title': title,
        'desc': desc}

    dh.writeXML(LessonsFilename)

    print "Content-type: text/plain\n"
    print "%s" % id

def saveBox():
    # Read in the data that the server passed us.  This
    # forms the box information that we'll use to save
    # the box.
    data = sys.__stdin__.read()

    buffer = StringIO.StringIO(data)
    config = ConfigParser.ConfigParser()
    config.readfp(buffer)
    buffer.close()

    # Parse the config data
    box = {}
    links = []
    for entry in config.items('meta'):
        box[entry[0]] = entry[1]

    for entry in config.items('links'):
        id = entry[1]
        links.append(id)

    # Load in the database of boxes.  We'll look for the
    # corresponding ID to what we were given, or create
    # a new one if no ID was specified (or if ID = -1).
    parser = make_parser()
    parser.setFeature(feature_namespaces, 0)
    dh = GetBoxes()
    parser.setContentHandler(dh)
    file = open(BoxesFilename, "r")
    parser.parse(file)
    file.close()

    # The boxes are now stored as dh.boxes, a dictionary
    # keyed to the ID.  If the ID we were given exists,
    # we just modify it with the passed data.  Otherwise,
    # we create a new ID (pased on the max ID seen + 1).
    if dh.boxes.has_key(box['id']):
        # The box was found.  We're just modifying it.
        dh.boxes[box['id']] = {
            'title': box['title'],
            'desc': box['desc'],
            'lessons': links}
        id = box['id']
    else:
        # No existing box, find the maximum ID we have
        # and add one to it.  That'll be the new ID.
        max = -1
        for entry in dh.boxes:
            if int(entry) > max:
                max = int(entry)
        id = max + 1

        # Create the new box.
        dh.boxes[str(id)] = {
            'title': box['title'],
            'desc': box['desc'],
            'lessons': links}

    # Write out the new XML file.
    dh.writeXML(BoxesFilename)

    # Return the ID of the box for the server to reference.
    print "Content type: text/plain\n"
    print "%s" % id

if __name__ == '__main__':
    import __main__

    # Figure out what action we want.
    action = sys.argv[1]

    myFunc = getattr(__main__, action)
    myFunc()

