{*******************************************************
* ViewPort Conduit                                v2.0 *
* Share Propeller Memory with the ViewPort Application *
* (C) 2007 myDanceBot.com, Inc.                        *
********************************************************}{{
Use this Object to share memory with the ViewPort Application during development.
Once memory is shared, the value of a variable can be viewed and changed while the
program is running.  Data from other objects can be displayed and their behavior
controlled from ViewPort's graphical interface.

Features:
-Transfers data at 2Mbps Full-Duplex over a Parallax USB-Serial connection or
 115Kbps over other Serial connections.
-Manages configuration of variables and registration of ViewPort objects
-Automatically switches between 2Mbps and 115Kbps depending on ViewPort setting

Use:
-Include this object in your program:
        Obj vp:"ViewPortConduit"
-Register and initialize other components:
        vp.register(component)
        *See component documentation for details on how to initialize them
-Share memory with ViewPort
        vp.share(@varA,@varC)
        *All memory between and including varA and varC will be shared with Viewport
-Start the Conduit
        vp.start()
-Optionally configure ViewPort's Interface
        vp.channels(string("a,b,c"))
        *Assign names to the longs being shared

        vp.view(string("a,b"))
        *Graph these variables

        vp.edit(string("a=True,b=10,c=5(1..10)"))
        *Set up Window editing controls for variables a,b,c:
        *a will be set up with a checkbox- True=selected=1, False=unselected=0
        *b will be set up with a textbox starting at 10, allowing entry of numbers,
                %binary number, and $hexadecimal numbers
        *c will be set up with a scrollbar, default at 5, sliding from 1..10

        vp.trigger(string("ina[16]+,a>50,b[rfx01]"))
        *ina will trigger when bit 16 rises
        *a will trigger when its value rises above 50
        *b will trigger when bit 0 is 1, bit 2 is 0, bit 3 is falling, bit 4 is rising

        vp.decode(string("adc=ina[0-7]"))
        *adc will be decoded as an analog value made up of bits 0..7 of ina

        vp.general(string("timescale=1ms/div,title=analog io,mode=dso"))
        *sets timescale to 1ms/div
        *sets title of graph to "analog io"
        *sets ViewPort mode to "dso", valid are: dso,lsa,video,fuzzy,visualizer}}
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000
  TXPIN=|<30  
  RXPIN=|<31
pub general(sptr)
  txstrPtr0:=sptr
pub channels(sptr)
  txstrPtr1:=sptr
pub view(sptr)
  txstrPtr2:=sptr
pub edit(sptr)
  txstrPtr3:=sptr
pub trigger(sptr)
  txstrPtr4:=sptr
pub decode(sptr)
  txstrPtr5:=sptr
PUB share(firstVarPtr,lastVarPtr)
  varLen:=lastVarPtr-firstVarPtr+4
  varPtr:=firstVarPtr
  txvar~
pub start|c
  txone:=varLen-1
  return cognew(@entry, 0)
pub register(mPtr)|mode,config
   
   
   
  mode:=mptr & 3
  mPtr-=mode
  config:=long[mPtr]
  if mode==0
    tvLen:=6002*4
    tvPtr:=mptr
    tvModePtr:=tvPtr+6000*4
    txtv~
  elseif mode==1'     (config>>16)==0




     memPtr:=mPtr+8
  elseif mode==2
   
    qsPtr:=mptr
    ncog:=config & 1
    qscog:=config-ncog+1
    ncog:=ncog*3+1  

    sync:=mPtr+16
    xtime:=sync+4
    qsLen:=ncog*4*360+24
    txqs~
  else
   
    fzPtr:=mPtr
    fzLen:=config
    txfz~
DAT
{************************************
* Assembly language conduit driver  *
*************************************}

                        org
entry
                        sub     tximask,#1
                        sub     tximask,txmask
                        mov     txcnt,hsec
                        add     txcnt,cnt
                       
                        waitcnt txcnt,#100
                        or      dira,txmask
                        or      outa,txmask
                       
