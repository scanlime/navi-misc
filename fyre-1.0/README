==================
 Fyre
==================

Introduction
------------

Fyre is a tool for producing computational artwork based on histograms of
iterated chaotic functions. At the moment, it implements the Peter de Jong map
in a fixed-function pipeline with an interactive GTK+ frontend and a command
line interface for easy and efficient rendering of high-resolution, high
quality images.

This program was previously known as 'de Jong Explorer', but has been renamed
to make way for supporting other chaotic functions.

All the images you can create with this program are based on the simple Peter
de Jong map equations:

   x' = sin(a * y) - cos(b * x)
   y' = sin(c * x) - cos(d * y)

For most values of a,b,c and d the point (x,y) moves chaotically. The resulting
image is a map of the probability that the point lies within the area
represented by each pixel. As you let Fyre render longer it collects more
samples and this probability map and the image becomes more accurate.

The resulting probability map is treated as a High Dynamic Range image.  This
software includes some image manipulation features that let you apply linear
interpolation and gamma correction at the full internal precision, producing a
much higher quality image than if you tried to create the same effects using
standard image processing tools.  Additionally, Gaussian blurs can be applied
to the image using a stochastic process that produces much more natural-looking
images than most programs, again without losing any of the image's original
precision.

Contact info and the latest version of Fyre is available from:
  http://fyre.navi.cx


Quality Metric
--------------

Fyre 1.0.0 introduces a new quality metric for determining stopping conditions
on command-line and animation rendering. It is much more consistent across
images than using the maximum density. A quality of 1.0 is generally "pretty
good", and 2.0 produces a very high quality image. As usual, while this metric
is pretty reliable, it's impossible to automatically judge aesthetics, so you
may need to fiddle with it if you're dissatisfied with the results.


Loading and Saving
------------------

Unless given a .exr extension, all images saved using the GTK+ frontend or the
-o command line option are standard PNG files containing an extra 'tEXt' chunk
with all the parameters used to create the image. This means the image can be
loaded back in using the -i command line option or the "Parameters -> Load from
image" menu item.

One convenient use of this is to save small low-quality images using the GUI,
then use the -i, -q, -s, and -o options to render very high quality and/or
large images noninteractively.

When using the -o (--output) option, no GUI is presented. Instead, Fyre just
outputs status lines every so often with the current rendering process.  These
lines include:

  - percent completion
  - current number of iterations
  - speed in iterations per second
  - current and target peak density values
  - elapsed and estimated remaining time


Animation
---------

Fyre includes an animation subsystem. From the main explorer window, use View >
Animation window to open the animation interface. Animations are represented as
a list of keyframes, with a transition after each. Keyframes can be manipulated
with the buttons at the top of the window. Transitions consist of a duration,
in seconds, and an editable curve. This curve's presets can be used to
transition linearly or ease in and out of keyframes, but you can also edit the
curve for more complex effects.

Each keyframe in the list also shows you the bifurcation diagram between that
keyframe's parameters and the next. Intense black dots or lines in the
bifurcation diagrams are fixed points or limit cycles, which usually don't look
good in animation.

You can preview the animation by dragging the two horizontal seek bars, or
using the 'play' button. The keyframe list can be loaded from and saved to a
custom binary format. Once you have an animation you like, you can get a higher
quality version using File -> Render. The result will be an uncompressed .AVI
file. You can use tools like mencoder or transcode to compress this file with
your favorite codec.

Animations can also be rendered from the command line using the -n and -o
options together.


Clustering
----------

As of 0.7, Fyre supports cluster rendering. On each worker node, start up fyre
with the -r command-line switch. On the head node, select your cluster nodes
using either the GUI or -c on the command line. If you're dealing with a large
cluster, the cluster-utils.sh file will probably prove useful.  Clusters on the
local network can be autodetected using UDP broadcast packets.  See the -C
command line option, or the autodetection check box in the GUI.  Note that
cluster mode cannot be used for animations at this time.


Dependencies
------------

Hard Dependencies:
- GTK+ 2.0
- libglade

Soft Dependencies:
- GTK+ 2.4
- OpenEXR, necessary for saving .exr files
- GNet, for clustering and remote control mode


Compiling
---------

Standard autotools procedure. configure, make, make install, you know the
drill.

Fyre supports Windows, using the same autotools build system. You'll need
Cygwin to compile this under Windows, but we find a cross-compiler under Linux
easier. The tools in the 'build' directory are used to create the final binary
packages.


Authors
--------

David Trowbridge <trowbrds@cs.colorado.edu>
Micah Dowty <micah@navi.cx>

