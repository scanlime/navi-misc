#!/usr/bin/env python
"""
This is a utility for backing up parts of navi into date-coded
directories on DVD, with MD5 catalogs stored both on DVD and
on navi itself.
This uses growisofs, and assumes your burner is at /dev/dvd.

 Usages:

  * navi-backup store <paths>
    Archive all paths on the current DVD, without checking for
    available space or preexisting backups

  * navi-backup status <paths>
    Using the catalogs on navi, show the backup status of all files
    in the given paths

  * navi-backup auto <paths>
    Find files under the given paths that need backing up, sort
    them, and fit as many as possible onto the remaining space
    on the current DVD.

If the path list is omitted, the current directory is assumed.
Note that this uses an on-disk md5sum cache, to avoid horrible
slowness a lot of the time. If you have reason to believe that
any files may have been corrupted or updated, the md5 cache
(at ~/.navi_backup_md5cache) should be removed.

-- Micah Dowty <micah@navi.cx>
"""

import time, os, sys, re, shutil, popen2


def shellEscape(s):
    return re.sub(r"([^0-9A-Za-z_\./-])", r"\\\1", s)


class MD5Cache:
    """In-memory and on-disk caches for md5sums. Sums we don't have
       are computed on navi and returned via ssh. The disk cache
       is awful cheesy, but better than waiting on a new md5sum.
       """
    def __init__(self, filename="~/.navi_backup_md5cache"):
        self.cache = {}
        self.filename = os.path.expanduser(filename)
        try:
            self.readDiskCache()
        except IOError:
            pass

    def readDiskCache(self):
        for line in open(self.filename):
            line = line.strip()
            self.cache[line.split()[1]] = line

    def updateDiskCache(self, line):
        open(self.filename, "a").write(line + "\n")

    def calc(self, filename):
        """Calculate an md5sum remotely"""
        sys.stderr.write("Waiting on md5sum...")
        sum = os.popen("ssh navi md5sum %s" % shellEscape(shellEscape(filename)),
                       "r").readline().strip()
        if not sum:
            raise IOError("md5sum on %r failed" % filename)
        sys.stderr.write("\r" + " "*30 + "\r")
        return sum

    def get(self, filename):
        """Get a sum from the cache, calculating if necessary"""
        filename = os.path.abspath(filename)
        try:
            return self.cache[filename]
        except KeyError:
            sum = self.calc(filename)
            self.cache[filename] = sum
            self.updateDiskCache(sum)
            return sum

md5sum = MD5Cache().get


class BurnFailure(Exception):
    pass

class MediaInfoFailure(Exception):
    pass