slowCfg
                        mov     bitticks,bitticksS
                        mov     bitticks15,bitticks15S
                        mov     stopticks,stopticksS
                        mov     xt,xtS
                        jmp     #txCfgStr
fastData
                        mov     bitticks,#40
                        mov     bitticks15,#44
                        mov     stopticks,#80
                        mov     xt,xtF
                        jmp     #txData                        

receive                 or      outa,txmask
:r1                     test    rxmask,ina      wc
            if_c        jmp     #:r1
:s1                     test    rxmask,ina      wc   
            if_nc       jmp     #:s1
                        mov     rxcnt,cnt
                        add     rxcnt,bitticks15                                                                                                                                                      
                        mov     t2,#20       
:bit                    waitcnt rxcnt,bitticks
                        test    rxmask,ina      wc
                        rcr     t1,#1
                        djnz    t2,#:bit
                       
                        mov     t3,#30 
:bit3                   waitcnt rxcnt,bitticks
                        test    rxmask,ina      wc
                        rcr     t4,#1
                        djnz    t3,#:bit3
'process t1,t2                        
                        mov     t2,t1
                        shr     t2,#32-19
                        and     t2,#$FF
                        cmp     t2,#255         wz
                        shr     t1,#32-9
                        and     t1,#$FF
                        mov     txcnt,cnt
                        add     txcnt,#200                        
              if_z      add     t1,#:docontrol
              if_z      jmp     t1
             
       
       
       
       
       
       
                        
                        cmp     t2,#254         wz
              if_z      mov     t2,tvModePtr
              if_z      jmp     #:makeInt
                        cmp     t2,#250         wc
              if_nc     sub     t2,#250
              if_nc     shl     t2,#2
              if_nc     add     t2,qsPtr
              if_nc     jmp     #:makeInt
                        cmp     t2,#100         wc
              if_nc     sub     t2,#100
              if_nc     shl     t2,#2
              if_nc     add     t2,fzPtr
              if_nc     jmp     #:makeInt
                        shl     t2,#2
                        add     t2,varPtr




                       
                       
 :makeInt               mov     t3,t4
                        shr     t3,#23
                        and     t3,#$FF
                        shl     t3,#24

                        or      t1,t3
                        
                        mov     t3,t4
                        shr     t3,#13
                        and     t3,#$FF
                        shl     t3,#16

                        or      t1,t3
                        
                        mov     t3,t4
                        shr     t3,#3
                        and     t3,#$FF
                        shl     t3,#8

                        or      t1,t3
                        wrlong  t1,t2
                        mov     txcnt,cnt
                        add     txcnt,#100
                        jmp     #txData

:docontrol              jmp     #sleep    
                        jmp     #txMem    
                        jmp     #txData   
                        jmp     #restart  
                        jmp     #slowcfg  
                        jmp     #fastData 
                        mov     t1,qscog  
                        and     t1,#7     
                        mov     t2,ncog                        
 stopC                  cogstop t1
                        add     t1,#1
                        djnz    t2,#stopC
                        jmp     #txData
 sleep                  wrlong  t1,memPtr
                        mov     txcnt,cnt
                        add     txcnt,twosec
                        waitcnt txcnt,#0
                        waitpne rxmask,rxmask  
                        mov     txcnt,cnt
                        add     txcnt,#100
                        jmp     #txData 
                        
  restart               mov     t1,cnt
                        wrlong  t1,sync
                        wrlong  xt,xtime
                        mov     t1,qscog   
                        mov     t2,ncog
  startC                coginit t1 nr
                        add     t1,#1
                        djnz    t2,#startC
                        jmp     #txData     
txData                  tjnz    txone,#txMultB
                        mov     txByte,#$EE
                        call    #transmit

tx1Byte                 rdbyte  txByte,varPtr
                        call    #transmit
                        jmp     #tx1Byte

