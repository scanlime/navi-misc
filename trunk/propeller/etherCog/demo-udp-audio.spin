''
'' Simple UDP to Audio example.
'' See the accompanying demo-udp-audio.py client app.
''
'' Micah Dowty <micah@navi.cx>
''

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  UDP_PORT    = 1234

  BUFFER_SHIFT = 9
  BUFFER_SIZE  = 1 << BUFFER_SHIFT
  NUM_BUFFERS  = 32                    ' Must be a power of two
  RING_SIZE    = BUFFER_SIZE * NUM_BUFFERS

  ' Default sample rate

  SAMPLE_HZ   = 44100  

  ' Gain for the sample rate servo. We try to adjust our sample
  ' rate to match the rate we're receiving packets at, but we want
  ' to do it slowly enough so as not to introduce perceptible jitter.

  OSC_GAIN    = 10000

  ' Demo board pin assignments

  LEFT_PIN    = 11
  RIGHT_PIN   = 10
  
OBJ
  netDrv  : "etherCog-enc28j60"
  sock    : "etherCog-udp-socket"
  bufq    : "etherCog-buffer-queue"
  
  debug   : "TV_Text"

VAR     
  long  readPtr, audioPeriod

  long  bufMem[BUFFER_SIZE * NUM_BUFFERS / 4]
  long  bufBD[bufq#BD_SIZE * NUM_BUFFERS]
  
PUB main | count, fill, writePtr

  debug.start(12)
  netDrv.start(3, 2, 1, 0)
  netDrv.link(sock.init(UDP_PORT))

  bufq.initFromMem(NUM_BUFFERS, BUFFER_SIZE, @bufMem, @bufBD)
  sock.rxRingInit(bufq.getAll)

  bufStart := @bufMem
  audioPeriod := (clkfreq / SAMPLE_HZ) << 16
  cognew(@audioDriver, @readPtr)
  
  repeat
    count := sock.rxRingGetCount
    writePtr := (count << BUFFER_SHIFT) & constant(RING_SIZE - 1)                           
    fill := (readPtr - writePtr) & constant(RING_SIZE - 1)                       
                     
    if fill > constant(RING_SIZE * 3 / 4)
      audioPeriod += OSC_GAIN
    elseif fill < constant(RING_SIZE * 1 / 4)
      audioPeriod -= OSC_GAIN
    
    debug.str(string(1, "Packet count: "))
    debug.hex(count, 8)
    debug.str(string(13, "Read "))
    debug.hex(readPtr, 8)
    debug.str(string(" / Write "))
    debug.hex(writePtr, 8)
    debug.str(string(13, "Fill level: "))
    debug.hex(fill, 8)
    debug.str(string(13, "Sample period: "))
    debug.hex(audioPeriod, 8)

DAT

                        org
audioDriver
                        mov     dira, init_dira
                        mov     ctra, init_ctra
                        mov     ctrb, init_ctrb

                        mov     audioPtr, bufStart     
                        mov     timer, cnt
                        add     timer, period
                        
:loop                   waitcnt timer, period   

                        add     audioPtr, #4
                        and     audioPtr, bufMask                        
                        wrlong  audioPtr, par
                        mov     r0, audioPtr
                        add     r0, bufStart                        
                        rdlong  sample, r0
        
                        mov     left, sample
                        sar     left, #16
                        mov     right, sample
                        shl     right, #16
                        sar     right, #16

                        shl     left, #14
                        shl     right, #14

                        add     left, h80000000
                        add     right, h80000000
                                     
                        mov     frqa, left
                        mov     frqb, right
                                     
                        mov     r0, par
                        add     r0, #4
                        rdlong  period, r0
                        shr     period, #16
                                               
                        jmp     #:loop

init_dira               long    (|< LEFT_PIN) | (|< RIGHT_PIN)
init_ctra               long    %00110 << 26 + LEFT_PIN     
init_ctrb               long    %00110 << 26 + RIGHT_PIN     
bufMask                 long    RING_SIZE - 1
bufStart                long    0
timer                   long    0
h80000000               long    $80000000                                       
period                  long    $10000
                
r0                      res     1
sample                  res     1
audioPtr                res     1
left                    res     1
right                   res     1  

                        fit
                       