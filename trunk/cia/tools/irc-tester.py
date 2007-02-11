import socket, sys, random, re, time

class IRCException(Exception):
    pass

def iter_socket_lines(socket, bufferSize=32768, separator="\n"):
    """xreadlines(), for sockets."""
    remaining = ""
    while 1:
        buffer = socket.recv(bufferSize)
        if not buffer:
            break

        lines = buffer.split(separator)
        lines[0] = remaining + lines[0]
        remaining = lines.pop()
        for line in lines:
            yield line

def set_socket_deadline(socket, deadline):
    socket.settimeout(max(0.0, deadline - time.time()))

def getServerNetwork(host, port, timeout=20.0):
    tempNick = "CIA-temp%03d" % random.randint(0, 999)
    realname = "Network Tester for cia.navi.cx"
    username = "cia"
    network = None
    deadline = time.time() + timeout

    # Connect and log in
    s = socket.socket()
    set_socket_deadline(s, deadline)
    s.connect((socket.gethostbyname(host), port))
    set_socket_deadline(s, deadline)
    s.sendall("NICK %s\r\nUSER %s localhost %s :%s\r\n" % (
        tempNick, username, host, realname))
        
    # Start receving the MOTD and other
    # connection-establishment-oriented messages.
    set_socket_deadline(s, deadline)
    for line in iter_socket_lines(s):
        set_socket_deadline(s, deadline)

        if line.startswith("PING"):
            s.sendall("PONG" + line[4:].rstrip() + "\r\n")

        parts = line.split(' ', 2)
        if len(parts) == 3:
            # We're looking for the server capabilities message.
            # It may have a NETWORK= key.
            if parts[1] == '005':
                match = re.search(" NETWORK=([^ ]+)", parts[2])
                if match:
                    network = match.group(1)

            # Once we hit the end of the MOTD, disconnect.
            #  376: End of MOTD
            #  422: No MOTD
            if parts[1] in ('376', '422'):
                s.sendall("QUIT\r\n")

        set_socket_deadline(s, deadline)

    s.close()
    return network

sql = []
for server in """
london.uk.eu.undernet.org
irc.perl.org
irc.ethoo.net
irc.axora.org
irc.anynet.net
irc.anynet.org
irc.krikket.org
irc.krikket.net
irc.shortcircuit.net.au
irc.quakenet.org
poseidon.dragonboricua.com
irc.da4.org
irc.deltaanime.net
irc.ookoo.org
irc.chatspike.net
irc.inspircd.org
irc.anoxs.net
irc.plethoranet.org
irc.runestatus.net
irc.freenode.net
orwell.freenode.net
lester.mithis.com
irc.osanet.cz
irc.gimp.org
irc.dev.mort-os.com
irc.oftc.net
irc.appliedirc.com
irc.rizon.net
irc.desync.com
irc.smart-serv.net
irc.coldfront.net
""".split():
    print server
    start = time.time()
    try:
        network = getServerNetwork(server, 6667)
    except socket.error, e:
        print "\tError: %s" % e
    else:
        print "\tNetwork: %r" % network

        if network:
            sql.append("UPDATE accounts_network SET description = '%s' WHERE uri = 'irc://%s/';"
                       % (network, server))

    end = time.time()
    print "\t(%.02f seconds)" % (end - start)


print
for s in sql:
    print s
