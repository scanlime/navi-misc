#!/usr/bin/env python
#
# A simple Nouvelle example using BaseHTTPServer and Nouvelle.Table
# This runs a web server at http://localhost:8080, and presents a table
# that you can resort by clicking the headings.
#

# So we can find Nouvelle even if it isn't installed...
import sys, os; sys.path[0] = os.path.join(sys.path[0], '..', '..')

import Nouvelle
from Nouvelle import tag, place, BaseHTTP


class MyTable(Nouvelle.ResortableTable):
    tableTag = tag('table', border='1')


class Hello(BaseHTTP.Page):
    document = tag('html')[
                   tag('body') [
                       tag('h3')[ "Hello World!" ],
                       place("dataTable"),
                   ],
               ]

    data = [
        ['Llama',   3, 6, 2],
        ['Newt',    2, 7, 1],
        ['Banana',  5, 6, 0],
        ['Oatmeal', 3, 8, 2],
        ['Pine',    2, 9, 4],
        ]

    def render_dataTable(self, context):
        return MyTable(self.data, [
            Nouvelle.IndexedColumn('name', 0),
            Nouvelle.IndexedColumn('absorbency', 1),
            Nouvelle.IndexedColumn('conductivity', 2),
            Nouvelle.IndexedColumn('luminance', 3),
            ])

if __name__ == "__main__":
    BaseHTTP.main(Hello())
