{{

etherCog
--------

This is a fast single-cog driver for the ENC28J60, an easy to
use SPI-attached 10baseT chip from Microchip. It implements
the chip's SPI protocol, as well as a fast low-level packet
dispatch system, ARP, and UDP.
  
┌───────────────────────────────────┐
│ Copyright (c) 2008 Micah Dowty    │               
│ See end of file for terms of use. │
└───────────────────────────────────┘

}}

VAR
  byte cog

PUB start(csPin, sckPin, siPin, soPin, intPin, debug) : okay
  '' Initialize the ethernet driver. This resets and
  '' initializes the ENC28J60 chip, then starts a single
  '' cog for receiving and transmitting packets.
            
  cs_mask  := |< csPin
  si_mask  := |< siPin
  so_mask  := |< soPin
  int_mask := |< intPin

  init_dira := cs_mask | si_mask | (|< sckPin)

  ' We use CTRA to generate the SPI clock
  init_ctra := sckPin | constant(%00100 << 26) 
  
  okay := cog := 1 + cognew(@entry, debug)

PUB stop
  if cog > 0
    cogstop(cog - 1)
    cog~

CON

'==============================================================================
' ENC28J60 Constants
'==============================================================================

' SPI Commands

SPI_RCR = %000_00000   ' Read Control Register
SPI_RBM = %001_11010   ' Read Buffer Memory
SPI_WCR = %010_00000   ' Write Control Register
SPI_WBM = %011_11010   ' Write Buffer Memory
SPI_BFS = %100_00000   ' Bit Field Set
SPI_BFC = %101_00000   ' Bit Field Clear
SPI_SRC = %111_11111   ' System Reset Command

' Register names.
'
' Each register's address is encoded: Bits 4:0 are the register's
' position within its bank, bits 6:5 are its bank number, bit
' 7 is cleared if the register is available in all banks, and bit 8
' is set if the register will be preceeded by a dummy byte on read.
'
' These encoded register names can be used with the reg_Read and reg_Write routines.

' All banks
EIE       = (%00_00 << 5) | $1B
EIR       = (%00_00 << 5) | $1C
ESTAT     = (%00_00 << 5) | $1D
ECON2     = (%00_00 << 5) | $1E
ECON1     = (%00_00 << 5) | $1F

' Bank 0      
ERDPTL    = (%01_00 << 5) | $00
ERDPTH    = (%01_00 << 5) | $01
EWRPTL    = (%01_00 << 5) | $02
EWRPTH    = (%01_00 << 5) | $03
ETXSTL    = (%01_00 << 5) | $04
ETXSTH    = (%01_00 << 5) | $05
ETXNDL    = (%01_00 << 5) | $06
ETXNDH    = (%01_00 << 5) | $07
ERXSTL    = (%01_00 << 5) | $08
ERXSTH    = (%01_00 << 5) | $09
ERXNDL    = (%01_00 << 5) | $0A
ERXNDH    = (%01_00 << 5) | $0B
ERXRDPTL  = (%01_00 << 5) | $0C
ERXRDPTH  = (%01_00 << 5) | $0D
ERXWRPTL  = (%01_00 << 5) | $0E
ERXWRPTH  = (%01_00 << 5) | $0F
EDMASTL   = (%01_00 << 5) | $10
EDMASTH   = (%01_00 << 5) | $11
EDMANDL   = (%01_00 << 5) | $12
EDMANDH   = (%01_00 << 5) | $13
EDMADSTL  = (%01_00 << 5) | $14
EDMADSTH  = (%01_00 << 5) | $15
EDMACSL   = (%01_00 << 5) | $16
EDMACSH   = (%01_00 << 5) | $17
              
