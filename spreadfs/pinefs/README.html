<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
<title>
Pinefs
</title>
</head>
<body>
<H3>Introduction</H3>
<p>Pinefs is a user-space NFS server, written entirely in Python.  It
provides three sample filesystems: pyfs, which provides a view of the entire
Python namespace as a file system, allowing you to change variables in a
running Python program by writing to the corresponding file, or to use
Unix tools like find; memfs, a fairly trivial in-memory
filesystem; and tarfs, which populates memfs from a tar file specified
by the mount command (requires tarfile, included in Python 2.3, or
<a href="http://www.gustaebel.de/lars/tarfile/">available separately</a>).
The package also includes rpcgen.py, a Python
compiler from ONC RPC IDL to Python source.  Pinefs requires Python 2.2
or later, and has been developed on Linux and lightly tested on Win 98.
You can download it <a href="Pinefs-1.1.tar.gz">here</a>.</p>
<H3>Running the Pinefs pyfs Server</H3>
<p>srv.py takes a single option, '-f', which defaults to 'py'
for the Python filesystem, but can be 'mem' for the memory
filesystem or 'tar' for the tar filesystem.  When the Python filesystem is
specified, the server
runs a Python interactive loop.</p>
<p>The mount server runs on port 5555, and the NFS server on 2049.
(Both register with the portmapper.)  On my Linux box, I use the following
command to mount the server for pyfs:
<pre>
mount -t nfs -o noac 127.0.0.1:/ /mnt/nfs
</pre>
<p>
Once the Python filesystem is mounted, you can try:
<pre>
echo 1 > &lt;mount point&gt;/rpchelp/trace_rpc
</pre>
<p>to set the global trace_rpc in module rpchelp to 1, which causes
information about rpc calls to be printed.  You should see trace
information for the NFSPROC_WRITE call.  After you've
looked around the filesystem for a bit, you can turn off tracing by either:
<pre>
echo 0 > &lt;mount point&gt/rpchelp/trace_rpc
</pre>
<p>
or typing into the Python interaction loop:
<pre>
import rpchelp
rpchelp.trace_rpc = 0
</pre>
<p>See <a href="pyfs.html">here</a> for more information on the mapping
implemented by pyfs.  The mapping isn't perfect, but works well enough
to use emacs, or to untar, configure, and compile, e.g., my 
<a href="http://www.pobox.com/~asl2/music/dissoc_studio/">Dissociated Studio</a> package.  (Note: if you want a production-quality monitoring/debugging
system, you should probably use something built for that purpose (like
manhole from <a href="http://www.twistedmatrix.com/">Twisted</a>), since you
could get more functionality, Pinefs doesn't address synchronization issues,
and pyfs filehandles aren't persistent across restarts).)
</p>
<H3>Running the Pinefs tarfs Server</H3>
<p>This works much as above, except that instead of <kbd>127.0.0.1:/</kbd>,
substitute for <kbd>/</kbd> the pathname to a tar file.  Now you can,
change directory to the mount point and browse, or, if it's a software
package, e.g., <kbd>./configure; make</kbd>.

<H3>Running rpcgen.py</H3>
<p>rpcgen.py takes an IDL filename as a parameter, and writes Python
code implementing that IDL on stdout.  The generated code imports
rpchelp and rpc from the Pinefs distribution, which thus must be
present at runtime.  rpcgen.py requires Dave Beazley's <a
href="http://systems.cs.uchicago.edu/ply/">PLY package</a>, licensed
under the LGPL.  Pinefs includes a precompiled rfc1094.py (generated
from rfc1094.idl), so you don't need PLY in order to run Pinefs.
See <a href="rpcgen.html">here</a> for more information on rpcgen.
<H3>Notes</H3>
<p>There are several other related programs, of which I was unaware,
partly because they weren't in <a href="http://www.python.org/pypi">
PyPI</a> orthe <a href="http://www.vex.net/parnassus">
Vaults of Parnassus</a>, partly because they weren't in the first
ten pages or so of google results (web and comp.lang.python.* search) when
I initially looked, and partly because I started from the Python 2.1
rather than 2.2 Demo/rpc directory.  (Consider this a plea for people
to use Vaults or post to comp.lang.python.announce.) I still think Pinefs
may be of interest, because it exposes the Python namespace via NFS, and
the included rpcgen handles nested structs and unions.
<p>
Here are the others:
<ul>
<li><a href="http://www.cendio.se/~peter/pynfs/">Pynfs</a>, a NFSv4
client, server, and test suite, and rpcgen</li>
<li><a href="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/zodbex/zodbex/nfs/">Zodbex NFS</a>, a NFSv[23] implementation with an in-memory filesystem</li>
<li>Wim Lewis's <a href="http://www.omnigroup.com/~wiml/soft/stale-index.html#python">rpcgen.py</a></li>
</ul>
<p>Pinefs is licensed under a MIT/X style license, except for rpc.py, which,
since my version is based on the Python 2.1 distribution, has the Python
license.
<p>The name "Pinefs" is both a pun on PyNFS, the obvious name for
such a program, and allows me to make the quasi-obligatory 
<a href="http://www.mtholyoke.edu/~ebarnes/python/dead-parrot.htm">allusion
</a>:
"It's pining for the files!"
<p>If you have any questions, you can reach me at 
<a href="mailto:asl2@pobox.com">Aaron Lav</a>.
<p><a href="http://www.pobox.com/~asl2/">Back</a> to Aaron's home page.
</body>
</html>
