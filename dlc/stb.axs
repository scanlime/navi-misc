program_name = 'xine-control'
module_name  = 'xine-control'

(* This module is for controlling the custom set-top-box hardware in the *)
(* DLC Collaboratory. These are basically small PCs running linux, and   *)
(* playback is handled via the xine [http://xine.sf.net] media player    *)
(* All video files are assumed to be offset from a root directory of     *)
(* /media, which is an nfs mount from the video server.                  *)


(* constants *)
stb_port	= 6789		// default xine telnet control port

(* ugh, you'd think these would exist in the language *)
true		= 1
false		= 0
ip_tcp		= 1
ip_udp		= 2

(* global variables *)
char	stb_addresses[13][7]

(* startup code *)
define_start
stb_addresses[0] = '128.138.74.20'
stb_addresses[1] = '128.138.74.21'
stb_addresses[2] = '128.138.74.22'
stb_addresses[3] = '128.138.74.23'
stb_addresses[4] = '128.138.74.24'
stb_addresses[5] = '128.138.74.25'
stb_addresses[6] = '128.138.74.26'
