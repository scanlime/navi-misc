/*
	see:CARDME.H
*/

#include "cardme.h"

#define CARDME_CR1       (*(vuint16*)0x040001A0) /*fixed*/ 
#define CARDME_EEPDATA   (*(vuint16*)0x040001A2)	 /*fixed*/

#define EepromWaitBusy()   while (CARDME_CR1 & /*BUSY*/0x80); 

void cardmeReadEeprom(u32 address, u8 *data, u32 length, u32 addrtype);
void cardmeWriteEepromT1(u32 address, u8 *apData, u32 aLength, u32 addrtype);
void cardmeWriteEepromT2(u32 address, u8 *apData, u32 aLength, u32 addrtype);
void cardmeWriteEepromT3(u32 address, u8 *apData, u32 aLength, u32 addrtype);



void cardmeReadHeader(uint8 * header) {
//---------------------------------------------------------------------------------
	cardRead00(0, (uint32 *)header, 512, 0xA93F1FFF);
}


u8 cardmeCMD(u8 cmd,int address) 
{ 
//	int address=i & 1;
	u8 ret;
	int k;
    CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40; 

    CARDME_CR1 = 0xFFFF; 
    CARDME_EEPDATA = cmd;	// 

    EepromWaitBusy(); 

        CARDME_EEPDATA = (address >> 16) & 0xFF; 
        EepromWaitBusy(); 

        CARDME_EEPDATA = (address >> 8) & 0xFF; 
        EepromWaitBusy(); 

    CARDME_EEPDATA = (address) & 0xFF; 
    EepromWaitBusy(); 

	for(k=0;k<256;k++)
	{
        ret = CARDME_EEPDATA; 
        if(ret!=0xFF)break;
	    EepromWaitBusy(); 
	}
    CARDME_CR1 = /*MODE*/0x40; 
    return ret;
} 


u8 cardmeReadID(int i) 
{ 
	int address=i & 1;
	u8 ret;
	int k;
    CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40; 

    CARDME_CR1 = 0xFFFF; 
    CARDME_EEPDATA = 0xAB;	// READ ID

    EepromWaitBusy(); 

        CARDME_EEPDATA = (address >> 16) & 0xFF; 
        EepromWaitBusy(); 

        CARDME_EEPDATA = (address >> 8) & 0xFF; 
        EepromWaitBusy(); 

    CARDME_EEPDATA = (address) & 0xFF; 
    EepromWaitBusy(); 

	for(k=0;k<16;k++)
	{
        ret = CARDME_EEPDATA; 
        if(ret!=0xFF)break;
	    EepromWaitBusy(); 
	}
    CARDME_CR1 = /*MODE*/0x40; 
    return ret;
} 



int cardmeGetType(void)
{
//	u8 ret;
	u8 c00;
	u8 c05;
	u8 c9f;
	u8 c03;
	
		WAIT_CR &= ~0x0880;	// DS Card access ARM9:bit11=0   GBA Cart access ARM9:bit7=0
	c03=cardmeCMD(0x03,0);
	c05=cardmeCMD(0x05,0);
	c9f=cardmeCMD(0x9f,0);
	c00=cardmeCMD(0x00,0);
	if((c00==0x00) && (c9f==0x00))return 0;	//	PassMe? 判別不可
	if((c00==0xff) && (c05==0xff) && (c9f==0xff))return -1;

	if((c00==0xff) &&  (c05 & 0xFD) == 0xF0 && (c9f==0xff))return 1;
	if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0xff))return 2;
	if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x00))return 3;
	if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x12))return 3;	//	NEW TYPE 3
	if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x13))return 3;	//	NEW TYPE 3+  4Mbit
//	if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x14))return 3;	//	NEW TYPE 3++ 8Mbit MK4-FLASH Memory

	//	/* 判定不可 */
	return 0;
	
}

int cardmeSizeT3(void)
{
	u8 c9f;
	c9f=cardmeCMD(0x9f,0);

	if(c9f==0x14)	//	NEW TYPE 3++ 8Mbit
	{
		return 1024*1024;	//	 //	NEW TYPE 3++ 8Mbit(1024KByte)
	}
	
	if(c9f==0x13)	//	NEW TYPE 3+ 4Mbit(512MByte)
	{
		return 512*1024;	//	 NEW TYPE 3+ 4Mbit(512KByte)
	}
	
	return 256*1024;		//	TYPE 3  2Mbit(256KByte)


}

