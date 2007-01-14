/* -*- Mode: C; c-basic-offset: 4 -*- 
 * Copyright (c) 2003-2007 Micah Dowty <micah@navi.cx>
 *
 * This is a Javascript version of the LibCIA.Units module.
 */

/*
 * An abstract group of units for some quntity
 */
var UnitCollection = function() {};

/*
 * A list of (singular name, plural name, multiplier) arrays.
 * Must be sorted by multiplier in descending order.
 */
UnitCollection.prototype.units = [];

/*
 * If the converted value would be less than this
 * threshold, a smaller unit will be chosen.
 */
UnitCollection.prototype.threshold = 0.8;

/*
 * Number of digits to the right of the decimal.
 */
UnitCollection.prototype.precision = 2;

/*
 * Pick an appropriate unit and format the given value
 */
UnitCollection.prototype.format = function(value)
{
    var i, singular, plural, multipler, converted;

    for (i in this.units) {
	singular = this.units[i][0];
	plural = this.units[i][1];
	multiplier = this.units[i][2];

	converted = value / multiplier;
	if (converted > this.threshold) {
	    break;
	}
    }

    /* Round this number according to our precision */
    var s = converted.toFixed(this.precision);

    /* Chop off the trailing zeroes */
    s = s.replace(/.0+$/, "");

    /* Choose singular or plural form */
    var unit = plural;
    if (s == "1") {
	unit = singular;
    }

    return s + ' ' + unit;
};

/*
 * Time units, standard unit is 1 second
 */
var TimeUnits = new UnitCollection();
TimeUnits.units =
[
 ["year",        "years",        365 * 24 * 60 * 60],
 ["month",       "months",       30 * 24 * 60 * 60],
 ["week",        "weeks",        7 * 24 * 60 * 60],
 ["day",         "days",         24 * 60 * 60],
 ["hour",        "hours",        60 * 60],
 ["minute",      "minutes",      60],
 ["second",      "seconds",      1],
 ["millisecond", "milliseconds", 0.001],
 ["microsecond", "microseconds", 0.000001],
];

/*
 * Abbreviated time units, standard unit is 1 second
 */
var TimeUnitsAbbrev = new UnitCollection();
TimeUnitsAbbrev.units =
[
 ["yr",    "yr",    365 * 24 * 60 * 60],
 ["mth",   "mth",   30 * 24 * 60 * 60],
 ["wk",    "wk",    7 * 24 * 60 * 60],
 ["d",     "d",     24 * 60 * 60],
 ["hr",    "hr",    60 * 60],
 ["min",   "min",   60],
 ["sec",   "sec",   1],
 ["ms",    "ms",    0.001],
 ["us",    "us",    0.000001],
];

/*
 * Storage units, standard is 1 byte
 */
var StorageUnits = new UnitCollection();
StorageUnits.units =
[
 ["TB",   "TB",    1024 * 1024 * 1024 * 1024],
 ["GB",   "GB",    1024 * 1024 * 1024],
 ["MB",   "MB",    1024 * 1024],
 ["kB",   "kB",    1024],
 ["byte", "bytes", 1],
];
