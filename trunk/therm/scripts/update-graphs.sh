#!/bin/bash
#
# Generate RRD graphs for the temperature sensors, and update an RSS file
# which includes per-file timestamps in order to defeat client-side caching.
#
# Micah Dowty <micah@navi.cx>
#

####### Configuration ########

#
# Which intervals to generate graphs for? 
# These must be rrdtool-compatible time differences.
#
INTERVALS="1d 1w 1m"

#
# Variables for the generated RSS file
#
TITLE="Temperature Data"
URL_BASE="http://192.168.1.77/therm"
RSSFILE="therm.rss"

##############################

hash=`date +%s`

( cat <<EOF
<?xml version="1.0" encoding="utf-8"?>
<rss version="2.0" xmlns:media="http://search.yahoo.com/mrss/">
<channel>

<title>$TITLE</title>
<ttl>2</ttl>
EOF

for interval in $INTERVALS; do
   echo "<item><media:content url=\"$URL_BASE/temp-$interval.jpeg?t=$hash\" type=\"image/jpeg\" /></item>"
done

echo "</channel></rss>" ) > $RSSFILE

for interval in $INTERVALS; do

imgName=temp-$interval
graphTitle="Temperature Data ($interval) - `date`"

rrdtool graph $imgName.png -a PNG -w 900 -h 350 \
	--font DEFAULT:12:Vera.ttf \
	-t "$graphTitle" \
	--end now --start end-$interval \
	-v "Degrees Fahrenheit" \
	DEF:rf3=rf3-temperature.rrd:temperature:AVERAGE CDEF:rf3f=9,5,/,rf3,*,32,+ VDEF:vrf3f=rf3f,AVERAGE \
	DEF:rf4=rf4-temperature.rrd:temperature:AVERAGE CDEF:rf4f=9,5,/,rf4,*,32,+ VDEF:vrf4f=rf4f,AVERAGE \
	DEF:rf6=rf6-temperature.rrd:temperature:AVERAGE CDEF:rf6f=9,5,/,rf6,*,32,+ VDEF:vrf6f=rf6f,AVERAGE \
	DEF:rf7=rf7-temperature.rrd:temperature:AVERAGE CDEF:rf7f=9,5,/,rf7,*,32,+ VDEF:vrf7f=rf7f,AVERAGE \
	DEF:rf8=rf8-temperature.rrd:temperature:AVERAGE CDEF:rf8f=9,5,/,rf8,*,32,+ VDEF:vrf8f=rf8f,AVERAGE \
	DEF:rf9=rf9-temperature.rrd:temperature:AVERAGE CDEF:rf9f=9,5,/,rf9,*,32,+ VDEF:vrf9f=rf9f,AVERAGE \
	LINE2:rf3f#674264:"Refrigerator" GPRINT:vrf3f:"%0.1lf F\n" \
	LINE2:rf4f#404040:"Outside" GPRINT:vrf4f:"%0.1lf F\n" \
	LINE2:rf6f#db5669:"Garage" GPRINT:vrf6f:"%0.1lf F\n" \
	LINE2:rf7f#79d1d5:"Office" GPRINT:vrf7f:"%0.1lf F\n" \
	LINE2:rf8f#798634:"Upstairs Hall" GPRINT:vrf8f:"%0.1lf F\n" \
	LINE2:rf9f#f76115:"Downstairs Thermostat" GPRINT:vrf9f:"%0.1lf F\n" \

convert $imgName.png $imgName.tmp.jpeg
mv $imgName.tmp.jpeg $imgName.jpeg

done
