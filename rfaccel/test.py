port = open("/dev/usb/tts/0", "rb")

def sum(bytes):
    s = 0
    for byte in bytes:
        s = (s + ord(byte)) & 0xFF
    return s

def hexdump(bytes):
    return " ".join(["%02X" % ord(byte) for byte in bytes])

buffer = ""
while 1:
    buffer = (buffer + port.read(1))[-11:]
    if buffer[0] != "U":
        continue
    if sum(buffer) != 0:
        continue

    print hexdump(buffer)


