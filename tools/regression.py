#!/usr/bin/env python
"""
This script gathers memory and CPU usage from a range
of CIA revisions, generating a CSV file with
the results. This CSV file can be graphed to quickly
analyze the performance effects of each change to the
source code.

DO NOT use this on a server with important data, as
the database is reinitialized before each trial! Don't
use it on a server running twistd for other purposes,
or an important server of any kind. This program
may eat your pets and cause your toaster to impersonate
former US senators.

usage: regression.py first-rev last-rev output.csv
"""

import sys, os, time, re, xmlrpclib, shutil
import RandomMessage
sys.path[0] = os.path.join(sys.path[0], "..")
from LibCIA import Database

Database.init({'db': None})
dbcursor = Database.pool.connect().cursor()


def readStatements(filename):
    """Return a sequence of SQL statements from the given file"""
    lines = []
    for line in open(filename).xreadlines():
        line = line.strip()
        if not line.startswith("--"):
            lines.append(line)
    fulltext = " ".join(lines)
    for statement in fulltext.split(";"):
        statement = statement.strip()
        if statement:
            yield statement

def getProcRSS(pid):
    """Return the resident set size, in bytes, of the process ID given.
       This will only work on Linux
       """
    return int(open("/proc/%d/stat" % pid).read().split()[23]) * 4096


class SourceTree:
    """A CIA source tree. We can start and stop a server using this source,
       and run various benchmarks and database operations on it.
       """
    rulesets = [
    """
    <ruleset uri='stats://project'>
        <return path='/message/source/project'/>
    </ruleset>
    """,
    ]
       
    def __init__(self, path):
        self.path = path
        self.pid = None

    def findDbVersion(self):
        """Return the database version this source tree should use"""
        latestVersion = 0
        for f in os.listdir(os.path.join(self.path, "database")):
            m = re.match("init-([0-9]+).sql", f)
            if m:
                latestVersion = max(latestVersion, int(m.group(1)))
        return latestVersion

    def initDatabase(self):
        """Initialize the database to this source tree's DB version"""
        version = self.findDbVersion()
        try:
            dbcursor.execute("DROP DATABASE cia")
        except:
            # The database is probably already gone.. if there's
            # a more serious problem we'll see it soon.
            pass
        for statement in readStatements("../database/init-%d.sql" % version):
            dbcursor.execute(statement)

    def findTacFile(self):
        """Return the name of the .tac file we use in this CIA version"""
        for name in ("cia_full.tac", "cia_server.tac"):
            if os.path.isfile(os.path.join(self.path, name)):
                return name

    def startServer(self):
        """Start a CIA server with this source, save its PID"""
        self.stopServer()
        time.sleep(3)
        os.system("cd %s; twistd -oy %s" % (
            self.path, self.findTacFile()))
        self.pid = int(open(os.path.join(self.path, "twistd.pid"))
                       .read().strip())

    def stopServer(self):
        if self.pid:
            try:
                os.kill(self.pid, 15)
                self.pid = None
                time.sleep(2)
            except OSError:
                pass
        os.system("killall -9 twistd")

    def loadRulesets(self, server):
        """Load stats rulesets for benchmarking"""
        key = open(os.path.expanduser("~/.cia_key")).read()
        for ruleset in self.rulesets:
            server.ruleset.store(key, ruleset)

    def benchmark(self):
        """Reset the database, start the server, run a benchmark on it,
           and shut it down. Returns a (messages/sec, resident bytes) tuple.
           """
        self.initDatabase()
        self.startServer()
        time.sleep(3)

        server = xmlrpclib.ServerProxy("http://localhost:3910", allow_none=True)
        self.loadRulesets(server)

        speed = RandomMessage.benchmark(server)
        memory = getProcRSS(self.pid)

        self.stopServer()

        return (speed, memory)


class Workspace:
    """Working area for checking out CIA revisions"""
    def __init__(self, 
                 format="/tmp/cia-regression-%d",
                 repos="http://navi.cx/svn/misc/trunk/cia"):
        self.path = format % os.getpid()
        self.repos = repos

    def isRevApplicable(self, revision):
        """Determine whether the given revision is applicable to
           CIA's source tree. Returns True if there were any changes
           to the 'cia' directory in it.
           """
        log = os.popen("svn log -r %d %s" % (revision, self.repos)).readlines()
        if len(log) > 1:
            for line in log:
                print line
            return True
        else:
            print "Skipping revision %d" % revision
            return False

    def checkout(self, revision):
        """Check out one revision of CIA, return its path"""
        try:
            os.mkdir(self.path)
        except OSError:
            pass
        wc = os.path.join(self.path, "cia-%d" % revision)
        os.system("svn co -r %d %s %s" % (revision, self.repos, wc))
        return wc

    def clear(self):
        shutil.rmtree(self.path, True)


def main(firstRev, lastRev, outFileName):
    outfile = open(outFileName, "w")
    ws = Workspace()
    outfile.write("revision, speed, memory\n")
    outfile.flush()

    for rev in xrange(firstRev, lastRev+1):
        if not ws.isRevApplicable(rev):
            continue
        print "Testing revision %d..." % rev
        try:
            tree = SourceTree(ws.checkout(rev))
            speed, memory = tree.benchmark()
            outfile.write("%r, %r, %r\n" % (rev, speed, memory))
            outfile.flush()
        except:
            print sys.exc_info()
        ws.clear()
        try:
            tree.stopServer()
        except:
            pass


if __name__ == "__main__":
    main(int(sys.argv[1]), int(sys.argv[2]), sys.argv[3])

### The End ###
