#!/usr/bin/env python
import ledboard, time
led = ledboard.Server()

load = [[0]*8]*5

def gridFunc(led):
    return load[led['x']][led['y']]

grid = led.select({'group': 'grid'})

oldUserSys = 0L
oldIdle = 0L

while True:
    led.set(grid, gridFunc)
    led.blit()
    time.sleep(0.25)

    # Fetch the system load
    procstat = open("/proc/stat","r")
    for line in procstat.readlines():
        fields = line.split()
        if fields[0] == 'cpu':
            newUserSys = long(fields[1]) + long(fields[3])
            newIdle = long(fields[4])
            if newIdle == oldIdle:
                newLoad = 1.0
            else:
                newLoad = float(newUserSys - oldUserSys) / float(newIdle - oldIdle)
            oldUserSys = newUserSys
            oldIdle = newIdle
            break
        
    led.vfd.writeScreen("%10.02f%% CPU\n" % (newLoad*100))

    # Draw a load bar, with sub-LED accuracy
    newLoadBar = []
    for i in range(8):
        lowPos = i/8.0
        highPos = (i+1)/8.0
        if newLoad < lowPos:
            b = 0
        elif newLoad > highPos:
            b = 15
        else:
            b = (newLoad - lowPos) / (highPos - lowPos) * 15.0
        newLoadBar = [int(b)] + newLoadBar

    # Shift in the new load bar
    load = load[1:] + [newLoadBar]
