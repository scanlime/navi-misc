{{                                              

 etherCog-udp-socket
──────────────────────────────────────────────────────────────────

This is a UDP socket object, for use with etherCog. Applications
should create one instance of this object for every UDP socket they
want to use simultaneously.

┌───────────────────────────────────┐
│ Copyright (c) 2008 Micah Dowty    │               
│ See end of file for terms of use. │
└───────────────────────────────────┘

}}

VAR
  ' Socket Descriptor structure.
  ' See the etherCog driver's documentation.
  word  sd_local_port
  word  sd_next
  word  sd_peer_port
  word  sd_peer_mac_high
  long  sd_peer_mac_low
  long  sd_peer_ip
  long  sd_rx
  long  sd_tx

PUB init(port) : p
  '' Initialize the UDP port. For convenience, we also return
  '' the port's Socket Descriptor pointer (see 'ptr' below).
  '' This pointer must be linked into the driver cog, or linked
  '' to another socket which is in turn linked to the driver cog.

  p := @sd_local_port
  longfill(p, 0, 6)
  sd_local_port := port

PUB ptr : p
  '' Return a pointer to our Socket Descriptor. This is the
  '' value passed to the driver's link() function.

  p := @sd_local_port  

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

  sd_next := (sd_next & !%11) | nextSocket
  p := @sd_local_port
  
PUB bind(ip)
  '' Bind this socket to a single IP address, specified as
  '' a single 32-bit long. After binding, all transmitted
  '' packets are directed to this IP, and we can only receive
  '' packets from   
    

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