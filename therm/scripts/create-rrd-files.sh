heartbeat=600
step=60
nSamples=2880
rra="-s $step RRA:AVERAGE:0.5:1:$nSamples \
              RRA:AVERAGE:0.5:7:$nSamples \
              RRA:AVERAGE:0.5:31:$nSamples \
              RRA:AVERAGE:0.5:365:$nSamples \
              RRA:MIN:0.5:1:$nSamples \
              RRA:MIN:0.5:7:$nSamples \
              RRA:MIN:0.5:31:$nSamples \
              RRA:MIN:0.5:365:$nSamples \
              RRA:MAX:0.5:1:$nSamples \
              RRA:MAX:0.5:7:$nSamples \
              RRA:MAX:0.5:31:$nSamples \
              RRA:MAX:0.5:365:$nSamples"

for sensor in local0 rf0 rf1 rf2 rf3 rf4 rf5 rf6 \
              rf7 rf8 rf9 rf10 rf11 rf12 rf13 rf14 rf15; do

    rrdtool create $sensor-temperature.rrd DS:temperature:GAUGE:$heartbeat:U:U $rra
    rrdtool create $sensor-voltage.rrd DS:voltage:GAUGE:$heartbeat:0:10 $rra
    rrdtool create $sensor-signal.rrd DS:signal:GAUGE:$heartbeat:0:1 $rra
    rrdtool create $sensor-sequence.rrd DS:sequence:COUNTER:$heartbeat:U:U $rra

done
