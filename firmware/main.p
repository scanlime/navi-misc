pcode dump


	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_sendPacket	;Function start
; 2 exit points
;#CSRC	main.c 331
;  putc(0x80);
	MOVLW	0x80
	CALL	_putc
;#CSRC	main.c 333
;  h = reading0 >> 8;
	MOVF	(_reading0 + 1),W
	MOVWF	r0x72
	CLRF	r0x73
	BTFSC	_reading0,7
	DECF	r0x73,F
;;     peep 11 - Removed redundant move
	MOVF	r0x72,W
;#CSRC	main.c 334
;  l = reading0;
	MOVF	_reading0,W
	MOVWF	r0x74
;#CSRC	main.c 335
;  putc(h);
	MOVF	r0x72,W
	CALL	_putc
;#CSRC	main.c 336
;  putc(l);
	MOVF	r0x74,W
	CALL	_putc
;#CSRC	main.c 337
;  checksum += h + l;
	MOVF	r0x74,W
	ADDWF	r0x72,W
	MOVWF	r0x75
;#CSRC	main.c 339
;  h = reading1 >> 8;
	MOVF	(_reading1 + 1),W
	MOVWF	r0x76
	CLRF	r0x77
	BTFSC	_reading1,7
	DECF	r0x77,F
	MOVF	r0x76,W
	MOVWF	r0x72
;#CSRC	main.c 340
;  l = reading1;
	MOVF	_reading1,W
	MOVWF	r0x74
;#CSRC	main.c 341
;  putc(h);
	MOVF	r0x72,W
	CALL	_putc
;#CSRC	main.c 342
;  putc(l);
	MOVF	r0x74,W
	CALL	_putc
;#CSRC	main.c 343
;  checksum += h + l;
	MOVF	r0x74,W
	ADDWF	r0x72,W
	ADDWF	r0x75,F
;#CSRC	main.c 345
;  h = reading2 >> 8;
	MOVF	(_reading2 + 1),W
	MOVWF	r0x78
	CLRF	r0x79
	BTFSC	_reading2,7
	DECF	r0x79,F
	MOVF	r0x78,W
	MOVWF	r0x72
;#CSRC	main.c 346
;  l = reading2;
	MOVF	_reading2,W
	MOVWF	r0x74
;#CSRC	main.c 347
;  putc(h);
	MOVF	r0x72,W
	CALL	_putc
;#CSRC	main.c 348
;  putc(l);
	MOVF	r0x74,W
	CALL	_putc
;#CSRC	main.c 349
;  checksum += h + l;
	MOVF	r0x74,W
	ADDWF	r0x72,W
	ADDWF	r0x75,F
;#CSRC	main.c 351
;  h = reading3 >> 8;
	MOVF	(_reading3 + 1),W
	MOVWF	r0x7A
	CLRF	r0x7B
	BTFSC	_reading3,7
	DECF	r0x7B,F
	MOVF	r0x7A,W
	MOVWF	r0x72
;#CSRC	main.c 352
;  l = reading3;
	MOVF	_reading3,W
	MOVWF	r0x74
;#CSRC	main.c 353
;  putc(h);
	MOVF	r0x72,W
	CALL	_putc
;#CSRC	main.c 354
;  putc(l);
	MOVF	r0x74,W
	CALL	_putc
;#CSRC	main.c 355
;  checksum += h + l;
	MOVF	r0x74,W
	ADDWF	r0x72,W
	ADDWF	r0x75,F
;#CSRC	main.c 357
;  h = reading4 >> 8;
	MOVF	(_reading4 + 1),W
	MOVWF	r0x7C
	CLRF	r0x7D
	BTFSC	_reading4,7
	DECF	r0x7D,F
	MOVF	r0x7C,W
	MOVWF	r0x72
;#CSRC	main.c 358
;  l = reading4;
	MOVF	_reading4,W
	MOVWF	r0x74
;#CSRC	main.c 359
;  putc(h);
	MOVF	r0x72,W
	CALL	_putc
;#CSRC	main.c 360
;  putc(l);
	MOVF	r0x74,W
	CALL	_putc
;#CSRC	main.c 361
;  checksum += h + l;
	MOVF	r0x74,W
	ADDWF	r0x72,W
	ADDWF	r0x75,F
;#CSRC	main.c 363
;  h = reading5 >> 8;
	MOVF	(_reading5 + 1),W
	MOVWF	r0x7E
	CLRF	r0x7F
	BTFSC	_reading5,7
	DECF	r0x7F,F
	MOVF	r0x7E,W
	MOVWF	r0x72
