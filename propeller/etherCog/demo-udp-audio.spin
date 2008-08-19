''
'' UDP to Audio example.
''
'' This example expects a periodic stream of identically
'' sized UDP packets. We buffer those packets in a 16kB ring
'' buffer, and output them as 16-bit 44.1kHz stereo audio
'' on the Propeller Demo Board's headphone jack.
''
'' This is a good example for the rxRing buffer style. The
'' Spin cog does no explicit flow control between the etherCog
'' and our audio driver cog- we just try our best to keep
'' the two on opposite ends of our ring buffer, to increase our
'' tolerance to network jitter.
''
'' We use a simple digital PLL to adjust our audio sample rate
'' and phase to match the input source.
''
'' See the accompanying demo-udp-audio.py client app.
''
'' Micah Dowty <micah@navi.cx>
''

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  UDP_PORT    = 6502

  BUFFER_SHIFT      = 10
  BUFFER_SIZE       = 1 << BUFFER_SHIFT
  NUM_BUFFERS_SHIFT = 4
  NUM_BUFFERS       = 1 << NUM_BUFFERS_SHIFT
  RING_SIZE_SHIFT   = BUFFER_SHIFT + NUM_BUFFERS_SHIFT
  RING_SIZE         = 1 << RING_SIZE_SHIFT

  ' Default sample rate
  SAMPLE_HZ   = 44100  

  ' How fast to adjust the sample rate to match the source frequency.
  PERIOD_GAIN = 100

  ' How fast to adjust the sample rate to match the source phase.
  ' The magnitude of this gain will also depend on the size of the phase
  ' filter, below.
  '
  ' Larger values here will improve phase lock speed, but they will increase
  ' jitter in the audio sample rate. Smaller values will make it harder to
  ' lock on to the input phase.

  PHASE_GAIN = 12

  ' Size of the box filter for phase difference.
  ' Must be a power of two. Larger values may improve sample
  ' rate stability by reducing noise in our measured phase.
  
  PHASE_FILTER_SIZE = 32
  
  ' Demo board pin assignments
  LEFT_PIN    = 11
  RIGHT_PIN   = 10
  
OBJ
  netDrv  : "etherCog-enc28j60"
  sock    : "etherCog-udp-socket"
  bufq    : "etherCog-buffer-queue"
  
  debug   : "TV_Text"

