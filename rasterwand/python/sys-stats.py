#!/usr/bin/env python
import rasterwand, time
import commands
import re

# Lots of initial stuff
dev = rasterwand.Device()

dev.params.display_width = 0xFFFF * 0.75
dev.params.duty_cycle = 0xFFFF * 0.3

dev.vScrollText( "Welcome!", width=80, pause=0, speed=1)
totaliter = -1
net = 1
temperature = ""

old = [ 0.0, 0.0, 0.0, 0.0 ]

graph = [ 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF ]
title_cpu = [ 0x7e, 0x81, 0x81, 0x81, 0x00, 0xff, 0x11, 0x11, 0x0e, 0x00, 0x7f, 0x80, 0x80, 0x7f ]
title_load = [ 0xff, 0x80, 0x80, 0x00, 0x60, 0x90, 0x60, 0x00, 0x60, 0x90, 0xe0, 0x00, 0x60, 0x90, 0xff, 0x00, 0x00 ]
hist = [ 0x00 ] * 30

# Here we go
while 1:

	# Keep the weather module from updating every minute
	totaliter = totaliter + 1
	if totaliter == 30:
		totaliter = 0

    # Show the time
	ct = 0
	while ct < 15:
		dev.showText(time.strftime("%l:%M%P  %b %d"))
		time.sleep(0.5)
		dev.showText(time.strftime("%l %M%P  %b %d"))
		time.sleep(0.5)
		ct = ct + 1;
        
    # Get uptime
	data = commands.getoutput("cat /proc/uptime")
	uptime = float( data.split()[0] )
	days = int( uptime / 60 / 60 / 24 )
	uptime = uptime - days * 60 * 60 * 24
	hours = int( uptime / 60 / 60 )
	uptime = uptime - hours * 60 * 60
	minutes = int( uptime / 60 )
	uptime = uptime - minutes * 60
	seconds = int( uptime )
	data = "".join(["Up:  %d days, %d:%02d:%02d" % (days, hours, minutes, seconds)])
	dev.vScrollText( data, width=80, pause=5, speed=1 )

	# Get Load averages
	data = commands.getoutput("cat /proc/loadavg")
	load1 = float( data.split()[0] )
	load2 = float( data.split()[1] )
	load3 = float( data.split()[2] )
	data = "".join(["Load: %0.2f, %0.2f, %0.2f" % (load1, load2, load3)])
	rasterwand.TextScroller(dev, data, width=80).scroll(start=(1,0), end=(-1, 0), speed=2.0)
	
	# Show the CPU load graph for 10 seconds
	myCount = 0
	while (myCount < 100):
		myCount = myCount + 1
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
	
	dev.writeRaw( "\x00" )
	
    # Get CPU temperature
	data = commands.getoutput("cat /proc/acpi/thermal_zone/THRM/temperature | cut -d ' ' -f 14")
	dev.vScrollText( "".join(["CPU Temp: %s\xB0  C" % data]), width=80, pause=5, speed=1)

    # Get local IP addresses
	eth0 = commands.getoutput("/sbin/ifconfig eth0 2>/dev/null | grep 'inet addr:' | cut -d ':' -f 2 | cut -d ' ' -f 1")
	eth1 = commands.getoutput("/sbin/ifconfig eth1 2>/dev/null | grep 'inet addr:' | cut -d ':' -f 2 | cut -d ' ' -f 1")
    
	if eth0 == "" and eth1 == "":
		dev.vScrollText( "Network is down", width=80, pause=5, speed=1 )
		net = 0
	else:
		net = 1
		if eth0 != "":
			dev.vScrollText( "".join(["eth0: %s" % (eth0)]), width=80, pause=5, speed=1)
        
		if eth1 != "":
			dev.vScrollText( "".join(["eth1: %s" % (eth1)]), width=80, pause=5, speed=1)
    
    # Show the current weather
	if net == 1:
		if totaliter == 0:
			weather = commands.getoutput( "wget --quiet -O - " \
                               "http://weather.yahoo.com/search/" \
                               "weather?p=boulder,colorado%20us" )

		RE_TEMPERATURE = re.compile("\n(?P<temp>\d+)")
		temperature = float(RE_TEMPERATURE.search(weather).group("temp"))
		dev.vScrollText( "".join(["Weather:   %d\xB0  F" % temperature]), width=80, pause=5, speed=1)
	elif temperature != "":
		dev.vScrollText( "".join(["Weather:   %d\xB0  F" % temperature]), width=80, pause=5, speed=1)
		totaliter = totaliter - 1
	else:
		dev.vScrollText( "No weather data", width=80, pause=5, speed=1 )
		totaliter = totaliter - 1

    # Display a nice fortune
	data = commands.getoutput("fortune -s")
	rasterwand.TextScroller(dev, data, width=80).scroll(start=(1,0), end=(-1,0), speed=2)