class DVDBurner:
    """DVD burner abstraction, using dvd+rw-tools"""
    def __init__(self, device="/dev/dvd"):
        self.device = device
        self.messageMode = None

    def iterMediaInfo(self):
        """Iterate over the key, value pairs returned by dvd+rw-mediainfo"""
        for line in os.popen("dvd+rw-mediainfo /dev/dvd", "r"):
            l = line.split(":", 1)
            if len(l) > 1:
                key = l[0].strip()
                value = l[1].strip()
                yield key, value

    def getFreeSpace(self):
        """Returns the amount of free space on the current disc, in bytes"""
        space = 0
        for key, value in self.iterMediaInfo():
            # Store the free block count of the last track we see
            if key == "Free Blocks":
                assert value.endswith("*2KB")
                blocks = int(value[:-4])
                space = blocks * 2 * 1024
        return space

    def isBlank(self):
        """Determines whether the current disc is blank"""
        for key, value in self.iterMediaInfo():
            # Store http://www.google.com/search?&q=python%20popen3&sourceid=firefoxthe free block count of the last track we see
            if key == "Disc status":
                if value == "appendable":
                    return False
                elif value == "blank":
                    return True
                else:
                    raise MediaInfoFailure("Disc is neither appendable nor blank, it is %r" % value)
        raise MediaInfoFailure("No disc status found")

    def _graftEscape(self, s):
        """Escape a path for use with mkisofs graft points"""
        return s.replace("\\", "\\\\").replace("=", "\\")

    def burn(self, pathMap):
        """Burn a new session to the current disc, with the given dict
           mapping paths on the disc to paths in our filesystem.

           This invokes growisofs (and therefore mkisofs) in a subprocess,
           feeding in a list of path mappings via stdin and getting
           progress information via stderr.
           """
        if self.isBlank():
            # Record an initial session
            sessionOption = '-Z'
        else:
            # Append to the disc
            sessionOption = '-M'

        cmd = 'growisofs %s %s -R -J -graft-points -path-list -' % (
            sessionOption, self.device)
        subproc = popen2.Popen4(cmd)

        # Write the path mapping list and close stdin
        for disc, fs in pathMap.iteritems():
            subproc.tochild.write(self._graftEscape(disc) + "=" + self._graftEscape(fs) + "\n")
        subproc.tochild.close()

        # Read all the output until stdout/stderr is closed
        while 1:
            line = subproc.fromchild.readline()
            if not line:
                break
            self.parseBurnOutput(line.strip())

        retval = subproc.wait()
        if retval != 0:
            raise BurnFailure("%r failed with error code %r" % (cmd, retval))

    def parseBurnOutput(self, line):
        """This receives lines of output from growisofs and tries to make sense of them"""
        match = re.match(r"\s*(\d+\.\d+)\% done, estimate finish (.*)", line)
        if match:
            percent = float(match.group(1))
            finishTime = time.mktime(time.strptime(match.group(2),
                                                   "%a %b %d %H:%M:%S %Y"))
            return self.burnProgress(percent, finishTime)

        self.burnMessage(line)

    def burnMessage(self, line):
        """Unparsed messages from our burn command go here"""
        self.setMessageMode("other")
        print line

    def getProgressBar(self, percent, width=50):
        """Create a textual progress bar representing some percentage completion"""
        complete = int(percent / 100.0 * width + 0.5)
        return "#"*complete + "-"*(width-complete)

    def burnProgress(self, percent, finishTime):
        """This receives burn progress updates, in the form of a percent
           completion and a projected completion time.
           """
        self.setMessageMode("progress")
        now = time.time()
        remaining = max(0, finishTime - now)
        status = "%7.2f%% %s (%d:%02d remaining)" % (
            percent, self.getProgressBar(percent), int(remaining/60),
            int(remaining)%60)
        sys.stderr.write("\r" + " "*75 + "\r" + status + " ")

    def setMessageMode(self, mode):
        """Keep track of what we're outputting, and put in a separator
           if it changes.
           """
        if mode != self.messageMode:
            if self.messageMode == "progress":
                # An extra blank line, since the progress meter didn't have a \n
                print
            print
        self.messageMode = mode


class CatalogWriter:
    """Creates catalog files from a list of paths being backed up"""
    catalogDir = "/navi/backups/catalog"

    def __init__(self, paths):
        self.paths = paths
        self.name = self._getName()
        self.temp_name = "/tmp/navi_backup_%r.catalog" % os.getpid()
        self.cd_name = os.path.join("catalog", self.name)
        self.archive_name = os.path.join(self.catalogDir, self.name)

    def _getName(self):
        """Determine the name of our catalog, based on the current time and
        on the common prefix shared by all our paths.
        """
        prefix = os.path.commonprefix(self.paths).replace(os.sep, '.')
        assert prefix[0] == '.'
        return time.strftime("%Y%m%d_%H%M%S") + prefix

    def addToDisc(self, pathMap):
        """Add this catalog to a path map used for DVD mastering"""
        pathMap[self.cd_name] = self.temp_name

    def generate(self):
        """Generate the catalog in a temporary file"""
        f = open(self.temp_name, "w")
        for path in self.paths:
            self._indexTree(f, path)
        f.close()

    def archive(self):
        """Copy the catalog to navi for archiving"""
        shutil.move(self.temp_name, self.archive_name)

    def _indexTree(self, f, path):
        if os.path.isfile(path):
            self._indexFile(f, path)
        else:
            for root, dirs, files in os.walk(path):
                for name in files:
                    self._indexFile(f, os.path.join(root, name))

    def _indexFile(self, f, path):
        sum = md5sum(path)
        print sum
        f.write(sum + "\n")


def flattenPaths(paths):
    """From a list of paths, generates a list of full filenames in sorted order"""
    for path in paths:
        if os.path.isfile(path):
            yield path
        else:
            for root, dirs, files in os.walk(path):
                dirs.sort()
                files.sort()
                for name in files:
                    yield os.path.join(root, name)


