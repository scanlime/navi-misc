#
# This is a mod_python frontend for accessing collected therm data.
# It includes an HTML interface for viewing current data and graphs,
# and an XML/RPC interface for remote access via other programs.
#
# Requires Nouvelle and of course mod_python.
#
# -- Micah Dowty <micah@navi.cx>
#

import sys, os
_thisPath = os.path.realpath(os.path.split(__file__)[0])
_modulesPath = os.path.join(_thisPath, "..", "modules")
sys.path.insert(0, _modulesPath)

import therm_db
import time
import Nouvelle
from Nouvelle import tag, place, xml, ModPython

# This is a very low-privilege account that can only read
# from the database, probably no reason not to have
# all the login info here.
database = therm_db.ThermDatabase(db="therm", user="therm_reader", passwd="e5ce14d3")


def prettifyName(str):
    """Replace underscores with spaces, and give it title capitalization"""
    words = str.replace("_", " ").split()
    return " ".join([ word.capitalize() for word in words ])


def degCtoF(degC):
    """Convert degrees centigrade to fahrenheit"""
    return degC * 9.0 / 5.0 + 32


class UnitCollection:
    """An abstract group of units for some quntity"""
    # A list of (singular name, plural name, multiplier) tuples.
    # Must be sorted by multiplier in descending order.
    units = []

    # If the converted value would be less than this
    # threshold, a smaller unit will be chosen.
    threshold = 0.8

    # Smallest decimal place to represent
    precision = 0.01

    def format(self, value):
        """Pick an appropriate unit and format the given value"""
        for singular, plural, multiplier in self.units:
            converted = value / multiplier
            if converted > self.threshold:
                break

        # Round the converted value to our set precision
        converted += self.precision / 2
        converted -= converted % self.precision

        # Is it close enough to 1 to use our singular form?
        if abs(converted - 1) < self.precision:
            unit = singular
        else:
            unit = plural

        # Chop off the trailing .0 if there is one
        s = str(converted)
        if s.endswith('.0'):
            s = s[:-2]
        return s + ' ' + unit


class TimeUnits(UnitCollection):
    """Time units, standard unit is 1 second"""
    units = [
        ('year',        'years',        365 * 24 * 60 * 60),
        ('month',       'months',       30 * 24 * 60 * 60),
        ('week',        'weeks',        7 * 24 * 60 * 60),
        ('day',         'days',         24 * 60 * 60),
        ('hour',        'hours',        60 * 60),
        ('minute',      'minutes',      60),
        ('second',      'seconds',      1),
        ('millisecond', 'milliseconds', 0.001),
        ('microsecond', 'microseconds', 0.000001),
        ]

formatTime = TimeUnits().format


class IndexPage(ModPython.Page):
    css = """
        .source {
            background: #DDD;
            border: 1px solid #777;
            margin: 2em;
            padding: 0.5em;
        }
        .name {
            font: 100% sans-serif;
            font-weight: bold;
            color: #333;
        }
        .description {
            font: 80% normal;
            color: #666;
            margin: 0.25em 1em;
        }

        .temperatures {
            float: right;
            margin: 0.5;
        }
        .mainTemperature {
            font: 180% sans-serif;
        }
        .temperatureSeparator {
            font: 180% sans-serif;
            font-weight: bold;
            color: #666;
        }
        .altTemperature {
            font: 120% sans-serif;
            color: #666;
        }

        .extraInfo {
            margin: 0.75em 0em 0em 0em;
            font: 80% sans-serif;
        }
        span.batteryVoltage {
            margin: 0em 0.5em;
            font: 80% sans-serif;
            background: #DEECE7;
            border: 1px solid #8CBBAC;
        }
    """

    document = tag('html')[
                   tag('head')[
                       tag('title')[ "Navi - Therm Server" ],
                       tag('style', type='text/css')[ css ],
                   ],
                   tag('body')[
                       place('sources'),
                   ],
               ]

    def render_sources(self, context):
        results = []
        for source in database.iterSources():
            results.append(self.renderSource(source, context))
        return results

    def renderSource(self, source, context):
        latest = source.getLatestPacket()

        info = []

        if latest.get('average'):
            degC = latest['average']
            degF = degCtoF(degC)
            info.append(tag('div', _class='temperatures')[
                tag('span', _class='mainTemperature')[ "%.01f" % degF, xml("&deg;"), "F" ],
                tag('span', _class='temperatureSeparator')[ " / " ],
                tag('span', _class='altTemperature')[ "%.01f" % degC, xml("&deg;"), "C" ],
                ])

        info.extend([
            tag('div', _class='name')[ prettifyName(source.name) ],
            tag('div', _class='description')[ source.description ],
            ])

        if latest.get('voltage'):
            voltage = latest['voltage']
            minVoltage = 6.5
            maxVoltage = 9.0
            qualitative = (voltage - minVoltage) / (maxVoltage - minVoltage)
            qualitative = max(0, min(1, qualitative))

            info.append(tag('div', _class='extraInfo')[
                "Battery: ",
                tag('span', _class='batteryVoltage', style='padding: 0.1em %fem;' % (
                    qualitative * 5.0)) [
                    "%.03f V" % voltage
                ],
            ])

        if latest.get('time'):
            delta = time.time() - latest['time'].ticks()
            info.append(tag('div', _class='extraInfo')[
                "Last reading received ",
                tag('strong')[ formatTime(delta) ], " ago",
            ])

        return tag('div', _class='source')[info]

index = IndexPage()

