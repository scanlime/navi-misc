#!/usr/bin/env python

import gtk, gtk.glade
from Wasabi.Icon import Icon

icons = {}
# icons['Navi']              = Icon('icon_navi.png')
icons['Nintendo 64']       = Icon('icon_n64.png')
icons['Playstation 2']     = Icon('icon_ps2.png')
icons['Sega Dreamcast']    = Icon('icon_dreamcast.png')
icons['Nintendo']          = Icon('icon_nes.png')
icons['Nintendo Gamecube'] = Icon('icon_gamecube.png')
icons['Unknown Input']     = Icon('icon_rca_cables.png')
icons['Sega Genesis']      = Icon('icon_genesis.png')
icons['Atari']             = Icon('icon_atari.png')
icons['Settings']          = Icon('icon_hammer.png')

class MainWindow:
    """This is the main menu window"""
    def __init__(self):
        self.glade = gtk.glade.XML('data/wasabi.glade')

        image = self.glade.get_widget('top image')
        image.set_from_pixbuf(icons['Nintendo Gamecube'].i120)
        label = self.glade.get_widget('top label')
        label.set_markup('<span size="x-large" weight="bold">Nintendo Gamecube</span>')

        image = self.glade.get_widget('middle image')
        image.set_from_pixbuf(icons['Playstation 2'].i240)
        label = self.glade.get_widget('middle label')
        label.set_markup('<span size="xx-large" weight="bold">Playstation 2</span>')

        image = self.glade.get_widget('bottom image')
        image.set_from_pixbuf(icons['Sega Dreamcast'].i120)
        label = self.glade.get_widget('bottom label')
        label.set_markup('<span size="x-large" weight="bold">Sega Dreamcast</span>')

win = MainWindow()
gtk.main()
