pcode dump


	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_sendPacket	;Function start
; 2 exit points
;#CSRC	main.c 272
;  putc(0x80);
	MOVLW	0x80
	CALL	_putc
;#CSRC	main.c 274
;  h = reading0 >> 8;
	MOVF	(_reading0 + 1),W
	MOVWF	r0x6F
	CLRF	r0x70
	BTFSC	_reading0,7
	DECF	r0x70,F
;;     peep 11 - Removed redundant move
	MOVF	r0x6F,W
;#CSRC	main.c 275
;  l = reading0;
	MOVF	_reading0,W
	MOVWF	r0x71
;#CSRC	main.c 276
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 277
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 278
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	MOVWF	r0x72
;#CSRC	main.c 280
;  h = reading1 >> 8;
	MOVF	(_reading1 + 1),W
	MOVWF	r0x73
	CLRF	r0x74
	BTFSC	_reading1,7
	DECF	r0x74,F
	MOVF	r0x73,W
	MOVWF	r0x6F
;#CSRC	main.c 281
;  l = reading1;
	MOVF	_reading1,W
	MOVWF	r0x71
;#CSRC	main.c 282
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 283
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 284
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 286
;  h = reading2 >> 8;
	MOVF	(_reading2 + 1),W
	MOVWF	r0x75
	CLRF	r0x76
	BTFSC	_reading2,7
	DECF	r0x76,F
	MOVF	r0x75,W
	MOVWF	r0x6F
;#CSRC	main.c 287
;  l = reading2;
	MOVF	_reading2,W
	MOVWF	r0x71
;#CSRC	main.c 288
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 289
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 290
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 292
;  h = reading3 >> 8;
	MOVF	(_reading3 + 1),W
	MOVWF	r0x77
	CLRF	r0x78
	BTFSC	_reading3,7
	DECF	r0x78,F
	MOVF	r0x77,W
	MOVWF	r0x6F
;#CSRC	main.c 293
;  l = reading3;
	MOVF	_reading3,W
	MOVWF	r0x71
;#CSRC	main.c 294
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 295
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 296
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 298
;  h = reading4 >> 8;
	MOVF	(_reading4 + 1),W
	MOVWF	r0x79
	CLRF	r0x7A
	BTFSC	_reading4,7
	DECF	r0x7A,F
	MOVF	r0x79,W
	MOVWF	r0x6F
;#CSRC	main.c 299
;  l = reading4;
	MOVF	_reading4,W
	MOVWF	r0x71
;#CSRC	main.c 300
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 301
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 302
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 304
;  h = reading5 >> 8;
	MOVF	(_reading5 + 1),W
	MOVWF	r0x7B
	CLRF	r0x7C
	BTFSC	_reading5,7
	DECF	r0x7C,F
	MOVF	r0x7B,W
	MOVWF	r0x6F
;#CSRC	main.c 305
;  l = reading5;
	MOVF	_reading5,W
	MOVWF	r0x71
;#CSRC	main.c 306
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 307
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 308
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 310
;  h = reading6 >> 8;
	MOVF	(_reading6 + 1),W
	MOVWF	r0x7D
	CLRF	r0x7E
	BTFSC	_reading6,7
	DECF	r0x7E,F
	MOVF	r0x7D,W
	MOVWF	r0x6F
;#CSRC	main.c 311
;  l = reading6;
	MOVF	_reading6,W
	MOVWF	r0x71
;#CSRC	main.c 312
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 313
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 314
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 316
;  h = reading7 >> 8;
	MOVF	(_reading7 + 1),W
	MOVWF	r0x7F
	CLRF	r0x80
	BTFSC	_reading7,7
	DECF	r0x80,F
	MOVF	r0x7F,W
	MOVWF	r0x6F
;#CSRC	main.c 317
;  l = reading7;
	MOVF	_reading7,W
	MOVWF	r0x71
;#CSRC	main.c 318
;  putc(h);
	MOVF	r0x6F,W
	CALL	_putc