' Bank 1
EHT0      = (%01_01 << 5) | $00
EHT1      = (%01_01 << 5) | $01
EHT2      = (%01_01 << 5) | $02
EHT3      = (%01_01 << 5) | $03
EHT4      = (%01_01 << 5) | $04
EHT5      = (%01_01 << 5) | $05
EHT6      = (%01_01 << 5) | $06
EHT7      = (%01_01 << 5) | $07
EPMM0     = (%01_01 << 5) | $08
EPMM1     = (%01_01 << 5) | $09
EPMM2     = (%01_01 << 5) | $0A
EPMM3     = (%01_01 << 5) | $0B
EPMM4     = (%01_01 << 5) | $0C
EPMM5     = (%01_01 << 5) | $0D
EPMM6     = (%01_01 << 5) | $0E
EPMM7     = (%01_01 << 5) | $0F
EPMCSL    = (%01_01 << 5) | $10
EPMCSH    = (%01_01 << 5) | $11
EPMOL     = (%01_01 << 5) | $14
EPMOH     = (%01_01 << 5) | $15
ERXFCON   = (%01_01 << 5) | $18
EPKTCNT   = (%01_01 << 5) | $19
              
' Bank 2
MACON1    = (%11_10 << 5) | $00
MACON3    = (%11_10 << 5) | $02
MACON4    = (%11_10 << 5) | $03
MABBIPG   = (%11_10 << 5) | $04
MAIPGL    = (%11_10 << 5) | $06
MAIPGH    = (%11_10 << 5) | $07
MACLCON1  = (%11_10 << 5) | $08
MACLCON2  = (%11_10 << 5) | $09
MAMXFLL   = (%11_10 << 5) | $0A
MAMXFLH   = (%11_10 << 5) | $0B
MICMD     = (%11_10 << 5) | $12
MIREGADR  = (%11_10 << 5) | $14
MIWRL     = (%11_10 << 5) | $16
MIWRH     = (%11_10 << 5) | $17
MIRDL     = (%11_10 << 5) | $18
MIRDH     = (%11_10 << 5) | $19
              
' Bank 3
MAADR5    = (%11_11 << 5) | $00
MAADR6    = (%11_11 << 5) | $01
MAADR3    = (%11_11 << 5) | $02
MAADR4    = (%11_11 << 5) | $03
MAADR1    = (%11_11 << 5) | $04
MAADR2    = (%11_11 << 5) | $05
EBSTSD    = (%01_11 << 5) | $06
EBSTCON   = (%01_11 << 5) | $07
EBSTCSL   = (%01_11 << 5) | $08
EBSTCSH   = (%01_11 << 5) | $09
MISTAT    = (%11_11 << 5) | $0A
EREVID    = (%01_11 << 5) | $12
ECOCON    = (%01_11 << 5) | $15
EFLOCON   = (%01_11 << 5) | $17
EPAUSL    = (%01_11 << 5) | $18
EPAUSH    = (%01_11 << 5) | $19

' PHY registers

PHCON1    = $00
PHSTAT1   = $01
PHID1     = $02
PHID2     = $03
PHCON2    = $10
PHSTAT2   = $11
PHIE      = $12
PHIR      = $13
PHLCON    = $14

' Individual register bits

'EIE
RXERIE    = 1 << 0
TXERIE    = 1 << 1
TXIE      = 1 << 3
LINKIE    = 1 << 4
DMAIE     = 1 << 5
PKTIE     = 1 << 6
INTIE     = 1 << 7

'EIR
RXERIF    = 1 << 0
TXERIF    = 1 << 1
TXIF      = 1 << 3
LINKIF    = 1 << 4
DMAIF     = 1 << 5
PKTIF     = 1 << 6

'ESTAT
CLKRDY    = 1 << 0
TXABRT    = 1 << 1
RXBUSY    = 1 << 2
LATECOL   = 1 << 4
BUFER     = 1 << 6
INT       = 1 << 7

'ECON2
VRPS      = 1 << 3
PWRSV     = 1 << 5
PKTDEC    = 1 << 6
AUTOINC   = 1 << 7

'ECON1
BSEL0     = 1 << 0
BSEL1     = 1 << 1
RXEN      = 1 << 2
TXRTS     = 1 << 3
CSUMEN    = 1 << 4
DMAST     = 1 << 5
RXRST     = 1 << 6
TXRST     = 1 << 7

