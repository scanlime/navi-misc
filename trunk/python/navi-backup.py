#!/usr/bin/env python
#
# Quick little python program for backing up parts of navi to DVD.
#
# This recreates the appropriate parts of the directory
# tree under a date-coded directory on the DVD:
#
# /navi/foo -> /YYYYMMDD/navi/foo
# /navi/bar/widget -> /YYYYMMDD/navi/bar/widget
#
# This also creates a date-stamped file of md5sums in both
# the /catalog directory on the DVD, and the /navi/backups/catalog directory.
#
# This uses growisofs, and assumes your burner is at /dev/dvd.
#

import time, os, sys, re, shutil


def shellEscape(s):
    return re.sub(r"([^0-9A-Za-z_\./-])", r"\\\1", s)

def shellJoin(args):
    return " ".join(map(shellEscape, args))

def md5sum(filename):
    """Compute the md5sum of a file remotely on navi"""
    sum = os.popen("ssh navi md5sum %s" % shellEscape(shellEscape(filename)),
                   "r").readline().strip()
    if not sum:
        raise IOError("md5sum on %r failed" % filename)
    return sum

class BurnFailure(Exception):
    pass

class MediaInfoFailure(Exception):
    pass


class DVDBurner:
    """DVD burner abstraction, using dvd+rw-tools"""
    def __init__(self, device="/dev/dvd"):
        self.device = device

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
            # Store the free block count of the last track we see
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
           """
        cmd = ['growisofs']
        if self.isBlank():
            # Record an initial session
            cmd.append('-Z')
        else:
            # Append to the disc
            cmd.append('-M')
        cmd.append(self.device)
        cmd.append('-R')
        cmd.append('-J')
        cmd.append('-graft-points')

        for disc, fs in pathMap.iteritems():
            cmd.append(self._graftEscape(disc) + "=" + self._graftEscape(fs))

        if os.system(shellJoin(cmd)) != 0:
            raise BurnFailure("Oh no!")


class CatalogWriter:
    """Creates catalog files from a list of paths being backed up"""
    def __init__(self, paths):
        self.paths = paths
        self.name = self._getName()
        self.temp_name = "/tmp/navi_backup_%r.catalog" % os.getpid()
        self.cd_name = os.path.join("catalog", self.name)
        self.archive_name = os.path.join("/navi/backups/catalog", self.name)

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


def map_normal_path(pathMap, fs_path):
    """Map a normal file to its datestamped equivalent directory on CD"""
    assert fs_path[0] == os.sep
    pathMap[time.strftime("%Y%m%d") + fs_path] = fs_path


def backup(burner, paths):
    """Perform a normal catalogging and backup of a list of paths"""
    paths = map(os.path.abspath, paths)

    # Generate catalog
    cat = CatalogWriter(paths)
    cat.generate()

    # Map fs paths to disc paths
    pathMap = {}
    cat.addToDisc(pathMap)
    for path in paths:
        map_normal_path(pathMap, path)

    # Burn the DVD. If that worked, archive the catalog
    burner.burn(pathMap)
    cat.archive()


def main():
    paths = sys.argv[1:]

    backup(DVDBurner(), paths)

if __name__ == "__main__":
    main()

### The End ###
