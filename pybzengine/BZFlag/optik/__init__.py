"""optik

A powerful, extensible, and easy-to-use command-line parser for Python.

By Greg Ward <gward@python.net>

See http://optik.sourceforge.net/
"""

# Copyright (c) 2001-2003 Gregory P. Ward.  All rights reserved.
# See the README.txt distributed with Optik for licensing terms.

__revision__ = "$Id: __init__.py,v 1.2 2003/07/06 03:57:28 micahjd Exp $"

__version__ = "1.4.1"


# Re-import these for convenience
from BZFlag.optik.option import Option
from BZFlag.optik.option_parser import *
from BZFlag.optik.help import *
from BZFlag.optik.errors import *


# Some day, there might be many Option classes.  As of Optik 1.3, the
# preferred way to instantiate Options is indirectly, via make_option(),
# which will become a factory function when there are many Option
# classes.
make_option = Option