'ERXFCON
BCEN      = 1 << 0
MCEN      = 1 << 1
HTEN      = 1 << 2
MPEN      = 1 << 3
PMEN      = 1 << 4
CRCEN     = 1 << 5
ANDOR     = 1 << 6
UCEN      = 1 << 7

'MACON1
MARXEN    = 1 << 0
PASSALL   = 1 << 1
RXPAUS    = 1 << 2
TXPAUS    = 1 << 3

'MACON3 
FULDPX    = 1 << 0
FRMLNEN   = 1 << 1
HFRMEN    = 1 << 2
PHDREN    = 1 << 3
TXCRCEN   = 1 << 4
PADCFG0   = 1 << 5
PADCFG1   = 1 << 6
PADCFG2   = 1 << 7

'MACON4 
NOBKOFF   = 1 << 4
BPEN      = 1 << 5
DEFER     = 1 << 6

'MICMD
MIIRD     = 1 << 0
MIISCAN   = 1 << 1

'EBSTCON
BISTST    = 1 << 0
TME       = 1 << 1
TMSEL0    = 1 << 2
TMSEL1    = 1 << 3
PSEL      = 1 << 4
PSV0      = 1 << 5
PSV1      = 1 << 6
PSV2      = 1 << 7

'MISTAT 
BUSY      = 1 << 0
SCAN      = 1 << 1
NVALID    = 1 << 2

'EFLOCON
FCEN0     = 1 << 0
FCEN1     = 1 << 1
FULDPXS   = 1 << 2
    
    
DAT

'==============================================================================
' Driver Cog
'==============================================================================

                        org

                        '======================================================
                        ' Initialization
                        '======================================================

entry
                        mov     dira, init_dira
                        mov     outa, cs_mask
                        mov     ctra, init_ctra

                        mov     spi_reg, #SPI_SRC       ' SPI soft reset
                        andn    outa, cs_mask
                        call    #spi_Write8
                        or      outa, cs_mask
                        

                        '======================================================
                        ' Main Loop
                        '======================================================

mainLoop
                        
                        'mov     phy_name, #PHSTAT2
                        'call    #phy_Read
                        'wrlong  phy_data, par

                        mov     reg_name, $
                        
                        mov     r0, cnt
                        mov     r1, #1
                        shl     r1, #12
                        add     r0, r1
                        waitcnt r0, r1
                        
                        jmp     #mainLoop


                        '======================================================
                        ' Low-level ENC28J60 control
                        '======================================================
                        
                        ' This layer implements low-level control over the
                        ' ENC28J60's main register file and PHY registers.
                        ' It's responsible for memory banking, controlling the
                        ' CS line, and performing PHY read/write sequences.

                        '
                        ' reg_Read --
                        '
                        '   Read an 8-bit register, named by reg_name.
                        '   Returns the value via reg_data.
                        '
reg_Read                call    #reg_BankSel
                        andn    outa, cs_mask
                        or      spi_reg, #SPI_RCR
                        call    #spi_Write8
                        test    reg_name, #$100 wz      ' Read dummy byte?
              if_nz     call    #spi_Read8              
                        call    #spi_Read8
                        mov     reg_data, spi_reg
                        or      outa, cs_mask
reg_Read_ret            ret

                        '
                        ' reg_Write --
                        '
                        '   Write any 8-bit register, named by reg_name,
                        '   other than ECON1. Uses data from the low 8 bits
                        '   of reg_data.
                        '
reg_Write               call    #reg_BankSel
                        andn    outa, cs_mask
                        or      spi_reg, #SPI_WCR
                        call    #spi_Write8
                        mov     spi_reg, reg_data
                        call    #spi_Write8
                        or      outa, cs_mask
reg_Write_ret           ret

                        '
                        ' reg_WriteECON1 --
                        '
                        '   Write to ECON1, using data from the low 8 bits
                        '   of reg_ECON1.
                        '
