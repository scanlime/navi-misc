#!/usr/bin/env python
from filterlib import CommitFilter
import re

class KdeFilter(CommitFilter):
    project = 'kde'

    def parse(self):
        # Directory name is the first token in the subject
        dirName = self.message['subject'].split(" ")[0]

        # Author is the last token of the first line, with a trailing colon
        self.addAuthor(self.body.readline().strip().split(" ")[-1][:-1])

        # The body is the set of non-blank lines starting on the third line
        self.body.readline()
        self.slurpLog()

        # Slurp up the rest of the message looking for files
        while True:
            line = self.body.readline()
            if not line:
                break
            try:
                status, added, removed, name, revision = re.split("[ \t]+", line.strip())
            except:
                continue
            self.addFile(dirName + '/' + name)

if __name__ == '__main__':
    KdeFilter().main()