;#CSRC	main.c 319
;  putc(l);
	MOVF	r0x71,W
	CALL	_putc
;#CSRC	main.c 320
;  checksum += h + l;
	MOVF	r0x71,W
	ADDWF	r0x6F,W
	ADDWF	r0x72,F
;#CSRC	main.c 323
;  putc(checksum);
	MOVF	r0x72,W
	CALL	_putc
	RETURN	
; exit point of _sendPacket

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_putc	;Function start
; 2 exit points
;#CSRC	main.c 241
;  void putc(unsigned char byte) {
	MOVWF	r0x6D
;#CSRC	main.c 245
;  SerialPort = 0;
	BCF	(_SerialPort >> 3), (_SerialPort & 7)
;#CSRC	main.c 246
;  SerialTris = 0;
	BCF	(_SerialTris >> 3), (_SerialTris & 7)
;#CSRC	main.c 247
;  serialBitDelay();
	CALL	_serialBitDelay
;#CSRC	main.c 251
;  do {
	MOVLW	0x08
	MOVWF	r0x6E
_00175_DS_
;#CSRC	main.c 252
;  if (byte & 1)
	BTFSS	r0x6D,0
	GOTO	_00173_DS_
;#CSRC	main.c 253
;  SerialPort = 1;
	BSF	(_SerialPort >> 3), (_SerialPort & 7)
	GOTO	_00174_DS_
_00173_DS_
;#CSRC	main.c 255
;  SerialPort = 0;
	BCF	(_SerialPort >> 3), (_SerialPort & 7)
_00174_DS_
;#CSRC	main.c 256
;  byte >>= 1;
	BCF	_STATUS,0
	RRF	r0x6D,F
;#CSRC	main.c 257
;  serialBitDelay();
	CALL	_serialBitDelay
;#CSRC	main.c 258
;  } while (--bitnum);
	DECFSZ	r0x6E,F
	GOTO	_00175_DS_
;#CSRC	main.c 261
;  SerialPort = 1;
	BSF	(_SerialPort >> 3), (_SerialPort & 7)
;#CSRC	main.c 262
;  serialBitDelay();
	CALL	_serialBitDelay
;#CSRC	main.c 263
;  SerialTris = 1;
	BSF	(_SerialTris >> 3), (_SerialTris & 7)
	RETURN	
; exit point of _putc

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_serialBitDelay	;Function start
; 2 exit points
;#CSRC	main.c 234
;  do {
	MOVLW	0x40
	MOVWF	r0x6C
_00167_DS_
;#CSRC	main.c 237
;  _endasm;
	NOP	
;#CSRC	main.c 238
;  } while (--i);
	DECFSZ	r0x6C,F
	GOTO	_00167_DS_
	RETURN	
; exit point of _serialBitDelay

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_sensorSample	;Function start
; 2 exit points
;#CSRC	main.c 165
;  int sensorSample(unsigned char transmitter, unsigned char receiver, unsigned char phase) {
	MOVWF	r0x63
	MOVF	s0xFFF,W
	MOVWF	r0x64
	MOVF	s0x1000,W
	MOVWF	r0x65
;#CSRC	main.c 171
;  PORTB &= 0x0F;               /* Disable all transmitters */  
	MOVLW	0x0f
	ANDWF	_PORTB,F
;#CSRC	main.c 172
;  TRISB = (~transmitter) & 0xF0;
	MOVWF	r0x66
	MOVLW	0xf0
	ANDWF	r0x66,W
	MOVWF	_TRISB
;#CSRC	main.c 173
;  ADCON0 = receiver;           /* Switch the ADC to this channel */
	MOVF	r0x64,W
	MOVWF	_ADCON0
;#CSRC	main.c 176
;  if (phase == 0) phase0 = 1; else phase0 = 0;
	MOVF	r0x65,W
	BTFSS	_STATUS,2
	GOTO	_00131_DS_
	BSF	(_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
	GOTO	_00132_DS_
_00131_DS_
	BCF	(_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
_00132_DS_
;#CSRC	main.c 177
;  if (phase == 1) phase1 = 1; else phase1 = 0;
	DECFSZ	r0x65,W
	GOTO	_00134_DS_
	BSF	(_sensorSample_phase1_1_1 >> 3), (_sensorSample_phase1_1_1 & 7)
	GOTO	_00135_DS_
_00134_DS_
	BCF	(_sensorSample_phase1_1_1 >> 3), (_sensorSample_phase1_1_1 & 7)
_00135_DS_
;#CSRC	main.c 178
;  if (phase == 2) phase2 = 1; else phase2 = 0;
	MOVF	r0x65,W
	XORLW	0x02
	BTFSS	_STATUS,2
	GOTO	_00137_DS_
	BSF	(_sensorSample_phase2_1_1 >> 3), (_sensorSample_phase2_1_1 & 7)
	GOTO	_00138_DS_
_00137_DS_
	BCF	(_sensorSample_phase2_1_1 >> 3), (_sensorSample_phase2_1_1 & 7)
_00138_DS_
;#CSRC	main.c 179
;  if (phase == 3) phase3 = 1; else phase3 = 0;
	MOVF	r0x65,W
	XORLW	0x03
	BTFSS	_STATUS,2
	GOTO	_00140_DS_
	BSF	(_sensorSample_phase3_1_1 >> 3), (_sensorSample_phase3_1_1 & 7)
	GOTO	_00162_DS_
_00140_DS_
	BCF	(_sensorSample_phase3_1_1 >> 3), (_sensorSample_phase3_1_1 & 7)
_00162_DS_
;#CSRC	main.c 186
;  do {
	MOVLW	0x03
	MOVWF	r0x67
_00142_DS_
;#CSRC	main.c 187
;  PORTB ^= transmitter;
	MOVF	r0x63,W
	XORWF	_PORTB,F
;#CSRC	main.c 188
;  NOP NOP NOP;
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 189
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 190
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 191
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 192
;  NOP NOP;
	NOP	
	NOP	
;#CSRC	main.c 193
;  } while (--halfcycles);
	DECFSZ	r0x67,F
;;     peep 1 - test/jump to test/skip
	GOTO	_00142_DS_
	BTFSC	(_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
	BSF	(_GO_DONE >> 3), (_GO_DONE & 7)
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 199
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
;;     peep 1 - test/jump to test/skip
	NOP	
	BTFSC	(_sensorSample_phase1_1_1 >> 3), (_sensorSample_phase1_1_1 & 7)
	BSF	(_GO_DONE >> 3), (_GO_DONE & 7)
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 203
;  NOP NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;;     peep 1 - test/jump to test/skip
	NOP	
	BTFSC	(_sensorSample_phase2_1_1 >> 3), (_sensorSample_phase2_1_1 & 7)
	BSF	(_GO_DONE >> 3), (_GO_DONE & 7)
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 207
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
;;     peep 1 - test/jump to test/skip
	NOP	
	BTFSC	(_sensorSample_phase3_1_1 >> 3), (_sensorSample_phase3_1_1 & 7)
	BSF	(_GO_DONE >> 3), (_GO_DONE & 7)
_00153_DS_
	BTFSC	(_NOT_DONE >> 3), (_NOT_DONE & 7)
	GOTO	_00153_DS_
;#CSRC	main.c 215
;  TRISB = 0xF0;
	MOVLW	0xf0
	MOVWF	_TRISB
;#CSRC	main.c 217
;  h = ADRESH;
	MOVF	_ADRESH,W
	MOVWF	r0x68
;#CSRC	main.c 218
;  l = ADRESL;
	MOVF	_ADRESL,W
	MOVWF	r0x69
;;     peep 11 - Removed redundant move
	MOVF	r0x68,W
	CLRF	r0x6A
	MOVF	r0x68,W
	MOVWF	r0x6A
	CLRF	r0x68
;;     peep 11 - Removed redundant move
	MOVF	r0x69,W
	CLRF	r0x6B
	MOVF	r0x69,W
	IORWF	r0x68,F
	MOVF	r0x6B,W
	IORWF	r0x6A,F
	MOVF	r0x68,W
	MOVWF	s0xFFF
	MOVF	r0x6A,W
	RETURN	
; exit point of _sensorSample

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_integrateSample	;Function start
; 2 exit points
;#CSRC	main.c 146
;  int integrateSample(unsigned char transmitter, unsigned char receiver) {
	MOVWF	r0x53
	MOVF	s0xFFF,W
	MOVWF	r0x54
;#CSRC	main.c 147
;  signed int i=0, q=0;
	CLRF	r0x55
	CLRF	r0x56
	CLRF	r0x57
	CLRF	r0x58
;#CSRC	main.c 150
;  do {
	MOVLW	0x08
	MOVWF	r0x59
	CLRF	r0x5A
_00119_DS_
;#CSRC	main.c 152
;  i += sensorSample(transmitter, receiver, 0);
	MOVLW	0x00
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x5C
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	MOVWF	r0x5B
	ADDWF	r0x55,F
	MOVF	r0x5C,W
	BTFSC	_STATUS,0
	INCFSZ	r0x5C,W
	ADDWF	r0x56,F
;#CSRC	main.c 153
;  q += sensorSample(transmitter, receiver, 1);
	MOVLW	0x01
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x5E
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	MOVWF	r0x5D
	ADDWF	r0x57,F
	MOVF	r0x5E,W
	BTFSC	_STATUS,0
	INCFSZ	r0x5E,W
	ADDWF	r0x58,F
;#CSRC	main.c 154
;  i -= sensorSample(transmitter, receiver, 2);
	MOVLW	0x02
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x60
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	MOVWF	r0x5F
	SUBWF	r0x55,F
	MOVF	r0x60,W
	BTFSS	_STATUS,0
	INCFSZ	r0x60,W
	SUBWF	r0x56,F
;#CSRC	main.c 155
;  q -= sensorSample(transmitter, receiver, 3);
	MOVLW	0x03
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x62
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	MOVWF	r0x61
	SUBWF	r0x57,F
	MOVF	r0x62,W
	BTFSS	_STATUS,0
	INCFSZ	r0x62,W
	SUBWF	r0x58,F
;#CSRC	main.c 156
;  } while (--numsamples);
	MOVLW	0xff
	ADDWF	r0x59,F
	BTFSS	_STATUS,0
	DECF	r0x5A,F
	MOVF	r0x59,W
	IORWF	r0x5A,W
	BTFSS	_STATUS,2
	GOTO	_00119_DS_
;#CSRC	main.c 159
;  if (i<0) i = -i;
	MOVF	r0x56,W
	ADDLW	0x80
	ADDLW	0x80
	BTFSC	_STATUS,0
	GOTO	_00123_DS_
	COMF	r0x55,F
	COMF	r0x56,F
	INCF	r0x55,F
	BTFSC	_STATUS,2
	INCF	r0x56,F
_00123_DS_
;#CSRC	main.c 160
;  if (q<0) q = -q;
	MOVF	r0x58,W
	ADDLW	0x80
	ADDLW	0x80
	BTFSC	_STATUS,0
	GOTO	_00125_DS_
	COMF	r0x57,F
	COMF	r0x58,F
	INCF	r0x57,F
	BTFSC	_STATUS,2
	INCF	r0x58,F
_00125_DS_
;#CSRC	main.c 161
;  return i+q;
	MOVF	r0x57,W
	ADDWF	r0x55,F
	MOVF	r0x58,W
	BTFSC	_STATUS,0
	INCFSZ	r0x58,W
	ADDWF	r0x56,F
	MOVF	r0x55,W
	MOVWF	s0xFFF
	MOVF	r0x56,W
	RETURN	
; exit point of _integrateSample

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_sampleAll	;Function start
; 2 exit points
;#CSRC	main.c 135
;  reading0 = integrateSample(Transmit0, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x10
	CALL	_integrateSample
	MOVWF	(_reading0 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading0
;#CSRC	main.c 136
;  reading1 = integrateSample(Transmit1, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x20
	CALL	_integrateSample
	MOVWF	(_reading1 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading1
;#CSRC	main.c 137
;  reading2 = integrateSample(Transmit2, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x40
	CALL	_integrateSample
	MOVWF	(_reading2 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading2
;#CSRC	main.c 138
;  reading3 = integrateSample(Transmit3, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x80
	CALL	_integrateSample
	MOVWF	(_reading3 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading3
;#CSRC	main.c 139
;  reading4 = integrateSample(Transmit0, Receive1);
	MOVLW	0x89
	MOVWF	s0xFFF
	MOVLW	0x10
	CALL	_integrateSample
	MOVWF	(_reading4 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading4
;#CSRC	main.c 140
;  reading5 = integrateSample(Transmit1, Receive1);
	MOVLW	0x89
	MOVWF	s0xFFF
	MOVLW	0x20
	CALL	_integrateSample
	MOVWF	(_reading5 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading5
;#CSRC	main.c 141
;  reading6 = integrateSample(Transmit2, Receive1);
	MOVLW	0x89
	MOVWF	s0xFFF
	MOVLW	0x40
	CALL	_integrateSample
	MOVWF	(_reading6 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading6
;#CSRC	main.c 142
;  reading7 = integrateSample(Transmit3, Receive1);
	MOVLW	0x89
	MOVWF	s0xFFF
	MOVLW	0x80
	CALL	_integrateSample
	MOVWF	(_reading7 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading7
	RETURN	
; exit point of _sampleAll

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_initialize	;Function start
; 2 exit points
;#CSRC	main.c 111
;  TRISA = 0xFF;
	MOVLW	0xff
	MOVWF	_TRISA
;#CSRC	main.c 112
;  TRISB = 0x00;
	CLRF	_TRISB
;#CSRC	main.c 113
;  TRISC = 0x00;
	CLRF	_TRISC
;#CSRC	main.c 114
;  TRISD = 0x00;
	CLRF	_TRISD
;#CSRC	main.c 115
;  TRISE = 0x04;
	MOVLW	0x04
	MOVWF	_TRISE
;#CSRC	main.c 118
;  PORTA = 0x00;
	CLRF	_PORTA
;#CSRC	main.c 119
;  PORTB = 0x00;
	CLRF	_PORTB
;#CSRC	main.c 120
;  PORTC = 0x00;
	CLRF	_PORTC
;#CSRC	main.c 121
;  PORTD = 0x00;
	CLRF	_PORTD
;#CSRC	main.c 122
;  PORTE = 0x00;
	CLRF	_PORTE
;#CSRC	main.c 125
;  ADCON1 = 0x82;
	MOVLW	0x82
	MOVWF	_ADCON1
;#CSRC	main.c 126
;  ADCON0 = 0x81;
	MOVLW	0x81
	MOVWF	_ADCON0
;#CSRC	main.c 127
;  GIE = 1;
	BSF	(_GIE >> 3), (_GIE & 7)
	RETURN	
; exit point of _initialize

	New pBlock

internal pblock, dbName =M
;; Starting pCode block
_main	;Function start
; 2 exit points
;#CSRC	main.c 95
;  NOP NOP NOP NOP   /* Padding for the bootloader */
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 97
;  initialize();
	CALL	_initialize
;#CSRC	main.c 99
;  Light0 = 1;
	BSF	(_Light0 >> 3), (_Light0 & 7)
_00106_DS_
;#CSRC	main.c 102
;  sampleAll();
	CALL	_sampleAll
;#CSRC	main.c 103
;  sendPacket();
	CALL	_sendPacket
	GOTO	_00106_DS_
	RETURN	
; exit point of _main
