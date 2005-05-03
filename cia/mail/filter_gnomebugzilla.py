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


            # the format is significantly different whether this is a new bug
            # or an old one being updated
            if state == 'new':
                if cline.startswith("ReportedBy:"):
                    self.addReporter(' '.join(cline.split(' ')[1:]))
            else:
                if cline.endswith("changed:"):
                    self.addReporter(' '.join(cline.split(' ')[:-1]))

if __name__ == '__main__':
    GnomeBugzillaFilter().main()
