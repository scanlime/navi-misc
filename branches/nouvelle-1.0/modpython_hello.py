#
# A very simple example of using Nouvelle with mod_python and
# it's Publisher handler.
#

import Nouvelle
from Nouvelle import tag, place, ModPython

class MyTable(Nouvelle.ResortableTable):
    tableTag = tag('table', border='1')

class Hello(ModPython.Page):
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

index = Hello().publish