VAR
  long  drv_sampleCount
  long  drv_period

  long  default_period
  long  servo_period

  word  rx_count
  word  play_count
  word  rx_phase
  word  play_phase

  long  count_diff
  long  phase_diff
  long  phase_filter[PHASE_FILTER_SIZE]
  long  phase_filter_accum
  byte  phase_filter_index
  
  long  bufMem[BUFFER_SIZE * NUM_BUFFERS / 4]
  long  bufBD[bufq#BD_SIZE * NUM_BUFFERS]
  
PUB main
  debug.start(12)
  netDrv.start(3, 2, 1, 0)
  netDrv.link(sock.init(UDP_PORT))

  initPlayback

  repeat
    initSocket
    resetPlayback
    waitForStream
    playStream

PRI initSocket
  ' Initialize/unbind the socket. This prepares it to listen
  ' for packets from any peer. As soon as it receives any packet,
  ' it binds to that peer.

  sock.unbind
  bufq.initFromMem(NUM_BUFFERS, BUFFER_SIZE, @bufMem, @bufBD)
  sock.rxRingInit(bufq.getAll)
    
PRI waitForStream
  ' Wait for a client to start sending us UDP packets

  debug.out(0)
  debug.str(string("etherCog UDP Audio demo", 13, "Micah Dowty <micah@navi.cx>", 13, 13))
  debug.str(string("Waiting for a UDP stream on port "))
  debug.dec(UDP_PORT)
  
  repeat until sock.isBound

PRI initPlayback
  ' Initialize our assembly language audio player cog

  bufStart := @bufMem
  drv_period := default_period := (clkfreq / SAMPLE_HZ) << 16
  cognew(@audioDriver, @drv_sampleCount)
  
PRI resetPlayback
  ' Reset the audio player cog to defaults, and clear its buffer

  longfill(@bufMem, 0, constant(BUFFER_SIZE * NUM_BUFFERS / 4))
  drv_period := default_period
  
PRI playStream
  ' Play a UDP audio stream. Returns if the stream is interrupted.

  debug.out(0)
  drv_sampleCount~
  phase_filter_accum~
  wordfill(@phase_filter, 0, PHASE_FILTER_SIZE)
  servo_period := default_period
  
  repeat
    ' How many packets have we received? (Modulo 2^16)
    rx_count := sock.rxRingGetCount

    ' How many packets have we played? (Modulo 2^16)
    play_count := drv_sampleCount >> constant(BUFFER_SHIFT - 2)

    ' Calculate the difference between received and played packets,
    ' also modulo 2^16. We use this to adjust our sample rate,
    ' and to determine if the stream has been dropped.

    count_diff := play_count - rx_count
    ~~count_diff
    
    if count_diff > 100
      ' We've played many more packets than we've received.
      ' Assume the stream dropped out.
      return

    ' Slowly adjust our playback sample rate, to match the
    ' rate at which we're receiving samples. This slaves our
    ' playback clock frequency to the clock in the application
    ' which is sending us this stream.

    servo_period += count_diff * PERIOD_GAIN

    ' Calculate the phases (location in the ring buffer)
    ' of the receiver and the playback cog. We want to synchronize
    ' our playback to be 180 degrees out of phase with the receiver
    ' cog, so we have as much jitter headroom as possible.

    rx_phase := (rx_count << BUFFER_SHIFT) & constant(RING_SIZE - 1)
    play_phase := (drv_sampleCount << 2) & constant(RING_SIZE - 1)
    phase_diff := (play_phase - rx_phase - constant(RING_SIZE / 2))

    ' Sign extend the RING_SIZE_SHIFT-bit-wide value, so we can
    ' represent positive and negative shifts.
    phase_diff := (phase_diff << constant(32 - RING_SIZE_SHIFT)) ~> constant(32 - RING_SIZE_SHIFT)

    ' Filter the phase difference, since it's quite a noisy signal.
    phase_filter_accum -= phase_filter[phase_filter_index &= constant(PHASE_FILTER_SIZE - 1)]
    phase_filter_accum += phase_filter[phase_filter_index++] := phase_diff    

    ' Temporarily adjust the period to nudge us back in phase
    drv_period := servo_period + phase_filter_accum * PHASE_GAIN
                      
    showPlayStatus

PRI showPlayStatus | peerAddr
  ' Display a text mode status screen, with playback info.

  debug.str(string(1, "Streaming from "))
  peerAddr := sock.peerAddr
  debug.dec(peerAddr >> 24)
  debug.out(".")
  debug.dec((peerAddr >> 16) & $FF)
  debug.out(".")
  debug.dec((peerAddr >> 8) & $FF)
  debug.out(".")
  debug.dec(peerAddr & $FF)
  debug.out(":")
  debug.dec(sock.peerPort)
  

  debug.str(string(13, 13, "Packets: RX="))
  debug.hex(rx_count, 4)
  debug.str(string(" Play="))
  debug.hex(play_count, 4)
  debug.str(string(" Diff="))
  debug.hex(count_diff, 4)

  debug.str(string(13, "Phase: RX="))
  debug.hex(rx_phase, 4)
  debug.str(string(" Play="))
  debug.hex(play_phase, 4)
  debug.str(string(" Diff="))
  debug.hex(phase_filter_accum, 8)

  debug.str(string(13, "Period="))
  debug.hex(drv_period, 8)

    
DAT
' Assembly language driver for playing audio from our receive ring buffer.

                        org
audioDriver
                        mov     dira, init_dira
                        mov     ctra, init_ctra
                        mov     ctrb, init_ctrb

                        mov     timer, cnt
                        add     timer, period
                        
:loop                   waitcnt timer, period

                        rdlong  r0, par
                        add     r0, #1                  ' Update sample counter
                        wrlong  r0, par

                        shl     r0, #2                  ' Calculate sample address
                        and     r0, bufMask
                        add     r0, bufStart
                        rdlong  sample, r0
        
                        mov     left, sample            ' Split into left/right channels
                        sar     left, #16
                        mov     right, sample
                        shl     right, #16
                        sar     right, #16

                        shl     left, #14               ' Scale volume
                        shl     right, #14
                        add     left, h80000000         ' Signed -> Biased
                        add     right, h80000000                                     
                        mov     frqa, left              ' Out to counter DACs
                        mov     frqb, right
                                     
                        mov     r0, par                 ' Read next period from the hub
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
period                  long    $1000

r0                      res     1
sample                  res     1
left                    res     1
right                   res     1  

                        fit
                       