#!/usr/bin/env python
import rasterwand
import commands
import time

dev = rasterwand.Device()

dev.params.display_width = 0xFFFF * 0.7
#dev.params.duty_cycle = 0xFFFF * 0.2

old = [ 0.0, 0.0, 0.0, 0.0 ]

graph = [ 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF ]
title_cpu = [ 0x7e, 0x81, 0x81, 0x00, 0xff, 0x11, 0x0e, 0x00, 0x7f, 0x80, 0x80, 0x7f, 0x00, 0x00 ]
title_load = [ 0xff, 0x80, 0x80, 0x00, 0x60, 0x90, 0x60, 0x00, 0x60, 0x90, 0xe0, 0x00, 0x60, 0x90, 0xff, 0x00, 0x00 ]
hist = [ 0x00 ] * 30

while 1:
	cpu = commands.getoutput("cat /proc/stat").splitlines()[0].split()
	
	uN = float(cpu[1])
	nN = float(cpu[2])
	sN = float(cpu[3])
	iN = float(cpu[4])
	
	u = uN - old[0]
	n = nN - old[1]
	s = sN - old[2]
	i = iN - old[3]
	
	old = [ uN, nN, sN, iN ]
		
	t = u + n + s + i
	m = float(100) / t
	
	load = (u + n + s) * m
	user = (u + n) * m
	sys = (s) * m
	data = ""
	
	num = int( ( (load) * 8.0 ) / 100.0 )

	for i in xrange(0, 14):
		data = data + "%c" % title_cpu[i]

	for i in xrange(0, 30):
		data = data + "%c" % hist[i]
	
	data = data + "%c" % graph[num]
	dev.writeRaw( data )

	for i in xrange(0, 29):
		hist[i] = hist[i+1]
	
	hist[29] = graph[num]
		
	time.sleep(0.1)
