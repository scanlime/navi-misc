#!/usr/bin/env python
#
# A simple Nouvelle example using BaseHTTPServer and Nouvelle.Table
# This runs a web server at http://localhost:8080, and presents tables
# that you can resort by clicking the headings.
#

import Nouvelle
from Nouvelle import tag, place, BaseHTTP

class MyTable(Nouvelle.ResortableTable):
    tableTag = tag('table', border='1')

class Hello(BaseHTTP.Page):
    document = tag('html')[
                   tag('body') [
                       tag('h3')[ "Hello World!" ],
                       tag('p') [ place("dataTable") ],
                       tag('hr'),
                       tag('p') [ place("foodTable") ],
                   ],
               ]

    data = [
        ['Llama',   3, 6, 2, 'Oatmeal'],
        ['Newt',    2, 7, 1, 'Pine'],
        ['Banana',  5, 6, 0, 'Banana'],
        ['Oatmeal', 3, 8, 2, 'Newt'],
        ['Pine',    2, 9, 4, 'Llama'],
        ]

    def render_dataTable(self, context):
        return MyTable(self.data, [
            Nouvelle.IndexedColumn('name', 0),
            Nouvelle.IndexedColumn('absorbency', 1),
            Nouvelle.IndexedColumn('conductivity', 2),
            Nouvelle.IndexedColumn('luminance', 3),
            ], id='widgets')

    def render_foodTable(self, context):
        return MyTable(self.data, [
            Nouvelle.IndexedColumn('name', 0),
            Nouvelle.IndexedColumn('eats', 4),
            ], id='food')

if __name__ == "__main__":
    BaseHTTP.main(Hello())