;#CSRC	main.c 364
;  l = reading5;
	MOVF	_reading5,W
	MOVWF	r0x74
;#CSRC	main.c 365
;  putc(h);
	MOVF	r0x72,W
	CALL	_putc
;#CSRC	main.c 366
;  putc(l);
	MOVF	r0x74,W
	CALL	_putc
;#CSRC	main.c 367
;  checksum += h + l;
	MOVF	r0x74,W
	ADDWF	r0x72,W
	ADDWF	r0x75,F
;#CSRC	main.c 369
;  h = reading6 >> 8;
	MOVF	(_reading6 + 1),W
	MOVWF	r0x80
	CLRF	r0x81
	BTFSC	_reading6,7
	DECF	r0x81,F
	MOVF	r0x80,W
	MOVWF	r0x72
;#CSRC	main.c 370
;  l = reading6;
	MOVF	_reading6,W
	MOVWF	r0x74
;#CSRC	main.c 371
;  putc(h);
	MOVF	r0x72,W
	CALL	_putc
;#CSRC	main.c 372
;  putc(l);
	MOVF	r0x74,W
	CALL	_putc
;#CSRC	main.c 373
;  checksum += h + l;
	MOVF	r0x74,W
	ADDWF	r0x72,W
	ADDWF	r0x75,F
;#CSRC	main.c 375
;  h = reading7 >> 8;
	MOVF	(_reading7 + 1),W
	MOVWF	r0x82
	CLRF	r0x83
	BTFSC	_reading7,7
	DECF	r0x83,F
	MOVF	r0x82,W
	MOVWF	r0x72
;#CSRC	main.c 376
;  l = reading7;
	MOVF	_reading7,W
	MOVWF	r0x74
;#CSRC	main.c 377
;  putc(h);
	MOVF	r0x72,W
	CALL	_putc
;#CSRC	main.c 378
;  putc(l);
	MOVF	r0x74,W
	CALL	_putc
;#CSRC	main.c 379
;  checksum += h + l;
	MOVF	r0x74,W
	ADDWF	r0x72,W
	ADDWF	r0x75,F
;#CSRC	main.c 382
;  putc(checksum);
	MOVF	r0x75,W
	CALL	_putc
	RETURN	
; exit point of _sendPacket

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_getc	;Function start
; 2 exit points
;#CSRC	main.c 294
;  SerialPort = 1;
	BSF	(_SerialPort >> 3), (_SerialPort & 7)
;#CSRC	main.c 295
;  SerialTris = 1;
	BSF	(_SerialTris >> 3), (_SerialTris & 7)
;#CSRC	main.c 296
;  NOP;
	NOP	
_00176_DS_
;#CSRC	main.c 300
;  while (SerialPort);
	GOTO	_00176_DS_
;#CSRC	main.c 322
;  return byte;
	RETURN	
