#!/usr/bin/env python
from filterlib import CommitFilter
import re, posixpath

class KdeFilter(CommitFilter):
    project = 'kde'

    def parse(self):
        # Directory name is the first token in the subject
        dirName = self.message['subject'].split(" ")[0]

        # Extract the first directory in the subject as the module name
        if dirName.find('/'):
            sections = dirName.split('/')
            self.addModule(sections[0])
            dirName = '/'.join(sections[1:])

        # Author is the last token of the first line, with a trailing colon
        self.addAuthor(self.body.readline().strip().split(" ")[-1][:-1])

        # A regex that matches lines containing file information, returning groups
        # for the status, lines added, lines removed, name, and revision.
        fileRegex = re.compile(r"  (?P<status>[UPARMC\?]) (\+(?P<addedLines>[0-9]+))? (\-(?P<removedLines>[0-9]+))? +(?P<file>[^ ]+) +(?P<revision>[0-9]+(\.[0-9]+)+)\n")

        # Read in the rest of the file, sorting lines into either log message
        # lines or file information lines.
        logLines = []
        fileInfo = []
        while True:
            line = self.body.readline()

            # Remove all those annoying CVS_SILENT thingies
            line.replace("CVS_SILENT", "")

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

        for info in fileInfo:
            self.addFile(posixpath.join(dirName, info['file']))

if __name__ == '__main__':
    KdeFilter().main()
