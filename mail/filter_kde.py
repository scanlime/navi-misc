#!/usr/bin/env python
from filterlib import CommitFilter
import re, posixpath

class KdeFilter(CommitFilter):
    project = 'kde'

    def extractModule(self, dirname):
        # Extract the CVS module from the given directory
        # name, addModule'ing the module and returning the
        # remainder of the directory name.
        # Since lots of the KDE CVS modules are gigantic,
        # this also returns the first subdirectory as
        # the 'submodule'.

        if dirname.find('/') < 0:
            return dirname

        sections = dirname.split('/')
        module = sections[0]
        del sections[0]
        self.addModule(module)
        self.xml.source.addElement('submodule', content=sections[0])
        return '/'.join(sections)

    def parse(self):
        subjectTokens = self.message['subject'].split(" ")

        # If the first token in the subject ends with a colon, that's the branch
        # and the next one is the directory- otherwise the first one is the directory
        if subjectTokens[0].endswith(':'):
            dirName = subjectTokens[1]
            self.addBranch(subjectTokens[0][:-1])
        else:
            dirName = subjectTokens[0]

        # Extract the first directory in the subject as the module name
        dirName = self.extractModule(dirName)

        # Author is the last token of the first line, with a trailing colon
        self.addAuthor(self.body.readline().strip().split(" ")[-1][:-1])

        # A regex that matches lines containing file information, returning groups
        # for the status, lines added, lines removed, name, and revision.
        fileRegex = re.compile(r"^  (?P<status>[UPARMC\?]) (\+(?P<addedLines>[0-9]+))? (\-(?P<removedLines>[0-9]+))? +(?P<file>[^ ]+) +(?P<revision>[0-9]+(\.[0-9]+)+)( +\[[^\[\]]+\])? *$\n")

        # Read in the rest of the file, sorting lines into either log message
        # lines or file information lines.
        logLines = []
        fileInfo = []
        while True:
            line = self.body.readline()

            # Remove all those annoying CVS_SILENT thingies
            for word in ('CVS_SILENT:' , 'CVS_SILENT'):
                line = line.replace(word, "")

            # Quit when we get to a line starting with "---", this means we've reached the included diffs
            if line.startswith('---'):
                break

            # End of file
            if not line:
                break

            # Skip blank lines
            strippedLine = line.strip()
            if not strippedLine:
                continue

            # Is this a file or a log line?
            fileMatch = fileRegex.match(line)
            if fileMatch:
                fileInfo.append(fileMatch.groupdict())
            else:
                logLines.append(strippedLine)

        self.addLog("\n".join(logLines))

        # Currently we only store file names, but this could be easily extended
        # to send all the provided info about each file
        for info in fileInfo:
            self.addFile(posixpath.join(dirName, info['file']))

if __name__ == '__main__':
    KdeFilter().main()
