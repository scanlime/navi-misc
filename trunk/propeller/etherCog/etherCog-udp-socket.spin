{{                                              

 etherCog-udp-socket
──────────────────────────────────────────────────────────────────

This is a UDP socket object, for use with etherCog. Applications
should create one instance of this object for every UDP socket they
want to use simultaneously.

This module is just a Spin API for convenience. The TCP/IP
implementation behind this socket is contained entirely in the
driver cog. If you want to use a socket from assembly language,
you can use this file as a reference for how to perform various
socket operations using the Socket Descriptor structure.

For a full description of Socket Descriptors and Buffer Descriptors,
see the documentation comments in the etherCog driver source.

┌───────────────────────────────────┐
│ Copyright (c) 2008 Micah Dowty    │               
│ See end of file for terms of use. │
└───────────────────────────────────┘

}}

CON
  BD_SIZE = 2     ' Buffer descriptor size, in longs

VAR
  ' Socket Descriptor structure.
  '
  ' See the etherCog driver's documentation.
  ' Note that we must declare this entirely using
  ' longs, not a mix of longs and words, because Spin
  ' always groups VARs by size before allocating them.
  
  long  sd_next_flags
  long  sd_ports
  long  sd_peer_ip
  long  sd_rx
  long  sd_tx

PUB init(port) : p
  '' Initialize the UDP port. For convenience, we also return
  '' the port's Socket Descriptor pointer (see 'ptr' below).
  '' This pointer must be linked into the driver cog, or linked
  '' to another socket which is in turn linked to the driver cog.

  p := @sd_next_flags
  longfill(p, 0, 5)
  sd_ports := port << 16

PUB ptr : p
  '' Return a pointer to our Socket Descriptor. This is the
  '' value passed to the driver's link() function.

  p := @sd_next_flags

PUB link(nextSocket) : p
  '' Link this socket to the provided 'nextSocket' socket descriptopr.
  '' For convenience, this also returns a pointer to our own socket
  '' descriptor.
  ''
  '' This function can be used for building linked lists of socket
  '' descriptors, making it easier to develop objects which use
  '' many sockets. A single high-level object, like a web server,
  '' may link together several sockets into a single socket list
  '' that is passed back to the main program. This way even the web
  '' server does not need any direct coupling to the etherCog driver.

  WORD[@sd_next_flags] := nextSocket
  p := @sd_next_flags
  
PUB bind(ip, port)
  '' Bind this socket to a single IP address, specified as
  '' a single 32-bit long, and a single port. After binding,
  '' all transmitted packets are directed to this IP and port,
  '' and we can only receive packets from the same IP and port.
  ''
  '' A socket may only be bound if it has not yet been linked
  '' to the driver cog, or if it is already bound to another
  '' IP address. Unbound sockets may not be bound, because
  '' it causes a race condition: The driver cog may bind an
  '' unbound socket at any time, because it received a packet.

  WORD[@sd_ports] := 0             ' Disable packet reception without unbinding
  sd_peer_ip := ip              ' Atomically re-bind
  WORD[@sd_ports] := port          ' Re-enable on new port

PUB unbind
  '' Unbind this socket. Unbound sockets cannot transmit, and they
  '' can receive from any peer. Sockets start out in the unbound
  '' state. You can only unbind a socket if it is bound.

  WORD[@sd_ports] := 0
  sd_peer_ip := 0    

PUB peerAddr : p
  '' Get this socket's peer IP, as a single 32-bit long.

  p := sd_peer_ip

PUB peerPort : p
  '' Get this socket's peer port.

  p := WORD[@sd_ports]

PUB isBound : bool
  '' Is this socket bound to a peer? Sockets start out unbound.
  '' They may be explicitly bound/unbound by their owner, or an
  '' unbound socket may be implicitly bound when a packet is
  '' first received on that socket.

  bool := sd_peer_ip <> 0 

PUB localPort : p
  '' Get this socket's local port. Usually this is the same
  '' value passed to init(), but if this socket was initialized
  '' with no port (zero) and bound to a driver cog, this function
  '' will tell you which ephemeral port number was assigned to
  '' this socket.

  p := WORD[@sd_ports + 2]

PUB recv(buf, maxLength, bdPtr) : actualLength
  '' XXX: This function is temporary, as blocking reads are not
  '' likely to be useful in practice. Packets received when we
  '' aren't sitting inside read() would always be dropped!

  LONG[bdPtr] := 0
  LONG[bdPtr+4] := (maxLength << 16) | buf
  sd_rx := bdPtr

' repeat while sd_rx >> 16 <> 1
 ' actualLength := bd[0] & $FFFF


DAT

{{
┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                   TERMS OF USE: MIT License                                                  │                                                            
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation    │ 
│files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,    │
│modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software│
│is furnished to do so, subject to the following conditions:                                                                   │
│                                                                                                                              │
│The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.│
│                                                                                                                              │
│THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE          │
│WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR         │
│COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   │
│ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                         │
└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
}}   