int cardmeSizeT2(void)
{
	int tp=2;
#define OFOT0 (8*1024-1)	//	8KBの末端
#define OFOT1 (2*8*1024-1)	//	16KBの末端
	u8 buf1[1];	//	+0k data	read -> write
	u8 buf2[1];	//	+8k data	read -> read
	u8 buf3[1];	//	+0k ~data	   write
	u8 buf4[1];	//	+8k data new	comp buf2
	cardmeReadEeprom(OFOT0,buf1,1,tp);
	cardmeReadEeprom(OFOT1,buf2,1,tp);
	buf3[0]=~buf1[0];
	cardmeWriteEeprom(OFOT0,buf3,1,tp);
	cardmeReadEeprom(OFOT1,buf4,1,tp);
	cardmeWriteEeprom(OFOT0,buf1,1,tp);
	if(buf4[0]!=buf2[0])	//	+8kも書き換わっている
	{
		return 8*1024;	//	 8KB(64kbit)
	}
	return 64*1024;		//	64KB(512kbit)


}

int cardmeSize(int tp)
{
	if(-1 <= tp && tp <= 3)
	{
		//            -1    0    1    2         3
//		int eepsz[5]={ 0,8192, 512,	8192,		256*1024};
		int eepsz[5]={ 0,8192, 512,	64*1024,	256*1024};
		if(tp==2)
		{
			return cardmeSizeT2();
		}
		if(tp==3)
		{
			return cardmeSizeT3();
		}
		tp++;
		return eepsz[tp];
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////
void cardmeReadEeprom(u32 address, u8 *data, u32 length, u32 addrtype) 
{ 
    CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40; 
//    CARDME_CR1 = 0xFFFF; 
    CARDME_EEPDATA = 0x03 | ((addrtype == 1) ? address>>8<<3 : 0);
    EepromWaitBusy(); 

    if (addrtype == 3) 
    { 
        CARDME_EEPDATA = (address >> 16) & 0xFF; 
        EepromWaitBusy(); 
    } 
    if (addrtype >= 2) 
    { 
        CARDME_EEPDATA = (address >> 8) & 0xFF; 
        EepromWaitBusy(); 
    } 
    CARDME_EEPDATA = (address) & 0xFF; 
    EepromWaitBusy(); 

    while (length > 0) 
    { 
        CARDME_EEPDATA = 0; 
        EepromWaitBusy(); 
        *data++ = CARDME_EEPDATA; 
        length--; 
    } 
        EepromWaitBusy(); 
    CARDME_CR1 = /*MODE*/0x40; 
} 
//////////////////////////////////////////////////////////////////////


void cardmeWriteEeprom(u32 address, u8 *apData, u32 aLength, u32 addrtype)
{

	switch(addrtype)
	{
		case 1:
				cardmeWriteEepromT1(address,apData,aLength,addrtype);
				return;
		case 2:
				cardmeWriteEepromT2(address,apData,aLength,addrtype);
				return;
		case 3:
				cardmeWriteEepromT3(address,apData,aLength,addrtype);
				return;
	}

}


#define T1BLOCKSZ (16)
//  TYPE1 4kbit EEPROM
void cardmeWriteEepromT1(u32 address, u8 *apData, u32 aLength, u32 addrtype)
{
	int i;
	u32 last=address+aLength;

	while (address < last)
	{


		// set WEL (Write Enable Latch)
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x06; 
        EepromWaitBusy();

		CARDME_CR1 = /*MODE*/0x40;

		// program maximum of 16 bytes
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		//	WRITE COMMAND 0x02 + A8 << 3
		CARDME_EEPDATA = 0x02 | (address & BIT(8)) >> (8-3) ; 
        EepromWaitBusy();
		CARDME_EEPDATA = address & 0xFF; 
        EepromWaitBusy();


		for (i = 0; address < last && i < T1BLOCKSZ; i++, address++) 
        { 
            CARDME_EEPDATA = *apData; 
            EepromWaitBusy(); 
            apData++;
        }

		CARDME_CR1 = /*MODE*/0x40;

		// wait programming to finish
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x05; 
        EepromWaitBusy();

		do 
        { 
            CARDME_EEPDATA = 0; 
            EepromWaitBusy(); 
        } while (CARDME_EEPDATA & 0x01);	// WIP (Write In Progress) ?
        EepromWaitBusy(); 
		CARDME_CR1 = /*MODE*/0x40;

	}

}



//  TYPE2 64kbit EEPROM
void cardmeWriteEepromT2(u32 address, u8 *apData, u32 aLength, u32 addrtype)
{
	int i;
	u32 last=address+aLength;

	while (address < last)
	{


		// set WEL (Write Enable Latch)
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x06; 
        EepromWaitBusy();

		CARDME_CR1 = /*MODE*/0x40;

		// program maximum of 32 bytes
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x02; 
        EepromWaitBusy();
        CARDME_EEPDATA = address >> 8; 
        EepromWaitBusy(); 
		CARDME_EEPDATA = address & 0xFF; 
        EepromWaitBusy();


		for (i = 0; address < last && i < 32; i++, address++) 
        { 
            CARDME_EEPDATA = *apData; 
            EepromWaitBusy(); 
            apData++;
        }

		CARDME_CR1 = /*MODE*/0x40;

		// wait programming to finish
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x05; 
        EepromWaitBusy();

		do 
        { 
            CARDME_EEPDATA = 0; 
            EepromWaitBusy(); 
        } while (CARDME_EEPDATA & 0x01);	// WIP (Write In Progress) ?
        EepromWaitBusy(); 
		CARDME_CR1 = /*MODE*/0x40;

	}

}


#define T3BLOCKSZ (256)
//  TYPE3 2Mbit FLASH MEMORY
void cardmeWriteEepromT3(u32 address, u8 *apData, u32 aLength, u32 addrtype)
{
	int i;
	u32 last=address+aLength;

	while (address < last)
	{


		// set WEL (Write Enable Latch)
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x06; 
        EepromWaitBusy();

		CARDME_CR1 = /*MODE*/0x40;

		// program maximum of 32 bytes
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x02; 
        EepromWaitBusy();
        CARDME_EEPDATA = (address >> 16) & 0xFF; 
        EepromWaitBusy(); 
        CARDME_EEPDATA = (address >> 8) & 0xFF; 
        EepromWaitBusy(); 
		CARDME_EEPDATA = address & 0xFF; 
        EepromWaitBusy();


		for (i = 0; address < last && i < T3BLOCKSZ; i++, address++) 
        { 
            CARDME_EEPDATA = *apData; 
            EepromWaitBusy(); 
            apData++;
        }

		CARDME_CR1 = /*MODE*/0x40;

		// wait programming to finish
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x05; 
        EepromWaitBusy();

		do 
        { 
            CARDME_EEPDATA = 0; 
            EepromWaitBusy(); 
        } while (CARDME_EEPDATA & 0x01);	// WIP (Write In Progress) ?
        EepromWaitBusy(); 
		CARDME_CR1 = /*MODE*/0x40;

	}

}



//	Chip Erase NEW : clear FLASH MEMORY (TYPE 3 ONLY)
int cardmeChipErase(void)
{
	int sz;
	void cardmeSectorErase(u32 address);
	sz= cardmeSizeT3();
	
	cardmeSectorErase(0x00000);
	cardmeSectorErase(0x10000);
	cardmeSectorErase(0x20000);
	cardmeSectorErase(0x30000);
	if(sz==512*1024)
	{
	cardmeSectorErase(0x40000);
	cardmeSectorErase(0x50000);
	cardmeSectorErase(0x60000);
	cardmeSectorErase(0x70000);
	}
	return 0;
}

//	COMMAND Sec.erase 0xD8 
void cardmeSectorErase(u32 address)
{
//	int i;

		// set WEL (Write Enable Latch)
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x06; 
        EepromWaitBusy();

		CARDME_CR1 = /*MODE*/0x40;

		// SectorErase 0xD8
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0xD8; 
        EepromWaitBusy();
        CARDME_EEPDATA = (address >> 16) & 0xFF; 
        EepromWaitBusy(); 
        CARDME_EEPDATA = (address >> 8) & 0xFF; 
        EepromWaitBusy(); 
		CARDME_EEPDATA = address & 0xFF; 
        EepromWaitBusy();

		CARDME_CR1 = /*MODE*/0x40;

		// wait erase to finish
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x05; 
        EepromWaitBusy();

		do 
        { 
            CARDME_EEPDATA = 0; 
            EepromWaitBusy(); 
        } while (CARDME_EEPDATA & 0x01);	// WIP (Write In Progress) ?
//        EepromWaitBusy(); 
		CARDME_CR1 = /*MODE*/0x40;

}


//	Chip Erase  : clear FLASH MEMORY (TYPE 3 ONLY)
//	COMMAND 0xC7
int cardmeChipErase_old(void)
{
	{


		// set WEL (Write Enable Latch)
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x06; 
        EepromWaitBusy();

		CARDME_CR1 = /*MODE*/0x40;

		// Chip Erase 0xC7
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0xC7; 
        EepromWaitBusy();

		CARDME_CR1 = /*MODE*/0x40;

		// wait programming to finish
		CARDME_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARDME_EEPDATA = 0x05; 
        EepromWaitBusy();

		do 
        { 
            CARDME_EEPDATA = 0; 
            EepromWaitBusy(); 
        } while (CARDME_EEPDATA & 0x01);	// WIP (Write In Progress) ?
        EepromWaitBusy(); 
		CARDME_CR1 = /*MODE*/0x40;

	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