txMultB                 mov     txByte,#$EE
                        call    #transmit
         txvar          jmp     #txqs  
                        mov     t2,varLen
                        mov     t3,varPtr
                        call    #txbytes
         txqs           jmp     #txtv                              
                        mov     t2,qsLen
                        mov     t3,qsPtr
                        call    #txbytes
         txtv           jmp     #txfz                              
                        mov     t2,tvLen
                        mov     t3,tvPtr
                        call    #txbytes                        
         txfz           jmp     #txMultB
                        mov     t2,fzLen
                        mov     t3,fzPtr
        dotx1           rdbyte  txByte,t3
                        add     t3,#1
                        call    #transmit
                        djnz    t2,#dotx1                       
     jtx                jmp     #txMultB
txbytes                 
 :dotx                  rdbyte  txByte,t3
                        add     t3,#1
                        call    #transmit
                        djnz    t2,#txbytes
txbytes_ret             ret

txMem                   call    #txhdr
                        mov     t2,memPtr
                        rdlong  t3,t2
:memL                   rdbyte  txByte,t2
                        call    #transmit
                        add     t2,#1
                        djnz    t3,#:memL
                        jmp     #txData                                                

txCfgStr                call    #txhdr
                        mov     txByte,varLen
                        call    #transmit
                        mov     txByte,qsLen 
                        call    #transLong
                        mov     txByte,tvLen 
                        call    #transLong
                        mov     txByte,fzLen 
                        call    #transmit
                        mov     txByte,memPtr 
                        call    #transLong
                        mov     t2,txstrptr0
                        call    #txStr
                        mov     t2,txstrptr1
                        call    #txStr
                        mov     t2,txstrptr2
                        call    #txStr
                        mov     t2,txstrptr3
                        call    #txStr
                        mov     t2,txstrptr4
                        call    #txStr
                        mov     t2,txstrptr5
                        call    #txStr
                        mov     txByte,#$EE
                        call    #transmit       
                        jmp     #txCfgStr                        
txStr                   add     txcnt,bitticks
                        tjnz    t2,#:chars
                        mov     txByte,#0
                        call    #transmit
                        jmp     #txStr_ret
:chars                  rdbyte  txByte,t2
                        mov     t3,txByte
                        call    #transmit
                        add     t2,#1
                        tjnz    t3,#:chars
txStr_ret               ret                        

txhdr                   mov     t2,#20
:nums                   mov     txByte,#254
                        call    #transmit
                        djnz    t2,#:nums
                        mov     txByte,#11
                        call    #transmit
                        mov     txByte,#253
                        call    #transmit
txhdr_ret               ret

transLong               call #transmit
                        call #transmit
                        call #transmit
                        call #transmit
transLong_ret           ret

transmit                waitcnt txcnt,bitticks   
                        and     outa,tximask    
                        mov     t1,#8
:bit                    test    rxmask,ina      wc                        
            if_nc       jmp     #receive
                        shr     txByte,#1       wc
                        waitcnt txcnt,bitticks     
                        muxc    outa,txmask
                        djnz    t1,#:bit
                        waitcnt txcnt,stopticks                                          
                        or      outa,txmask     
transmit_ret            ret
txstrptr0               long    0
txstrptr1               long    0
txstrptr2               long    0
txstrptr3               long    0
txstrptr4               long    0
txstrptr5               long    0

varPtr                  long    0
qsPtr                   long    0
tvPtr                   long    0
fzPtr                   long    0
memPtr                  long    0

varLen                  long    0
qsLen                   long    0
tvLen                   long    0
fzLen                   long    0


modeN                   long    0
tvModePtr               long    0
txone                   long    1



bitticksS               long    694
bitticks15S             long    1026
stopticksS              long    1388
xtS                     long    360*2000*20
xtF                     long    360*2000   

twosec                  long    160_000_000
hsec                    long    100_000


qscog                   long    0
sync                    long    0
xtime                   long    0
ncog                    long    0

t1                      long    0
t2                      long    0
t3                      long    0
t4                      long    0
rxmask                  long    RXPIN
txmask                  long    TXPIN
tximask                 long    0
xt                      res     1
rxcnt                   res     1
txByte                  res     1
txcnt                   res     1
bitticks                res     1
bitticks15              res     1
stopticks               res     1
