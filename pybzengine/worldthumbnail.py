#!/usr/bin/env python
#
# Quick little utility for generating thumbnails of single BZFlag worlds, or generating
# web pages for indexing a directory full of world files.
#
from BZFlag import World
from BZFlag.UI import OverheadView
from BZFlag import optik, Util
import sys, re, os


# Lots of fun command line options!
parser = optik.OptionParser(usage = "usage: %prog [options] worlds")
parser.add_option("-o", "--output", metavar="FILE",
                  help="Sets the file or directory name to output thumbnails to.")
parser.add_option("-s", "--size", metavar="WIDTHxHEIGHT",
                  help="Sets the thumbnail size. Default is 160x160.", default="160x160")
parser.add_option("-i", "--index", action="store_true",
                  help="Generates an index of world files to the directory provided with -o.")
parser.add_option("-f", "--format", metavar="EXTENSION",
                  help="Sets the format, specified in the form of a file extension name, " +
                  "of the images to generate when an explicit filename is not given. Default is 'png'.", default='png')
parser.add_option("-a", "--oversample", metavar="AMOUNT",
                  help="Sets the amount of oversampling. Higher numbers will produce smoother images " +
                  "with thinner borders, but will require much more memory and CPU time. Default is 3.", default=3)
parser.add_option("-t", "--template", metavar="FILE",
                  help="Sets the template file to use for producing index output. Default is " +
                  "'worldindex.html' in the data directory.", default=Util.dataFile('worldindex.html'))
parser.add_option("-n", "--index-name", metavar="FILE", dest="indexName",
                  help="Sets the name of the index file to produce in the output directory. Default is 'index.html'.",
                  default="index.html")
(options, args) = parser.parse_args()

if len(args) == 0:
    parser.error("One or more world filenames must be specified on the command line.")
if len(args) > 1 and options.output and not os.path.isdir(options.output):
    parser.error("An output directory must be specified for multiple input files, not an output file.")
if options.index and options.output and not os.path.isdir(options.output):
    parser.error("An output directory must be specified for indexing mode, not an output file.")

size = map(int, options.size.split("x"))
oversample = int(options.oversample)
worlds = []


# Generate thumbnails for all the valid files we can find, storing info for the index if we want one
for inFile in args:
    # Ignore non-files
    if not os.path.isfile(inFile):
        continue

    # Ignore files with common extensions that we know aren't bzflag worlds
    for extension in ('.png', '.jpeg', '.htm', '.html'):
        if inFile.lower().endswith(extension):
            inFile = ''
    if not inFile:
        continue
    
    # Determine a good output filename
    extensionless = re.sub("\.[^\/\\\.]+$", "", inFile)
    if options.output:
        # We were given a --output
        if os.path.isdir(options.output):
            # It was a directory, join it to a filename based on the world name and format
            outFile = os.path.join(options.output, os.path.basename(extensionless) + "." + options.format)
        else:
            # It was a filename, use it as-is
            outFile = options.output
    else:
        # No --output, so replace the world's extension, if any, with the image
        # extension without modifying its path.
        outFile = extensionless + "." + options.format

    # See if we can parse it, giving only a warning if we can't
    try:
        world = World.load(inFile)
        sys.stderr.write("- Thumbnailing '%s'\n" % inFile)
    except KeyboardInterrupt:
        raise
    except:
        sys.stderr.write("* Error loading '%s': %s\n" % (inFile, sys.exc_info()[1]))
        continue

    # Actually generate the thumbnail
    img = OverheadView.worldToImage(world, size, oversample)
    img.save(outFile)

    # Save information we can plug into the template if we're indexing
    worlds.append({
        'name': os.path.basename(extensionless),
        'file': os.path.basename(inFile),
        'thumbnail': os.path.basename(outFile),
        'thumbnailWidth': size[0],
        'thumbnailHeight': size[1],
        })


# We're generating an index
if options.index:
    # Open the template and index files
    template = open(options.template)
    if options.output:
        indexFile = os.path.join(options.output, options.indexName)
    else:
        indexFile = options.indexName
    index = open(indexFile, "w")
    sys.stderr.write("- Saving index to %s\n" % indexFile)

    # A cheesy little template engine. The input file comes in header,
    # footer, and item sections. The body is repeated for each world,
    # with $variables substituted.
    while True:
        line = template.readline()
        if line.find("TemplateHeader") >= 0 or not line:
            break
    header = ""
    while True:
        line = template.readline()
        if line.find("TemplateBody") >= 0 or not line:
            break
        header += line
    body = ""
    while True:
        line = template.readline()
        if line.find("TemplateFooter") >= 0 or not line:
            break
        body += line
    footer = template.read()

    index.write(header)

    # Sort worlds by name
    def nameSort(a,b):
        return cmp(a['name'], b['name'])
    worlds.sort(nameSort)

    for world in worlds:
        worldSection = body
        for key, value in world.items():
            worldSection = re.sub("\$%s" % key, str(value), worldSection)
        index.write(worldSection)

    index.write(footer)
