#!/usr/bin/env python
#
# A sandbox for testing the Nouvelle web framework used by CIA
# This runs a web server at http://localhost:8080
#
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