class CatalogIndex:
    """This is a slow junk-heap for searching the backup catalog.
       It could be made lots more efficient, but given the speed of
       my computer and the size of my CD binder, this shouldn't matter.
       """
    def __init__(self):
        self.empty()
        self.reindexAll()

    def empty(self):
        self.md5Map = {}
        self.dateMap = {}

    def reindexAll(self):
        """Reindex all catalog files, in order"""
        catalogs = os.listdir(CatalogWriter.catalogDir)
        catalogs.sort()
        for catalog in catalogs:
            self.reindexFile(os.path.join(CatalogWriter.catalogDir, catalog))

    def prettifyDate(self, date):
        """Convert the filename date codes into a tuple of easier
           to read date and time codes.

           >>> prettifyDate('20041101_204327')
           ['2004-11-01', '20:43:27']
           """
        return re.sub(r"(....)(..)(..)_(..)(..)(..)",
                      r"\1-\2-\3 \4:\5:\6", date).split()

    def reindexFile(self, filename):
        # Extract the date from the catalog's filename
        currentDate = self.prettifyDate(os.path.split(filename)[1].split('.', 1)[0])

        # Save the md5sum and date for each file we find within
        for line in open(filename):
            sum, filename = line.strip().split()
            self.dateMap[filename] = currentDate
            self.md5Map[filename] = sum

    def isFileModified(self, filename):
        """Check whether a file has been modified since it was archived"""
        filename = os.path.abspath(filename)
        storedSum = self.md5Map[filename]
        currentSum = md5sum(filename).split()[0]
        return currentSum != storedSum

    def needsBackup(self, filename):
        """Return True if the given file needs a new backup"""
        try:
            return self.isFileModified(filename)
        except KeyError:
            return True

    def fileStatus(self, filename):
        """Show a status line for one file. This starts with a one-character
           status code: '?' if the file is unknown, 'M' if it has been modified
           since the backup, and ' ' if it matches the backup. This is followed
           by a backup timestamp if one exists, then the filename.
           """
        try:
            backupDate = self.dateMap[os.path.abspath(filename)][0]
        except KeyError:
            status = '?'
            backupDate = ''
        else:
            if self.isFileModified(filename):
                status = 'M'
            else:
                status = ' '
        print "%1s %10s  %s" % (status, backupDate, filename)


def backup(paths, burner):
    """Perform a normal catalogging and backup of a list of paths"""
    paths = map(os.path.abspath, paths)

    # Generate catalog
    cat = CatalogWriter(paths)
    cat.generate()

    # Map fs paths to disc paths
    pathMap = {}
    cat.addToDisc(pathMap)
    prefix = time.strftime("%Y%m%d")
    for path in paths:
        assert path[0] == os.sep
        pathMap[prefix + path] = path

    # Burn the DVD. If that worked, archive the catalog
    burner.burn(pathMap)
    cat.archive()


def cmd_store(paths):
    backup(paths, DVDBurner())


def cmd_status(paths):
    index = CatalogIndex()
    for path in flattenPaths(paths):
        index.fileStatus(path)


def cmd_auto(paths, assumedOverhead = 0.01, safetyTimer = 3):
    index = CatalogIndex()
    burner = DVDBurner()
    burnPaths = []

    remaining = burner.getFreeSpace()
    print "Remaining space on disc: %.1fMB" % (remaining / (1024.0 * 1024.0))
    print "Assuming %r%% overhead" % (assumedOverhead*100.0)
    remaining -= assumedOverhead * remaining
    total = 0

    for path in flattenPaths(paths):
        # Show status of all files, to give visual indication of what we're scanning.
        # Only those marked with a '?' or 'M' will be backed up.
        index.fileStatus(path)

        if index.needsBackup(path):
            filesize = os.stat(path).st_size
            if filesize > remaining:
                print "Stopping, disc full"
                break

            total += filesize
            remaining -= filesize
            burnPaths.append(path)

    print "Found %d files to back up, a total of %.1fMB. About %.1fMB will be left on the disc." % (
        len(burnPaths), total / (1024.0 * 1024.0), remaining / (1024.0 * 1024.0))
    if burnPaths:
        print "Starting burn in %d seconds..." % safetyTimer
        time.sleep(safetyTimer)
        backup(burnPaths, burner)


def usage():
    print __doc__
    sys.exit(0)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        usage()
    command = sys.argv[1]
    paths = sys.argv[2:]
    if not paths:
        paths = '.'
    try:
        f = globals()["cmd_%s" % command]
    except:
        usage()
    else:
        f(paths)

### The End ###
