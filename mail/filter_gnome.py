#!/usr/bin/env python
from filterlib import CommitFilter
import re, posixpath

class GnomeFilter(CommitFilter):
    project = 'gnome'

    def parse(self):
        # Skip lines until we get to a section we can process
        while True:
            line = self.pullLine()
            if not line:
                break

            if line.endswith(' files:\n'):
                self.readFiles()
            elif line.lower().strip() == 'log message:':
                self.readLog()
            elif line.startswith('URL : '):
                self.readURL(line)
            elif line.startswith('Module name:'):
                self.readModule(line)
            elif line.startswith('Changes by:'):
                self.readAuthor(line)

    def readModule(self, line):
        self.addModule(line.split("\t")[1].strip())

    def readAuthor(self, line):
        self.addAuthor(line.split("\t")[1].strip())

    def readURL(self, line):
        self.addURL(line[5:].strip())

    def readLog(self):
        # The first line is usually the date and author, if it looks like that, skip it
        line = self.pullLine()
        if not (line.strip().startswith("20") and line.strip().endswith(">")):
            self.pushLine(line)

        lines = []
        while True:
            line = self.pullLine()
            if not line:
                break
            if line.startswith("URL :") or line.startswith("_"*20):
                self.pushLine(line)
                break
            line = line.strip()
            if line:
                lines.append(line)
        self.addLog("\n".join(lines))

    def readFiles(self):
        directory = ''
        while True:
            line = self.pullLine()

            # Stop when we hit a line that doesn't start with whitespace
            if not (line.startswith('\t') or line.startswith(' ')):
                self.pushLine(line)
                break
            line = line.strip()
            tokens = line.split()

            # Is this line setting the directory?
            if len(tokens) >= 3 and tokens[1] == ':':
                directory = tokens[0]
                del tokens[:2]
                if directory == '.':
                    directory = ''

            # Add the rest of the tokens as files
            for file in tokens:
                self.addFile(posixpath.join(directory, file))

if __name__ == '__main__':
    GnomeFilter().main()
