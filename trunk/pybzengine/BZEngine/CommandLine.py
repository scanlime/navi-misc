""" BZEngine.CommandLine

Utilities for processing command line options for different categories
of BZEngine-related utilities.
"""
#
# Python BZEngine Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from BZEngine import optik, Client, Player, Server, Protocol, UI
from BZEngine.Protocol import Common
import sys


# Override Optik's default options (for consistent grammar and capitalization)
optik.STD_HELP_OPTION.help    = "Shows this help message and exits."
optik.STD_VERSION_OPTION.help = "Shows the version number and exits."


class Parser(optik.OptionParser):
    """OptionParser subclass to assist with creating Client or Server instances
       using a standard set of command line options.

       cls should be the class to instantiate using command line options. It
       may also be a list of classes- all of them will be used to construct
       a list of command line options.
       """
    def __init__(self, *classes, **extraDefaults):
        optik.OptionParser.__init__(self)

        self.instances = []
        self.defaults = {}
        for c in classes:
            inst = c()
            self.instances.append(inst)
            self.defaults.update(inst.options)

        self.defaults.update(extraDefaults)

        def add(*names, **kw):
            """Wrapper around self.add_option that enforces our conventions about defaults
               and disables the option if its destination isn't in our list of available options
               """
            if kw['dest'] not in self.defaults.keys():
                return
            default = self.defaults.get(kw['dest'])
            kw['default'] = default
            if default is not None and default != [] and str(default):
                kw['help'] += " [%s]" % default
            self.add_option(*names, **kw)


        add("-s", "--server", dest="server", metavar="HOST",
            help="Sets the BZEngine server to connect to.")

        add("-p", "--public", dest="publicTitle", metavar="TITLE",
            help="Publicize the server under the given title.")

        add("-g", "--game", dest="gameStyle", metavar="STYLE", action="append",
            help="Enable the given game style. STYLE can be one of %s" %
            ", ".join(map(repr, Common.GameStyle.dict.keys())))

        add("-o", "--shots", dest="shots", metavar="N", 
            help="Sets the number of shots a player may fire before reloading.")

        add("-w", "--world", dest="world", metavar="NAME",
            help="Loads a BZEngine world. NAME can be a local file or one of several" +
            " other world specifiers. Try a world name of 'help' for more information.")

        add("-i", "--interface", dest="interface", metavar="HOST:PORT",
            help="Sets the host and/or the port to listen for clients on.")
        
        add("-c", "--call-sign", dest="callSign", metavar="NAME",
            help="Sets the player callsign to use when joining.")

        add("-t", "--team", dest="team", metavar="COLOR",
            help="Sets the team to join.")

        add("-e", "--email", dest="email", metavar="ADDRESS",
            help="Sets the optional email address to send when joining.")

        add("-p", "--player-type", dest="playerType", metavar="TYPE",
            help="Sets the player type to join the game as. This can be 'tank', 'observer', or 'computer'.")

        add("-u", "--ui", dest="ui", metavar="NAME",
            help="Use the given user interface to interact with or view the game. Available UIs: " +
            ", ".join(UI.list()))


    def parse(self, argv=sys.argv):
        (values, args)  = self.parse_args(argv[1:])
        values = values.__dict__
        options = {}
        options.update(self.defaults)
        options.update(values)

        for inst in self.instances:
            inst.cmdLineValues = values
            inst.cmdLineArgs = args
            inst.setOptions(**options)

        if len(self.instances) > 1:
            return self.instances
        else:
            return self.instances[0]


def client(clientClass=Client.PlayerClient, argv=sys.argv, **extraDefaults):
    """Convenience function for instantiating a ClientParser
       and immediately parsing args with it, for the case when you don't
       need to add any extra options.
       """
    return Parser(clientClass, **extraDefaults).parse(argv)

def server(serverClass=Server.StrictServer, argv=sys.argv, **extraDefaults):
    """A similar convenience function for servers"""
    return Parser(serverClass, **extraDefaults).parse(argv)

### The End ###