reg_WriteECON1          andn    outa, cs_mask
                        mov     spi_reg, #SPI_WCR | (%11111 & ECON1)
                        call    #spi_Write8
                        mov     spi_reg, reg_ECON1
                        call    #spi_Write8
                        or      outa, cs_mask
reg_WriteECON1_ret      ret

                        '
                        ' reg_BankSel --
                        '
                        '   Select the proper bank for the register in
                        '   reg_name, and load the lower 5 bits of the
                        '   register name into spi_data.
                        '
reg_BankSel             test    reg_name, #$80 wz       ' Is the register unbanked?
              if_nz     mov     r0, reg_name            ' Compute difference between...
              if_nz     shr     r0, #5                  '   reg_name[6:5]
              if_nz     xor     r0, reg_ECON1           '   and reg_ECON[1:0]
              if_nz     and     r0, #%11 wz             ' Already in the right bank?
              if_nz     xor     reg_ECON1, r0           ' Set new bank
              if_nz     call    #reg_WriteECON1                        
                        mov     spi_reg, reg_name       ' Save lower 5 bits
                        and     spi_reg, #%11111
reg_BankSel_ret         ret

                        '
                        ' phy_Read --
                        ' 
                        '   Read a 16-bit PHY register, named by phy_name,
                        '   and store the value in phy_data.
                        '
phy_Read                mov     reg_name, #MIREGADR     ' Set PHY address
                        mov     reg_data, phy_name
                        call    #reg_Write
                        mov     reg_name, #MICMD        ' Begin read
                        mov     reg_data, #MIIRD
                        call    #reg_Write
                        call    #phy_Wait               ' Wait for PHY to finish (~10uS)
                        mov     reg_name, #MICMD        ' End read
                        mov     reg_data, #0
                        call    #reg_Write
                        mov     reg_name, #MIRDL        ' Read low byte
                        call    #reg_Read
                        mov     phy_data, reg_data
                        mov     reg_name, #MIRDH        ' Read high byte
                        call    #reg_Read
                        shl     reg_data, #8
                        or      phy_data, reg_data
phy_Read_ret            ret                        

                        '
                        ' phy_Wait --
                        '
                        '   Wait for the PHY to become non-busy.
                        '
                        '   XXX: Might want a timeout here...
                        '
phy_Wait                mov     reg_name, #MISTAT
                        call    #reg_Read
                        test    reg_data, #BUSY wz
              if_nz     jmp     #phy_Wait
phy_Wait_ret            ret

                        
                        '======================================================
                        ' Fast SPI Engine
                        '======================================================
                        
                        ' This SPI engine uses a very carefully timed CTRA to
                        ' generate the clock pulses while an unrolled loop reads
                        ' the bus at two instructions per bit. This gives an SPI
                        ' clock rate 1/8 the system clock. At an 80Mhz system clock,
                        ' that's an SPI data rate of 10 Mbit/sec!
                        '
                        ' Only touch this code if you have an oscilloscope at hand :)

                        '
                        ' spi_Write8 --
                        '
                        '   Write the upper 8 bits of spi_reg to the SPI port.
                        ' 
spi_Write8              shl     spi_reg, #24            ' Left justify MSB
                        rcl     spi_reg, #1 wc          ' Shift bit 7
                        mov     phsa, #0                ' Rising edge at center of each bit
                        muxc    outa, si_mask           ' Output bit 7
                        mov     frqa, frq_8clk          ' First clock edge, period of 8 clock cycles
                        rcl     spi_reg, #1 wc          ' Bit 6
                        muxc    outa, si_mask         
                        rcl     spi_reg, #1 wc          ' Bit 5
                        muxc    outa, si_mask         
                        rcl     spi_reg, #1 wc          ' Bit 4
                        muxc    outa, si_mask         
                        rcl     spi_reg, #1 wc          ' Bit 3
                        muxc    outa, si_mask         
                        rcl     spi_reg, #1 wc          ' Bit 2
                        muxc    outa, si_mask         
                        rcl     spi_reg, #1 wc          ' Bit 1
                        muxc    outa, si_mask         
                        rcl     spi_reg, #1 wc          ' Bit 0
                        muxc    outa, si_mask
                        nop                             ' Finish last clock cycle
                        mov     frqa, #0                ' Turn off clock
                        andn    outa, si_mask           ' Turn off SI
