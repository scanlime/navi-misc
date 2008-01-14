rrdtool graph today.png -a PNG -w 800 -h 400 \
	-t "Today's temperature data" \
	-v "Degrees Fahrenheit" \
	DEF:rf3=rf3-temperature.rrd:temperature:AVERAGE \
	DEF:rf8=rf8-temperature.rrd:temperature:AVERAGE \
	DEF:rf9=rf9-temperature.rrd:temperature:AVERAGE \
	CDEF:rf3f=9,5,/,rf3,*,32,+ \
	CDEF:rf8f=9,5,/,rf8,*,32,+ \
	CDEF:rf9f=9,5,/,rf9,*,32,+ \
	VDEF:vrf3f=rf3f,AVERAGE \
	VDEF:vrf8f=rf8f,AVERAGE \
	VDEF:vrf9f=rf9f,AVERAGE \
	HRULE:32#808080 \
	LINE:rf3f#000080:"Refrigerator" GPRINT:vrf3f:"%0.1lf F\n"\
	LINE:rf8f#800000:"Upstairs Hall" GPRINT:vrf8f:"%0.1lf F\n"\
	LINE:rf9f#008000:"Downstairs Thermostat" GPRINT:vrf9f:"%0.1lf F\n"\

