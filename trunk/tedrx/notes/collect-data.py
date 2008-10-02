import serial
import ted
import time

t = ted.TED("COM6")
s = serial.Serial("COM3", 9600, timeout=5)
outfile = open("output.csv", "w")
outfile.write("kw,volts,packet\n")

while True:
    myPacket = s.readline()
    time.sleep(0.5)
    tedPackets = t.poll()
    if len(tedPackets) != 1:
        continue

    try:
        bytes = [int(x, 16) for x in myPacket.split(" ", 1)[1].split()]
    except:
        continue
    print bytes

    outfile.write("%s,%s,%s\n" % (tedPackets[0].kw, tedPackets[0].volts,
                                  ",".join(map(str, bytes))))
    outfile.flush()
    
    