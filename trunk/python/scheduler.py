#
# Example O(1) task scheduling algorithm.
# --Micah Dowty <micah@navi.cx>
#

import time


class Task:
    def __init__(self, priority, callback, *args):
        self.priority = priority
        self.slice = priority
        self.callback = callback
        self.args = args

    def run(self):
        self.callback(self, *self.args)
        self.slice -= 1


class Scheduler:
    """O(1) scheduler for tasks executing on a limited
      resource, with variable task priorities. Inspired
      by Linux's scheduler.
      """
    def __init__(self, *tasks):
        self.active = list(tasks)
        self.expired = []

    def schedule(self):
        # After we have no more active tasks, swap the lists
        if not self.active:
            self.active = self.expired
            self.expired = []

        # Get the next task from the end of our active list.
        # If it still has time left, run it and keep it in
        # the active list. If not, restore its timeslice
        # and put it on the expired list.
        #
        # This implementation is a bit slow for clarity's
        # sake, but if this was implemented with linked
        # lists it would be O(1).

        next = self.active.pop()
        if next.slice > 0:
            next.run()
            self.active = [next] + self.active
        else:
            next.slice += next.priority
            self.expired = [next] + self.expired


def p(task, arg):
    print "slice %s, %r" % (task.slice, arg)
    time.sleep(0.2)

sched = Scheduler(
    Task(5,   p, "Task 1"),
    Task(1,   p, "Task 2"),
    Task(0.5, p, "Task 3"),
    )

while 1:
    sched.schedule()
