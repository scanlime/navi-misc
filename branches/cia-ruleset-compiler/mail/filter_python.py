#!/usr/bin/env python
from filterlib import CommitFilter
import posixpath

class PythonFilter(CommitFilter):
    project = 'python'

    def readFiles(self, directory):
        # We're in an added, removed, or modified files section. Split the space-separated
        # list of files up and add them to the message, stopping when we hit a line that
        # doesn't begin with whitespace.
        while True:
            line = self.pullLine()
            if not (line.startswith('\t') or line.startswith(' ')):
                self.pushLine(line)
                break
            line = line.strip()

            # Is this line setting the tag/branch?
            if line.startswith("Tag: "):
                self.addBranch(line.split(" ", 1)[1])
                continue

            for file in line.split(' '):
                self.addFile(posixpath.join(directory, file))

    def readLog(self):
        # Read the log message, until we hit the beginning of the diffs
        # if the message has them.
        lines = []
        while True:
            line = self.pullLine()
            if not line:
                break
            if line.startswith("Index:"):
                self.pushLine(line)
                break
            line = line.strip()
            if line:
                lines.append(line)
        self.addLog("\n".join(lines))

    def parse(self):
        # Directory name is the second token in the subject. Only the part
        # after the first slash is the actual directory name, the first part
        # is the module.
        module = self.message['subject'].strip().split(" ")[1]
        try:
            module, dirName = module.split('/', 1)
        except ValueError:
            dirName = ''
        self.addModule(module)

        # Author is the first token of the from address. Authors are
        # in the form of an email address. Usually this is a sourceforge
        # account, so simplify it if we can.
        # are @users.sourceforge.net, so strip that out if we have it.
        address = self.message['from'].split(' ')[0]
        try:
            # If the address is in <brackets>, strip them off
            address = address.split("<", 1)[1].split(">", 1)[0]
        except IndexError:
            pass
        for host in ("@users.sourceforge.net",
                     "@projects.sourceforge.net"):
            address = address.replace(host, "")
        self.addAuthor(address)

        # Skip lines until we get to a section we can process
        while True:
            line = self.pullLine()
            if not line:
                break

            if line.endswith(' Files:\n'):
                self.readFiles(dirName)
            elif line == 'Log Message:\n':
                self.readLog()
                break

if __name__ == '__main__':
    PythonFilter().main()
