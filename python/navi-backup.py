#!/usr/bin/env python
#
# Quick little python program for backing up parts of navi to DVD.
#
# usage:   dvd-backup /navi/foo /navi/bar/widget
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

import time, os, sys, re

def escape_path(s):
    """Escape a path for use with mkisofs graft points"""
    return shell_escape(s.replace("\\", "\\\\").replace("=", "\\"))

def shell_escape(s):
    return re.sub(r"([^0-9A-Za-z_\./-])", r"\\\1", s)

def get_catalog_name(paths):
    """Determine the name of our catalog, based on the current time and
       on the common prefix shared by all our paths.
       """
    prefix = os.path.commonprefix(paths).replace(os.sep, '.')
    assert prefix[0] == '.'
    return time.strftime("%Y%m%d_%H%M%S") + prefix

def map_path(cd_path, fs_path):
    """Generate a mkisofs parameter mapping the given
       filesystem path to a path on the CD.
       """
    return "%s=%s" % (escape_path(cd_path),
                      escape_path(fs_path))

def map_normal_path(fs_path):
    """Map a normal file to its datestamped equivalent directory on CD"""
    assert fs_path[0] == os.sep
    return map_path(time.strftime("%Y%m%d") + fs_path, fs_path)

def md5sum(filename):
    """Compute the md5sum of a file, returning a one-line string.
       This actually runs the md5sum utility over ssh, to avoid
       copying the file over NFS first.
       """
    return os.popen("ssh navi md5sum %s" % shell_escape(shell_escape(filename)), "r").readline().strip()

def add_file_to_catalog(catalog, path):
    sum = md5sum(path)
    print sum
    catalog.write(sum + "\n")

def add_tree_to_catalog(catalog, path):
    if os.path.isfile(path):
        add_file_to_catalog(catalog, path)
    else:
        for root, dirs, files in os.walk(path):
            for name in files:
                add_file_to_catalog(catalog, os.path.join(root, name))

def main():
    record_cmd = "growisofs -Z /dev/dvd -R -J -graft-points".split()
    paths = map(os.path.abspath, sys.argv[1:])

    # Name and map the catalog
    catalog_name = get_catalog_name(paths)
    catalog_cd_name = os.path.join("catalog", catalog_name)
    catalog_fs_name = os.path.join("/navi/backups/catalog", catalog_name)
    record_cmd.append(map_path(catalog_cd_name, catalog_fs_name))

    # Fill the catalog
    catalog = open(catalog_fs_name, "w")
    for path in paths:
        add_tree_to_catalog(catalog, path)
    catalog.close()

    # Map other paths
    for path in paths:
        record_cmd.append(map_normal_path(path))

    os.system(" ".join(record_cmd))

if __name__ == "__main__":
    main()

### The End ###
