#
# Javascript/CSS minification and merge tool, built on jsmin.
# Copyright (c) 2007 Micah Dowty <micah@navi.cx>
#

import os, re
from jsmin import jsmin
from cStringIO import StringIO

def cssmin(source):
    """Cheesy CSS minifier. Removes comments and extra whitespace,
       but doesn't really understand strings or multiline comments
       correctly.
       """
    # Remove single-line comments
    source = re.sub(r"//[^\n]*", "", source)

    # Collapse lines first, to make the following regexes simpler.
    source = source.replace("\n", "")
    
    # Remove multiline comments
    source = re.sub(r"/\*(.*?)\*/", "", source)

    # Collapse duplicate whitespace
    source = re.sub(r"\s+", " ", source)

    # Remove whitespace surrounding certain punctuation
    source = re.sub(r" ?([;{}:]) ?", r"\1", source)
    return source

def preprocess(source):
    """Very simple macro preprocessor. Supported commands:

       //!DEF MACRO value...

       """
    # Collect symbol definitons
    syms = {}
    for line in source.split("\n"):
        m = re.match(r"^\s*//!DEF\s+(\S+)\s+(.*)\s*$", line)
        if m:
            syms[m.group(1)] = m.group(2)

    # Sort by length- apply longer symbol substitutions first,
    # to avoid problems with symbol names which are strict
    # substrings of other symbol names.
    sorted_syms = syms.keys()
    sorted_syms.sort(lambda a,b: cmp(len(b), len(a)))

    for name in sorted_syms:
        source = source.replace(name, syms[name])
    return source

def csspp(source):
    """Minified CSS, with macro preprocessing"""
    return cssmin(preprocess(source))

def merge(destPath, sourcePaths, minifier):
    """Build the specified destination file from any number of source
       files, included in the order specified.  Each file will be
       minified, and prefixed with a comment which includes original
       copyright messages and the original file name.
       """
    dest = open(destPath, "wb")

    for sourcePath in sourcePaths:
        if not sourcePath:
            continue
        original = open(sourcePath).read()

        dest.write("/* %s */\n" % os.path.basename(sourcePath))

        # Search the original un-minified source for licenses
        # and copyrights within the first comment block.
        for line in original.split("\n"):
            line = line.strip()
            if not line:
                break

            line = re.sub("^\s*\/*\**\s*", "", line)
            line = re.sub("\s*\**\/*\s*$", "", line)
            if re.search("(copyright|license)", line, re.IGNORECASE):
                dest.write("/* %s */\n" % line)

        dest.write("%s\n\n" % minifier(original).strip())
