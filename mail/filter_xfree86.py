#!/usr/bin/env python
from filterlib import CommitFilter
import posixpath, re

class Xfree86Filter(CommitFilter):
    project = 'xfree86'

    def readLog(self):
        # Read the log message, until we hit a line that doesn't start with whitespace
        lines = []
        while True:
            line = self.pullLine()
            if not line:
                break
            if not line.startswith(" "):
                self.pushLine(line)
                break
            line = line.strip()
            if line:
                lines.append(line)
        self.addLog("\n".join(lines))

    def readFiles(self):
        # Read the list of files, until we hit a blank line
        while True:
            line = self.pullLine()
            if not line.strip():
                break

            # crufty!
            fileName = line[25:].strip()

            # Chop off the first segment of the path, which will be the module name
            fileName = fileName.split('/', 1)[1]
            self.addFile(fileName)

    def parse(self):
        # Parse the subject with a regex
        match = re.match(r" *CVS Update: +(?P<module>[^ ]+)( *\(branch: (?P<branch>[^\) ]+)\))? *",
                         self.message['subject'])
        if not match:
            return
        module, branch = match.group('module', 'branch')
        if module:
            self.addModule(module)
        if branch:
            self.addBranch(branch)

        # Author is the first token of the from address. Most of the from addresses
        # are @XFree86.Org, so strip that out if we have it.
        address = self.message['from'].split(' ')[0]
        try:
            # If the address is in <brackets>, strip them off
            address = address.split("<", 1)[1].split(">", 1)[0]
        except IndexError:
            pass
        address = address.replace("@XFree86.Org", "")
        self.addAuthor(address)

        # Skip lines until we get to a section we can process
        while True:
            line = self.pullLine()
            if not line:
                break

            if line == 'Log message:\n':
                self.readLog()
            elif re.match(" +Revision +Changes +Path\n", line):
                self.readFiles()
                break

if __name__ == '__main__':
    Xfree86Filter().main()
