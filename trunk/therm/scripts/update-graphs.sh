for scale in 1h 1d 1w 1m 1y; do

rrdtool graph temp-$scale.png -a PNG -w 800 -h 400 \
	-t "Temperature Data" \
	--end now --start end-$scale \
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

done