spi_Write8_ret          ret

                        '
                        ' spi_Write32 --
                        '
                        '   Write entire 32-bit contents of spi_reg to the SPI port.
                        ' 
spi_Write32             rcl     spi_reg, #1 wc          ' Shift bit 31
                        mov     phsa, #0                ' Rising edge at center of each bit
                        muxc    outa, si_mask           ' Output bit 31
                        mov     frqa, frq_8clk          ' First clock edge, period of 8 clock cycles
                        rcl     spi_reg, #1 wc          ' Bit 30
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 29
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 28
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 27
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 26
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 25
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 24
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 23
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 22
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 21
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 20
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 19
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 18
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 17
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 16
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 15
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 14
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 13
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 12
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 11
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 10
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 9
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 8
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 7
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 6
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 5
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 4
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 3
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 2
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 1
                        muxc    outa, si_mask
                        rcl     spi_reg, #1 wc          ' Bit 0
                        muxc    outa, si_mask
                        nop                             ' Finish last clock cycle
                        mov     frqa, #0                ' Turn off clock
                        andn    outa, si_mask           ' Turn off SI
spi_Write32_ret         ret

                        '
                        ' spi_Read8 --
                        '
                        '   Read 8 bits from the SPI port, and return them in the lower 8
                        '   bits of spi_reg.
                        ' 
spi_Read8               mov     spi_reg, #0             ' Clear unused bits
                        mov     phsa, #0                ' Rising edge at center of each bit
                        mov     frqa, frq_8clk          ' First clock edge, period of 8 clock cycles
                        test    so_mask, ina wc         ' Bit 7
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 6
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 5
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 4
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 3
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 2
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 1
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Sample bit 0
                        mov     frqa, #0                ' Turn off clock
                        rcl     spi_reg, #1             ' Store bit 0
spi_Read8_ret           ret

                        '
                        ' spi_Read32 --
                        '
                        '   Read 32 bits from the SPI port, and return them in the
                        '   entirety of spi_reg.   
                        ' 
spi_Read32              mov     phsa, #0                ' Rising edge at center of each bit
                        mov     frqa, frq_8clk          ' First clock edge, period of 8 clock cycles
                        test    so_mask, ina wc         ' Bit 31
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 30
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 29
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 28
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 27
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 26
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 25
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 24
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 23
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 22
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 21
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 20
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 19
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 18
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 17
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 16
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 15
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 14
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 13
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 12
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 11
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 10
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 9
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 8
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 7
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 6
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 5
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 4
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 3
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 2
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Bit 1
                        rcl     spi_reg, #1
                        test    so_mask, ina wc         ' Sample bit 0
                        mov     frqa, #0                ' Turn off clock
                        rcl     spi_reg, #1             ' Store bit 0
spi_Read32_ret          ret


'------------------------------------------------------------------------------
' Initialized Data
'------------------------------------------------------------------------------

hFFFFFFFF               long    $FFFFFFFF
h80000000               long    $80000000

init_dira               long    0
init_ctra               long    0

frq_8clk                long    1 << (32 - 3)   ' 1/8th clock rate

cs_mask                 long    0
si_mask                 long    0
so_mask                 long    0
int_mask                long    0

reg_ECON1               long    0

'------------------------------------------------------------------------------
' Uninitialized Data
'------------------------------------------------------------------------------

r0                      res     1
r1                      res     1
r2                      res     1
r3                      res     1

reg_name                res     1
reg_data                res     1
phy_name                res     1
phy_data                res     1
spi_reg                 res     1

                        fit

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