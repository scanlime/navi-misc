#!/usr/bin/env python

import GTKsheet, gtk

sheet = GTKsheet.GTKsheet()
sheet.readSheet('sheet.xml')
gtk.main()
