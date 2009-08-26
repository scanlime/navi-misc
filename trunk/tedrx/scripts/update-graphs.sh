#!/bin/sh

THERM_DIR=~/src/navi-misc/therm
TEDRX_DIR=~/src/navi-misc/tedrx/scripts
GRAPHS_DIR=~/graphs
WWW_DIR=/var/www

POWER_INTERVALS="1h 1d 1w 1m"
TEMP_INTERVALS="1d 1w 1m"
URL_BASE="http://192.168.1.77"
RSSFILE="index.rss"

####### Restart Daemons

cd $THERM_DIR

(ps ax | grep therm-daemon | grep -v -q grep) || \
   nohup ./therm-daemon $GRAPHS_DIR \
   > /dev/null \
   2> $GRAPHS_DIR/therm-daemon.log &

cd $TEDRX_DIR

(ps ax | grep tedrx-daemon | grep -v -q grep) || \
   nohup ./tedrx-daemon.py $GRAPHS_DIR \
   > /dev/null \
   2> $GRAPHS_DIR/tedrx-daemon.log &

####### Update RSS file

date=`date`
hash=`date +%s`
cd $GRAPHS_DIR

( cat <<EOF
<?xml version="1.0" encoding="utf-8"?>
<rss version="2.0" xmlns:media="http://search.yahoo.com/mrss/">
<channel>

<title>Graphs</title>
<ttl>2</ttl>
EOF

### Temperature graphs

for interval in $TEMP_INTERVALS; do
  url="$URL_BASE/temp-$interval.jpeg?t=$hash"

cat <<EOF
  <item>
    <title>Temperature data ($interval)</title>
    <link>$url</link>
    <description>$url</description>
    <pubDate>$date</pubDate>
    <guid isPermaLink="false">therm-$interval-$hash</guid>
    <media:content url="$url" type="image/jpeg" />
  </item>
EOF
done

### Power usage graphs

for interval in $POWER_INTERVALS; do
  url="$URL_BASE/kw-$interval.jpeg?t=$hash"

cat <<EOF
  <item>
    <title>Power Usage ($interval)</title>
    <link>$url</link>
    <description>$url</description>
    <pubDate>$date</pubDate>
    <guid isPermaLink="false">kw-$interval-$hash</guid>
    <media:content url="$url" type="image/jpeg" />
  </item>
EOF
done

echo "</channel></rss>" ) > $WWW_DIR/$RSSFILE.temp

# Atomically update the RSS file
mv $WWW_DIR/$RSSFILE.temp $WWW_DIR/$RSSFILE

####### Temperature graphs

for interval in $TEMP_INTERVALS; do

imgName=temp-$interval
graphTitle="Temperature Data ($interval) - `date`"

nice rrdtool graph $WWW_DIR/$imgName.png -a PNG -w 900 -h 350 \
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

nice convert $WWW_DIR/$imgName.png $WWW_DIR/$imgName.tmp.jpeg
mv $WWW_DIR/$imgName.tmp.jpeg $WWW_DIR/$imgName.jpeg

done

####### Power graphs

for interval in $POWER_INTERVALS; do

imgName=kw-$interval
graphTitle="Power Usage ($interval) - `date`"

nice rrdtool graph $WWW_DIR/$imgName.png -a PNG -w 900 -h 455 \
	--font DEFAULT:12:Vera.ttf \
	-t "$graphTitle" \
	--end now --start end-$interval \
	-v "Kilowatts" \
	DEF:kw=tedrx-hc245-kw.rrd:value:AVERAGE VDEF:vkw=kw,AVERAGE \
        DEF:kwmin=tedrx-hc245-kw.rrd:value:MIN \
        DEF:kwmax=tedrx-hc245-kw.rrd:value:MAX \
        AREA:kwmax#cc8888 \
        AREA:kwmin#ffffff \
	LINE:kw#000000:"Current power usage" GPRINT:vkw:"%0.3lf kW\n" \

nice convert $WWW_DIR/$imgName.png $WWW_DIR/$imgName.tmp.jpeg
mv $WWW_DIR/$imgName.tmp.jpeg $WWW_DIR/$imgName.jpeg

done

####### Voltage graphs (Not in RSS feed)

# for interval in $POWER_INTERVALS; do

# imgName=volts-$interval
# graphTitle="Line Voltage ($interval) - `date`"

# nice rrdtool graph $WWW_DIR/$imgName.png -a PNG -w 900 -h 455 \
# 	-A --font DEFAULT:12:Vera.ttf \
# 	-t "$graphTitle" \
# 	--end now --start end-$interval \
# 	-v "Volts" \
# 	DEF:v=tedrx-hc245-volts.rrd:value:AVERAGE VDEF:vv=v,AVERAGE \
#         DEF:vmin=tedrx-hc245-volts.rrd:value:MIN \
#         DEF:vmax=tedrx-hc245-volts.rrd:value:MAX \
#         AREA:vmax#8888cc \
#         AREA:vmin#ffffff \
# 	LINE:v#000000:"Current line voltage" GPRINT:vv:"%0.3lf V\n" \
   

# nice convert $WWW_DIR/$imgName.png $WWW_DIR/$imgName.tmp.jpeg
# mv $WWW_DIR/$imgName.tmp.jpeg $WWW_DIR/$imgName.jpeg

# done
