#!/usr/bin/env python
from filterlib import BugFilter

class GnomeBugzillaFilter(BugFilter):
    project = 'gnome'

    def parse(self):
        component = self.message['X-Bugzilla-Component']
        module    = self.message['X-Bugzilla-Product']
        priority  = self.message['X-Bugzilla-Severity']
        severity  = self.message['X-Bugzilla-Priority']
        status    = self.message['X-Bugzilla-Status']
        version   = self.message['X-Bugzilla-Version']

        state = 'old'
        if self.message['Subject'].split(' ')[2] == 'New:':
            state = 'new'
        self.addType(state)

        self.addModule(module)
        if component != 'general':
            self.addComponent(component)

        # grab info out of the body
        while True:
            line = self.pullLine()
            if not line:
                break

            cline = line.strip()

            if cline.startswith("http://bugzilla.gnome.org/"):
                self.addURL(cline)

            if cline.endswith("changed:"):
                self.readReporter(cline)

    def readReporter(self, line):
        self.addReporter(' '.join(line.split(' ')[:-1]))

if __name__ == '__main__':
    GnomeBugzillaFilter().main()