; exit point of _getc

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_putc	;Function start
; 2 exit points
;#CSRC	main.c 265
;  void putc(unsigned char byte) {
	MOVWF	r0x70
;#CSRC	main.c 269
;  SerialPort = 0;
	BCF	(_SerialPort >> 3), (_SerialPort & 7)
;#CSRC	main.c 270
;  SerialTris = 0;
	BCF	(_SerialTris >> 3), (_SerialTris & 7)
;#CSRC	main.c 271
;  serialBitDelay();
	CALL	_serialBitDelay
;#CSRC	main.c 275
;  do {
	MOVLW	0x08
	MOVWF	r0x71
_00171_DS_
;#CSRC	main.c 276
;  if (byte & 1)
	BTFSS	r0x70,0
	GOTO	_00169_DS_
;#CSRC	main.c 277
;  SerialPort = 1;
	BSF	(_SerialPort >> 3), (_SerialPort & 7)
	GOTO	_00170_DS_
_00169_DS_
;#CSRC	main.c 279
;  SerialPort = 0;
	BCF	(_SerialPort >> 3), (_SerialPort & 7)
_00170_DS_
;#CSRC	main.c 280
;  byte >>= 1;
	BCF	_STATUS,0
	RRF	r0x70,F
;#CSRC	main.c 281
;  serialBitDelay();
	CALL	_serialBitDelay
;#CSRC	main.c 282
;  } while (--bitnum);
	DECFSZ	r0x71,F
	GOTO	_00171_DS_
;#CSRC	main.c 285
;  SerialPort = 1;
	BSF	(_SerialPort >> 3), (_SerialPort & 7)
;#CSRC	main.c 286
;  serialBitDelay();
	CALL	_serialBitDelay
;#CSRC	main.c 287
;  SerialTris = 1;
	BSF	(_SerialTris >> 3), (_SerialTris & 7)
	RETURN	
; exit point of _putc

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_halfSerialBitDelay	;Function start
; 2 exit points
;#CSRC	main.c 260
;  do {
	MOVLW	0x20
	MOVWF	r0x6F
_00163_DS_
;#CSRC	main.c 261
;  NOP;
	NOP	
;#CSRC	main.c 262
;  } while (--i);
	DECFSZ	r0x6F,F
	GOTO	_00163_DS_
	RETURN	
; exit point of _halfSerialBitDelay

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_serialBitDelay	;Function start
; 2 exit points
;#CSRC	main.c 253
;  do {
	MOVLW	0x40
	MOVWF	r0x6E
_00158_DS_
;#CSRC	main.c 254
;  NOP;
	NOP	
;#CSRC	main.c 255
;  } while (--i);
	DECFSZ	r0x6E,F
	GOTO	_00158_DS_
	RETURN	
; exit point of _serialBitDelay

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_sensorSample	;Function start
; 2 exit points
;#CSRC	main.c 182
;  int sensorSample(unsigned char transmitter, unsigned char receiver, unsigned char phase) {
	MOVWF	r0x65
	MOVF	s0xFFF,W
	MOVWF	r0x66
	MOVF	s0x1000,W
	MOVWF	r0x67
;#CSRC	main.c 188
;  PORTB &= 0x0F;            /* Disable all transmitters */  
	MOVLW	0x0f
	ANDWF	_PORTB,F
;#CSRC	main.c 189
;  TRISB = 0x00;             /* I -think- we want the other plates to be ground planes now,
	CLRF	_TRISB
;#CSRC	main.c 192
;  ADCON0 = receiver;        /* Switch the ADC to this channel */
	MOVF	r0x66,W
	MOVWF	_ADCON0
;#CSRC	main.c 195
;  if (phase == 0) phase0 = 1; else phase0 = 0;
	MOVF	r0x67,W
	BTFSS	_STATUS,2
	GOTO	_00131_DS_
	BSF	(_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
	GOTO	_00132_DS_
_00131_DS_
	BCF	(_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
_00132_DS_
;#CSRC	main.c 196
;  if (phase == 1) phase1 = 1; else phase1 = 0;
	DECFSZ	r0x67,W
	GOTO	_00134_DS_
	BSF	(_sensorSample_phase1_1_1 >> 3), (_sensorSample_phase1_1_1 & 7)
	GOTO	_00135_DS_
_00134_DS_
	BCF	(_sensorSample_phase1_1_1 >> 3), (_sensorSample_phase1_1_1 & 7)
_00135_DS_
;#CSRC	main.c 197
;  if (phase == 2) phase2 = 1; else phase2 = 0;
	MOVF	r0x67,W
	XORLW	0x02
	BTFSS	_STATUS,2
	GOTO	_00137_DS_
	BSF	(_sensorSample_phase2_1_1 >> 3), (_sensorSample_phase2_1_1 & 7)
	GOTO	_00138_DS_
_00137_DS_
	BCF	(_sensorSample_phase2_1_1 >> 3), (_sensorSample_phase2_1_1 & 7)
_00138_DS_
;#CSRC	main.c 198
;  if (phase == 3) phase3 = 1; else phase3 = 0;
	MOVF	r0x67,W
	XORLW	0x03
	BTFSS	_STATUS,2
	GOTO	_00140_DS_
	BSF	(_sensorSample_phase3_1_1 >> 3), (_sensorSample_phase3_1_1 & 7)
	GOTO	_00141_DS_
_00140_DS_
	BCF	(_sensorSample_phase3_1_1 >> 3), (_sensorSample_phase3_1_1 & 7)
_00141_DS_
;#CSRC	main.c 204
;  halfcycles = numResonationHalfcycles;
	MOVF	_numResonationHalfcycles,W
;;     peep 2 - Removed redundant move
	MOVWF	r0x68
	MOVWF	r0x69
_00142_DS_
;#CSRC	main.c 206
;  PORTB ^= transmitter;
	MOVF	r0x65,W
	XORWF	_PORTB,F
;#CSRC	main.c 207
;  NOP NOP NOP;
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 208
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 209
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 210
;  NOP NOP NOP NOP NOP;
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 211
;  NOP NOP;
	NOP	
	NOP	
;#CSRC	main.c 212
;  } while (--halfcycles);
	DECFSZ	r0x69,F
;;     peep 1 - test/jump to test/skip
	GOTO	_00142_DS_
	BTFSC	(_sensorSample_phase0_1_1 >> 3), (_sensorSample_phase0_1_1 & 7)
	BSF	(_GO_DONE >> 3), (_GO_DONE & 7)
	NOP	
	NOP	
	NOP	
	NOP	
	NOP	
;#CSRC	main.c 218
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
;#CSRC	main.c 222
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
;#CSRC	main.c 226
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
;#CSRC	main.c 234
;  TRISB = 0xF0;
	MOVLW	0xf0
	MOVWF	_TRISB
;#CSRC	main.c 236
;  h = ADRESH;
	MOVF	_ADRESH,W
	MOVWF	r0x6A
;#CSRC	main.c 237
;  l = ADRESL;
	MOVF	_ADRESL,W
	MOVWF	r0x6B
;;     peep 11 - Removed redundant move
	MOVF	r0x6A,W
	CLRF	r0x6C
	MOVF	r0x6A,W
	MOVWF	r0x6C
	CLRF	r0x6A
;;     peep 11 - Removed redundant move
	MOVF	r0x6B,W
	CLRF	r0x6D
	MOVF	r0x6B,W
	IORWF	r0x6A,F
	MOVF	r0x6D,W
	IORWF	r0x6C,F
	MOVF	r0x6A,W
	MOVWF	s0xFFF
	MOVF	r0x6C,W
	RETURN	
; exit point of _sensorSample

	New pBlock

CSEG    (CODE), dbName =C
;; Starting pCode block
_integrateSample	;Function start
; 2 exit points
;#CSRC	main.c 163
;  int integrateSample(unsigned char transmitter, unsigned char receiver) {
	MOVWF	r0x53
	MOVF	s0xFFF,W
	MOVWF	r0x54
;#CSRC	main.c 164
;  signed int i=0, q=0;
	CLRF	r0x55
	CLRF	r0x56
	CLRF	r0x57
	CLRF	r0x58
;#CSRC	main.c 165
;  int numsamples = numIntegrationSamples;
	MOVF	_numIntegrationSamples,W
	MOVWF	r0x59
	CLRF	r0x5A
;#CSRC	main.c 167
;  do {
	MOVF	r0x59,W
	MOVWF	r0x5B
	MOVF	r0x5A,W
	MOVWF	r0x5C
_00119_DS_
;#CSRC	main.c 169
;  i += sensorSample(transmitter, receiver, 0);
	MOVLW	0x00
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x5E
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	MOVWF	r0x5D
	ADDWF	r0x55,F
	MOVF	r0x5E,W
	BTFSC	_STATUS,0
	INCFSZ	r0x5E,W
	ADDWF	r0x56,F
;#CSRC	main.c 170
;  q += sensorSample(transmitter, receiver, 1);
	MOVLW	0x01
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x60
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	MOVWF	r0x5F
	ADDWF	r0x57,F
	MOVF	r0x60,W
	BTFSC	_STATUS,0
	INCFSZ	r0x60,W
	ADDWF	r0x58,F
;#CSRC	main.c 171
;  i -= sensorSample(transmitter, receiver, 2);
	MOVLW	0x02
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x62
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	MOVWF	r0x61
	SUBWF	r0x55,F
	MOVF	r0x62,W
	BTFSS	_STATUS,0
	INCFSZ	r0x62,W
	SUBWF	r0x56,F
;#CSRC	main.c 172
;  q -= sensorSample(transmitter, receiver, 3);
	MOVLW	0x03
	MOVWF	s0x1000
	MOVF	r0x54,W
	MOVWF	s0xFFF
	MOVF	r0x53,W
	CALL	_sensorSample
	MOVWF	r0x64
;;     peep 9c - Removed redundant move
	MOVF	s0xFFF,W
	MOVWF	r0x63
	SUBWF	r0x57,F
	MOVF	r0x64,W
	BTFSS	_STATUS,0
	INCFSZ	r0x64,W
	SUBWF	r0x58,F
;#CSRC	main.c 173
;  } while (--numsamples);
	MOVLW	0xff
	ADDWF	r0x5B,F
	BTFSS	_STATUS,0
	DECF	r0x5C,F
	MOVF	r0x5B,W
	IORWF	r0x5C,W
	BTFSS	_STATUS,2
	GOTO	_00119_DS_
;#CSRC	main.c 176
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
;#CSRC	main.c 177
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
;#CSRC	main.c 178
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
;#CSRC	main.c 152
;  reading0 = integrateSample(Transmit0, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x10
	CALL	_integrateSample
	MOVWF	(_reading0 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading0
;#CSRC	main.c 153
;  reading1 = integrateSample(Transmit1, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x20
	CALL	_integrateSample
	MOVWF	(_reading1 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading1
;#CSRC	main.c 154
;  reading2 = integrateSample(Transmit2, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x40
	CALL	_integrateSample
	MOVWF	(_reading2 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading2
;#CSRC	main.c 155
;  reading3 = integrateSample(Transmit3, Receive0);
	MOVLW	0x81
	MOVWF	s0xFFF
	MOVLW	0x80
	CALL	_integrateSample
	MOVWF	(_reading3 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading3
;#CSRC	main.c 156
;  reading4 = integrateSample(Transmit0, Receive1);
	MOVLW	0x89
	MOVWF	s0xFFF
	MOVLW	0x10
	CALL	_integrateSample
	MOVWF	(_reading4 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading4
;#CSRC	main.c 157
;  reading5 = integrateSample(Transmit1, Receive1);
	MOVLW	0x89
	MOVWF	s0xFFF
	MOVLW	0x20
	CALL	_integrateSample
	MOVWF	(_reading5 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading5
;#CSRC	main.c 158
;  reading6 = integrateSample(Transmit2, Receive1);
	MOVLW	0x89
	MOVWF	s0xFFF
	MOVLW	0x40
	CALL	_integrateSample
	MOVWF	(_reading6 + 1)
	MOVF	s0xFFF,W
	MOVWF	_reading6
;#CSRC	main.c 159
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
;#CSRC	main.c 128
;  TRISA = 0xFF;
	MOVLW	0xff
	MOVWF	_TRISA
;#CSRC	main.c 129
;  TRISB = 0x00;
	CLRF	_TRISB
;#CSRC	main.c 130
;  TRISC = 0x00;
	CLRF	_TRISC
;#CSRC	main.c 131
;  TRISD = 0x00;
	CLRF	_TRISD
;#CSRC	main.c 132
;  TRISE = 0x04;
	MOVLW	0x04
	MOVWF	_TRISE
;#CSRC	main.c 135
;  PORTA = 0x00;
	CLRF	_PORTA
;#CSRC	main.c 136
;  PORTB = 0x00;
	CLRF	_PORTB
;#CSRC	main.c 137
;  PORTC = 0x00;
	CLRF	_PORTC
;#CSRC	main.c 138
;  PORTD = 0x00;
	CLRF	_PORTD
;#CSRC	main.c 139
;  PORTE = 0x00;
	CLRF	_PORTE
;#CSRC	main.c 142
;  ADCON1 = 0x82;
	MOVLW	0x82
	MOVWF	_ADCON1
;#CSRC	main.c 143
;  ADCON0 = 0x81;
	MOVLW	0x81
	MOVWF	_ADCON0
;#CSRC	main.c 144
;  GIE = 1;
	BSF	(_GIE >> 3), (_GIE & 7)
	RETURN	
; exit point of _initialize

	New pBlock

internal pblock, dbName =M
;; Starting pCode block
_main	;Function start
; 2 exit points
;#CSRC	main.c 97
;  initialize();
	CALL	_initialize
;#CSRC	main.c 99
;  Light0 = 1;
	BSF	(_Light0 >> 3), (_Light0 & 7)
_00106_DS_
;#CSRC	main.c 119
;  sampleAll();
	CALL	_sampleAll
;#CSRC	main.c 120
;  sendPacket();
	CALL	_sendPacket
	GOTO	_00106_DS_
	RETURN	
; exit point of _main

	New pBlock

GSINIT  (CODE), dbName =D
;; Starting pCode block
;#CSRC	main.c 384
;  
	MOVLW	0x03
	MOVWF	_numResonationHalfcycles
;; Starting pCode block
;#CSRC	main.c 384
;  /* The End */

	MOVLW	0x08
	MOVWF	_numIntegrationSamples
