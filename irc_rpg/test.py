#!/usr/bin/env python

import sheetLayout

sheet = sheetLayout.sheetLayout()
sheet.readSheet('sheet.xml')
print sheet.root.attributes['data']
