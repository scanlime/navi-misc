#!/usr/bin/env python
from filterlib import CommitFilter
import re

class EmacsFilter(CommitFilter):
    project = 'emacs'

    def parse(self):
        # Module name is the second token in the subject
        self.addModule(message['subject'].split(" ")[1])

        # Use the from address as the author
        self.addAuthor(message['from'])

        # The body is the set of non-blank lines starting after "Log message:"
        while True:
            line = body.readline()
            if (not line) or line.strip = "Log message:":
                break
        self.slurpLog()

if __name__ == '__main__':
    EmacsFilter().main()
