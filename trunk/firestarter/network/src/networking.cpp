/* Project24
 * Copyright (c) 2002 - 2003 Jeffrey Myers
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named license.txt that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
// Network.cpp : a bunch of network stuff
//

#ifdef _WIN32
#pragma warning( disable : 4786 )  // Disable warning message
#endif

#include <set>
#include <map>
#include <vector>

#include "Networking.h"
#include "enet/enet.h"

// enet inits
int  iInitCount = 0;

void InitNetwork ( void )
{
	if (iInitCount <1)
	{
		enet_initialize();
	}
	iInitCount++;
}

void FreeNetwork ( void )
{
	iInitCount--;
	
	if (iInitCount <1)
	{
		enet_deinitialize();
		iInitCount = 0;
	}
}

// data pack utils
void WriteInt32 ( void *pData, int iData )
{
	(*(unsigned int *)pData) = ENET_HOST_TO_NET_32(iData);
}

int	ReadInt32 ( void *pData )
{
	return ENET_NET_TO_HOST_32(*(unsigned long*)(pData));
}

void WriteInt16 ( void *pData, int iData )
{
	(*(unsigned short *)pData) = ENET_HOST_TO_NET_16(iData);
}

int	ReadInt16 ( void *pData )
{
	return ENET_NET_TO_HOST_16(*(unsigned short*)pData);
}

void WriteFloat32 ( void *pData, float fData )
{
	int	iData;

	memcpy(&iData,&fData,4);
	WriteInt32(pData,iData);
}

float ReadFloat32  ( void *pData )
{
	int iVal = ReadInt32(pData);

	float fData;
	
	memcpy(&fData,&iVal,4);
	
	return fData;
}

//---------------------data chunk-----------------------------

typedef struct 
{
	teChunkDataTypes	type;
	int				offset;
}trChunkItem;

typedef std::vector<trChunkItem>	tvItemList;
typedef std::vector<unsigned char>	tvDataList;

struct CDataChunk::trDataChunkInfo
{
	tvItemList	items;
	tvDataList	data;

	unsigned short	type;
	int				current;
};	

CDataChunk::CDataChunk()
{
	info = new trDataChunkInfo;
	info->current = 0;
}

CDataChunk::CDataChunk(const CDataChunk& cunk)
{
	info = new trDataChunkInfo;
	info->current = cunk.info->current;
	info->type = cunk.info->type;
	info->data = cunk.info->data;
	info->items = cunk.info->items;
}

CDataChunk& CDataChunk::operator = (const CDataChunk& cunk)
{
	info->current = cunk.info->current;
	info->type = cunk.info->type;
	info->data = cunk.info->data;
	info->items = cunk.info->items;

	return *this;
}

CDataChunk::~CDataChunk()
{
	if (info)
		delete(info);
}

void CDataChunk::SetType ( unsigned short type )
{
	info->type = type;
}

void CDataChunk::SetType ( const char *type )
{
	SetType(*((unsigned short*)type));
}

unsigned short CDataChunk::GetType ( void )
{
	return info->type;
}

void CDataChunk::AddB ( bool data )
{
	trChunkItem	item;

	item.type = eBool;
	item.offset = (int)info->data.size();
	info->items.push_back(item);
	
	unsigned char pack = data;
	info->data.push_back(pack);
}

void CDataChunk::AddC ( unsigned char data )
{
	trChunkItem	item;

	item.type = eChar;
	item.offset = (int)info->data.size();
	info->items.push_back(item);
	
	unsigned char pack = data;
	info->data.push_back(pack);
}

void CDataChunk::AddStr ( const char *data )
{
	if (!data)
		return;

	trChunkItem	item;

	item.type = eString;
	item.offset = (int)info->data.size();
	info->items.push_back(item);
	
	unsigned short len;
	WriteInt16(&len,(int)strlen((const char*)data));
	info->data.push_back(((char*)&len)[0]);
	info->data.push_back(((char*)&len)[1]);

	for (unsigned int i = 0; i<strlen((const char*)data);i++)
		info->data.push_back(data[i]);
}

void CDataChunk::AddS ( unsigned short data )
{
	trChunkItem	item;

	item.type = eShort;
	item.offset = (int)info->data.size();
	info->items.push_back(item);
	
	unsigned short temp;
	WriteInt16(&temp,data);
	info->data.push_back(((char*)&temp)[0]);
	info->data.push_back(((char*)&temp)[1]);
}

void CDataChunk::AddI ( unsigned int data )
{
	trChunkItem	item;

	item.type = eInt;
	item.offset = (int)info->data.size();
	info->items.push_back(item);
	
	unsigned int temp;
	WriteInt32(&temp,data);
	info->data.push_back(((char*)&temp)[0]);
	info->data.push_back(((char*)&temp)[1]);
	info->data.push_back(((char*)&temp)[2]);
	info->data.push_back(((char*)&temp)[3]);
}
	
void CDataChunk::AddF ( float data )
{
	trChunkItem	item;

	item.type = eFloat;
	item.offset = (int)info->data.size();
	info->items.push_back(item);
	
	float temp;
	WriteFloat32(&temp,data);
	info->data.push_back(((char*)&temp)[0]);
	info->data.push_back(((char*)&temp)[1]);
	info->data.push_back(((char*)&temp)[2]);
	info->data.push_back(((char*)&temp)[3]);
}
	
void CDataChunk::AddV ( float data[3] )
{
	trChunkItem	item;

	item.type = eVector;
	item.offset = (int)info->data.size();
	info->items.push_back(item);
	
	float temp;
	WriteFloat32(&temp,data[0]);
	info->data.push_back(((char*)&temp)[0]);
	info->data.push_back(((char*)&temp)[1]);
	info->data.push_back(((char*)&temp)[2]);
	info->data.push_back(((char*)&temp)[3]);

	WriteFloat32(&temp,data[1]);
	info->data.push_back(((char*)&temp)[0]);
	info->data.push_back(((char*)&temp)[1]);
	info->data.push_back(((char*)&temp)[2]);
	info->data.push_back(((char*)&temp)[3]);

	WriteFloat32(&temp,data[2]);
	info->data.push_back(((char*)&temp)[0]);
	info->data.push_back(((char*)&temp)[1]);
	info->data.push_back(((char*)&temp)[2]);
	info->data.push_back(((char*)&temp)[3]);
}

void CDataChunk::AddN ( unsigned int size, void *data )
{
	return;

	trChunkItem	item;

	item.type = eRaw;
	item.offset = (int)info->data.size();
	info->items.push_back(item);

	unsigned int len;
	WriteInt32(&len,size);
	info->data.push_back(((char*)&len)[0]);
	info->data.push_back(((char*)&len)[1]);
	info->data.push_back(((char*)&len)[2]);
	info->data.push_back(((char*)&len)[3]);

	for (unsigned int i = 0; i<size;i++)
		info->data.push_back(((unsigned char*)data)[i]);
}
	
int CDataChunk::GetElements ( void )
{
	return (int)info->items.size();
}

void CDataChunk::SetElelemnt ( void )
{
	info->current = 0;
}

teChunkDataTypes CDataChunk::GetNextType ( void )
{
	if ( info->current >= GetElements())
		return eUnknown;

	return info->items[info->current].type;
}

unsigned int CDataChunk::GetNextSize ( void )
{
	switch (GetNextType())
	{
		case eUnknown:
			return 0;
		case eBool:
			return 1;
		case eChar:
			return 1;
		case eString:
		{
			unsigned char temp[2];
			temp[0] = info->data[info->items[info->current].offset];
			temp[1] = info->data[info->items[info->current].offset+1];
			return ReadInt16(temp);
		}
		case eShort:
			return 2;
		case eInt:
			return 4;
		case eFloat:
			return 4;
		case eVector:
			return 12;
		case eRaw:
		{
			unsigned char temp[4];
			temp[0] = info->data[info->items[info->current].offset];
			temp[1] = info->data[info->items[info->current].offset+1];
			temp[2] = info->data[info->items[info->current].offset+2];
			temp[3] = info->data[info->items[info->current].offset+3];
			return ReadInt32(temp);
		}
		default:
			return 0;
	}
}

bool CDataChunk::ReadB ( void )
{
	if (GetNextType()!= eBool)
		return false;

	bool data = info->data[info->items[info->current].offset] != 0;

	NextElement();

	return data;
}

unsigned char CDataChunk::ReadC ( void )
{
	if (GetNextType()!= eChar)
		return 0;

	unsigned char data = info->data[info->items[info->current].offset];

	NextElement();
	return data;
}

const char* CDataChunk::ReadStr ( void )
{
	if (GetNextType()!= eString)
		return NULL;

	static char temp[1024] = {0};

	int size = GetNextSize();

	if(size > 1023)
		size = 1023;

	for(int i = 0; i<size; i++)
		temp[i] = info->data[info->items[info->current].offset+2+i];

	temp[size] = NULL;

	NextElement();
	return temp;
}

unsigned short CDataChunk::ReadS ( void )
{
	if (GetNextType()!= eShort)
		return 0;

	unsigned char temp[2];
	temp[0] = info->data[info->items[info->current].offset];
	temp[1] = info->data[info->items[info->current].offset+1];
	unsigned short data = ReadInt16(temp);

	NextElement();
	return data;
}

unsigned int CDataChunk::ReadI ( void )
{
	if (GetNextType()!= eInt)
		return 0;

	unsigned char temp[4];
	temp[0] = info->data[info->items[info->current].offset];
	temp[1] = info->data[info->items[info->current].offset+1];
	temp[2] = info->data[info->items[info->current].offset+2];
	temp[3] = info->data[info->items[info->current].offset+3];
	unsigned int data = ReadInt32(temp);

	NextElement();
	return data;
}
	
float CDataChunk::ReadF ( void )
{
	if (GetNextType()!= eFloat)
		return 0;

	unsigned char temp[4];
	temp[0] = info->data[info->items[info->current].offset];
	temp[1] = info->data[info->items[info->current].offset+1];
	temp[2] = info->data[info->items[info->current].offset+2];
	temp[3] = info->data[info->items[info->current].offset+3];
	float data = ReadFloat32(temp);

	NextElement();
	return data;
}
	
void CDataChunk::ReadV ( float data[3] )
{
	if (GetNextType()!= eVector)
		return;

	unsigned char temp[4];
	temp[0] = info->data[info->items[info->current].offset];
	temp[1] = info->data[info->items[info->current].offset+1];
	temp[2] = info->data[info->items[info->current].offset+2];
	temp[3] = info->data[info->items[info->current].offset+3];
	data[0] = ReadFloat32(temp);

	temp[0] = info->data[info->items[info->current].offset+4];
	temp[1] = info->data[info->items[info->current].offset+4+1];
	temp[2] = info->data[info->items[info->current].offset+4+2];
	temp[3] = info->data[info->items[info->current].offset+4+3];
	data[1] = ReadFloat32(temp);
	
	temp[0] = info->data[info->items[info->current].offset+8];
	temp[1] = info->data[info->items[info->current].offset+8+1];
	temp[2] = info->data[info->items[info->current].offset+8+2];
	temp[3] = info->data[info->items[info->current].offset+8+3];
	data[2] = ReadFloat32(temp);

	NextElement();
}

void CDataChunk::ReadN ( void *data )
{
	if (GetNextType()!= eRaw)
		return;

	int size = GetNextSize();

	for(int i = 0; i<size; i++)
		((unsigned char*)data)[i] = info->data[info->items[info->current].offset+4+i];
}	

unsigned int CDataChunk::NetReadData( void *data )
{
	if (!data )
		return 0;

	unsigned char *pData = (unsigned char*)data;

	unsigned int iOffset = 0;	// read past the packet data descriptor (DC)

	info->current = 0;
	info->data.clear();
	info->items.clear();
	info->type = ReadInt16(pData);	// the data chunk desciptor
	unsigned int size = ReadInt32(&pData[2]);	// the data chunk desciptor

	if (size < 6)
		return size;

	iOffset = 6;

	while (iOffset < size)
	{
		trChunkItem	item;

		teChunkDataTypes	type = (teChunkDataTypes)pData[iOffset];

		iOffset++;
		switch(type)
		{
			case eUnknown:
				break;

			case eBool:
			{
				AddB((bool)(pData[iOffset++]!=0));
				break;
			}
			case eChar:
			{
				AddC(pData[iOffset++]);
				break;
			}
			case eString:
			{
				unsigned char temp[2];
				temp[0] = pData[iOffset++];
				temp[1] = pData[iOffset++];
				int iSize = ReadInt16(temp);

				char *szData = (char*)malloc(iSize+1);
				strncpy(szData,(const char*)(&(pData[iOffset])),iSize);
				szData[iSize] = NULL;
				AddStr(szData);
				free(szData);
				break;
			}
			case eShort:
			{
				item.type = eShort;
				item.offset = (int)info->data.size();
				info->items.push_back(item);

				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				break;
			}
			case eInt:
			{
				item.type = eInt;
				item.offset = (int)info->data.size();
				info->items.push_back(item);

				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				break;
			}
			case eFloat:
			{
				item.type = eFloat;
				item.offset = (int)info->data.size();
				info->items.push_back(item);

				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				break;
			}
			case eVector:
			{
				item.type = eFloat;
				item.offset = (int)info->data.size();
				info->items.push_back(item);

				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);

				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);

				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				info->data.push_back(pData[iOffset++]);
				break;
			}
			case eRaw:
			{
				item.type = eRaw;
				item.offset = (int)info->data.size();
				info->items.push_back(item);

				unsigned char temp[4];
				temp[0] = pData[iOffset++];
				temp[1] = pData[iOffset++];
				temp[2] = pData[iOffset++];
				temp[3] = pData[iOffset++];
				int iSize = ReadInt32(temp);

				info->data.push_back(temp[0]);
				info->data.push_back(temp[1]);
				info->data.push_back(temp[2]);
				info->data.push_back(temp[3]);
				
				for (int i = 0; i<iSize;i++)
					info->data.push_back(pData[iOffset++]);

				break;
			}
			default:
				break;
		}
	}

	return size;
}

unsigned int CDataChunk::NetWriteData( void *data )
{
	if (GetElements()<1)
		return -1;

	// size of the data
	int newLen = GetElements()+(int)info->data.size()+6;
	if (!data)
		return newLen;

	char* pData = (char*)data;
	// write the packet type descriptor
	pData[0] = 'D';
	pData[1] = 'C';

	WriteInt16(&(pData[0]),info->type);
	// and how big we are, as we may be mid stream
	WriteInt32(&(pData[2]),newLen);

	int iOffset = 6;

	tvItemList::iterator	itr = info->items.begin();

	while ( itr!= info->items.end() )
	{
		pData[iOffset] = itr->type;
		iOffset++;
		
		switch(itr->type)
		{
			case eUnknown:
				break;

			case eBool:
			case eChar:
			{
				pData[iOffset++] = info->data[itr->offset];
				break;
			}
			case eString:
			{
				unsigned char temp[2];
				temp[0] = info->data[itr->offset];
				temp[1] = info->data[itr->offset+1];
				int iSize = ReadInt16(temp);

				pData[iOffset++] = temp[0];
				pData[iOffset++] = temp[1];

				memcpy(&pData[iOffset],&info->data[itr->offset+2],iSize+1);

				iOffset += iSize+1;
				break;
			}
			case eShort:
			{
				memcpy(&pData[iOffset],&info->data[itr->offset],2);
				iOffset += 2;
				break;
			}
			case eInt:
			case eFloat:
				memcpy(&pData[iOffset],&info->data[itr->offset],4);
				iOffset += 4;
				break;
			case eVector:
				memcpy(&pData[iOffset],&info->data[itr->offset],12);
				iOffset += 12;
				break;
			case eRaw:
			{
				unsigned char temp[4];
				temp[0] = info->data[itr->offset];
				temp[1] = info->data[itr->offset+1];
				temp[2] = info->data[itr->offset+2];
				temp[3] = info->data[itr->offset+3];
				int iSize = ReadInt32(temp);

				pData[iOffset++] = temp[0];
				pData[iOffset++] = temp[1];
				pData[iOffset++] = temp[2];
				pData[iOffset++] = temp[3];

				memcpy(&pData[iOffset],&info->data[itr->offset+4],iSize+1);
				iOffset += iSize;
				break;
			}
			default:
				break;
		}
		itr++;
	}
	return newLen;
}

void CDataChunk::NextElement ( void )
{
	info->current++;
}


//---------------------net message-----------------------------

struct CNetworkMessage::trNetMessageInfo
{
	unsigned int	dataSize;
	void			*data;
	unsigned short	type;
	unsigned int	current;
};

CNetworkMessage::CNetworkMessage()
{
	info = new trNetMessageInfo;
	info->data = NULL;
	ClearData();
}

CNetworkMessage::CNetworkMessage(const CNetworkMessage& message)
{
	info = new trNetMessageInfo;
	info->type = message.info->type;
	info->current = message.info->current;

	info->dataSize = message.info->dataSize;
	info->data = malloc(message.info->dataSize);
	memcpy(info->data,message.info->data,info->dataSize+2);
}

CNetworkMessage& CNetworkMessage::operator = (const CNetworkMessage& message)
{
	info->type = message.info->type;
	info->current = message.info->current;

	if (info->data)
		free (info->data);
	info->dataSize = message.info->dataSize;
	info->data = malloc(message.info->dataSize);
	memcpy(info->data,message.info->data,info->dataSize+2);
	return *this;
}

CNetworkMessage::~CNetworkMessage()
{
	if (info->data)
		free(info->data);
	info->data = NULL;
	info->dataSize = -1;
	if (info)
		delete(info);
}

void CNetworkMessage::SetDataSize ( int iSize )
{
	void	*newData = malloc(iSize+2);
	if (!newData)
		return;

	memset(newData,0,iSize+2);

	int copySize = info->dataSize;
	if (copySize >= iSize+2)
		copySize = iSize+2;
	
	memcpy(newData,info->data,copySize+2);

	free(info->data);
	info->data = newData;
	info->dataSize = iSize+2;
}

void CNetworkMessage::ClearData ( void )
{
	if (info->data)
		free(info->data);
	info->data = malloc(2);
	memset(info->data,0,2);
	info->type = 0;
	info->dataSize = 2;
	ResetRead();
}

void CNetworkMessage::Send ( CNetworkPeer &peer, bool relyable )
{
	enet_uint32	flags = 0;
	if(relyable)
		flags |=ENET_PACKET_FLAG_RELIABLE;

	ENetPacket		*packet = enet_packet_create(info->data,info->dataSize,flags);

	ENetPeer *enetpeer = (ENetPeer*) peer.GetPeer();
	enet_peer_send(enetpeer,0,packet);
}

void CNetworkMessage::SetType ( unsigned short type )
{
	ClearData();
	info->type = type;
	WriteInt16(info->data,type);
}

void CNetworkMessage::SetType ( const char *type )
{
	CNetworkMessage::SetType(*((unsigned short*)type));
}

unsigned short CNetworkMessage::GetType ( void )
{
	return info->type;
}

unsigned int CNetworkMessage::GetSize ( void )
{
	return info->dataSize-2;
}

void CNetworkMessage::SetDataFromPacket( void *packet )
{
	if (!packet)
		return;

	SetDataSize((int)((ENetPacket*)packet)->dataLength-2);

	memcpy(info->data,((ENetPacket*)packet)->data,((ENetPacket*)packet)->dataLength);

	info->type = ReadInt16(info->data);
}

// raw data IO
unsigned int CNetworkMessage::GetSizeRaw ( void )
{
	return info->dataSize;
}

void CNetworkMessage::GetDataMem( void *packet )
{
	// we asume they have set us up with the right size
	if(!packet)
		return;

	memcpy(packet,info->data,info->dataSize);
}

void CNetworkMessage::SetDataFromMem( void *packet, unsigned int size )
{
	// this asumes that the type is in the data
	if (!packet)
		return;

	SetDataSize(size-2);
	memcpy(info->data,packet,size);
	info->type = ReadInt16(info->data);
}

	// variable data pack stuff
void CNetworkMessage::AddB ( bool data )
{
	SetDataSize(GetSize()+1);
	memcpy((char*)(info->data)+info->dataSize-1,&data,1);
}

void CNetworkMessage::AddC ( unsigned char data )
{
	SetDataSize(GetSize()+1);
	memcpy((char*)(info->data)+info->dataSize-1,&data,1);
}

void CNetworkMessage::AddStr ( const char *data )
{
	int iLen = GetSize();

	int stringLen = (int)strlen((char*)data)+1;
	SetDataSize(iLen+stringLen);
	memcpy((char*)(info->data)+iLen+2,data,stringLen);
}

void CNetworkMessage::AddS ( unsigned short data )
{
	int iLen = GetSize();
	SetDataSize(iLen+2);
	WriteInt16((char*)(info->data)+iLen+2,data);
}

void CNetworkMessage::AddI ( unsigned int data )
{
	int iLen = GetSize();

	SetDataSize(iLen+4);
	WriteInt32((char*)(info->data)+iLen+2,data);
}
	
void CNetworkMessage::AddF ( float data )
{
	int iLen = GetSize();

	SetDataSize(iLen+4);
	WriteFloat32((char*)(info->data)+iLen+2,data);
}
	
void CNetworkMessage::AddV ( float data[3] )
{
	int iLen = GetSize();

	SetDataSize(iLen+12);
	WriteFloat32((char*)(info->data)+iLen+2,data[0]);
	WriteFloat32((char*)(info->data)+iLen+4+2,data[1]);
	WriteFloat32((char*)(info->data)+iLen+8+2,data[2]);
}
	
void CNetworkMessage::AddN ( unsigned int size, void *data )
{
	int iLen = GetSize();

	SetDataSize(iLen+size);
	memcpy((char*)(info->data)+iLen+2,data,size);
}
	
void CNetworkMessage::AddChunk ( CDataChunk &chunk )
{
	unsigned int size = chunk.NetWriteData(NULL);
	void *data = malloc(size);
	chunk.NetWriteData(data);
	AddN(size,data);
	free(data);
}

bool CNetworkMessage::MoreToRead ( void )
{
	return (info->current < info->dataSize+2);
}

void CNetworkMessage::ResetRead ( void )
{
	info->current = 2;
}

unsigned long CNetworkMessage::DataLeft ( void )
{
	return info->current - info->dataSize+2; 
}

bool CNetworkMessage::ReadB ( void )
{
	if (info->current+1 > info->dataSize+2)
		return false;

	bool data = ((char*)(info->data))[info->current] != 0;

	info->current++;

	return data;
}

unsigned char CNetworkMessage::ReadC ( void )
{
	if (info->current+1 > info->dataSize+2)
		return 0;

	unsigned char data = ((char*)(info->data))[info->current];

	info->current++;

	return data;
}

const char* CNetworkMessage::ReadStr ( void )
{
	if (info->current+1 > info->dataSize+2)
		return NULL;
	
	char *str = (char*)(&((char*)info->data)[info->current]);
	info->current+= (int)strlen(str)+1;
	return str;
}

unsigned short CNetworkMessage::ReadS ( void )
{
	if (info->current+2 > info->dataSize+2)
		return 0;

	unsigned short data = ReadInt16(&((char*)info->data)[info->current]);
	info->current+= 2;
	return data;
}

unsigned int CNetworkMessage::ReadI ( void )
{
	if (info->current+4 > info->dataSize+2)
		return 0;

	unsigned int data = ReadInt32(&((char*)info->data)[info->current]);
	info->current+= 4;
	return data;
}
	
float CNetworkMessage::ReadF ( void )
{
	if (info->current+4 > info->dataSize+2)
		return 0.0f;

	float data = ReadFloat32(&((char*)info->data)[info->current]);
	info->current+= 4;
	return data;
}
	
void CNetworkMessage::ReadV ( float data[3] )
{
	data[0] = ReadF();
	data[1] = ReadF();
	data[2] = ReadF();
}

void CNetworkMessage::ReadN ( unsigned int size, void *data )
{
	if (info->current+size > info->dataSize+2)
		return;

	memcpy(data,&((char*)info->data)[info->current],size);
	info->current+= size;
}

void CNetworkMessage::ReadChunk ( CDataChunk &chunk )
{
	// make sure there is at least a header and a size
	if (info->current+6 > info->dataSize+2)
		return;

	// make sure the size is not over the data
	if ( ReadInt32(&((char*)info->data)[info->current+2]) +info->current < info->dataSize+2 )
		return;

	unsigned int size = chunk.NetReadData(&((char*)info->data)[info->current]);
	info->current+= size;
}


//---------------------peer-----------------------------

#define _DNS_STRING_LEN	64

struct CNetworkPeer::trNetPeerInfo
{
	ENetPeer	*peer;
	void		*param;
	char		dns[_DNS_STRING_LEN];
};

CNetworkPeer::CNetworkPeer(const CNetworkPeer& peer)
{
	address = peer.address;

	info = new trNetPeerInfo;
	memcpy(info,peer.info,sizeof(trNetPeerInfo));
}

CNetworkPeer& CNetworkPeer::operator = (const CNetworkPeer& peer)
{
	address = peer.address;
	memcpy(info,peer.info,sizeof(trNetPeerInfo));
	return *this;
}

CNetworkPeer::CNetworkPeer()
{
	address.host = 0;
	address.port = 0;

	info = new trNetPeerInfo;
	info->peer = NULL;
	info->param = NULL;
}

CNetworkPeer::~CNetworkPeer()
{
	if (info)
		delete(info);
}

void* CNetworkPeer::GetData (void)
{
	return info->param;
}

void CNetworkPeer::SetData ( void *param)
{
	info->param = param;
}

void* CNetworkPeer::GetPeer (void)
{
	return 	info->peer;
}

void CNetworkPeer::SetPeer ( void *peer)
{
	info->peer = (ENetPeer*)peer;
}

const char* CNetworkPeer::GetDNSName ( void )
{
	enet_address_get_host((ENetAddress*)&address,info->dns,_DNS_STRING_LEN);
	return (info->dns);
}

//---------------------network server-----------------------------

typedef std::map<ENetPeer*, CNetworkPeer> tvPeerMap;

struct CNetworkServer::trNetServerInfo
{
	tvPeerMap	peerMap;
	ENetHost *	host;
	CNetworkMessageProcessor *messageProcessor;
};

CNetworkServer::CNetworkServer()
{
	info = new trNetServerInfo;
	info->host = NULL;
	info->messageProcessor = NULL;
}

CNetworkServer::~CNetworkServer()
{
	if(!info)
		return;

	Stop();
	delete(info);
}

void CNetworkServer::Start ( int iClients, int iMaxUp, int iMaxDown, int iPort )
{
	ENetAddress	addy;

	addy.host = ENET_HOST_ANY;
	addy.port = (unsigned short)iPort;
	info->host = enet_host_create(&addy,iClients,iMaxUp,iMaxDown);

}
void CNetworkServer::Stop( void )
{
	if (!info->host)
		return;

	enet_host_flush(info->host);
	enet_host_destroy(info->host);
	info->host = NULL;
}

void CNetworkServer::SetMesageHandaler ( CNetworkMessageProcessor *messageProcessor )
{
	info->messageProcessor = messageProcessor;
}

void CNetworkServer::ProcessMessages ( void )
{
	if (!info->host)
		return;

	ENetEvent	event;
	unsigned int time = 0;

	while(info->host && (enet_host_service(info->host,&event,time) >0))
	{
		switch(event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				tvPeerMap::iterator itr = info->peerMap.find(event.peer);

				if (itr != info->peerMap.end())
				{	
					if (info->messageProcessor)
						info->messageProcessor->OnDisconnect(itr->second);

					info->peerMap.erase(itr);
				}

				CNetworkPeer peer;
				peer.SetPeer(event.peer);
				info->peerMap[event.peer] = peer;
				
				if (info->messageProcessor)
					info->messageProcessor->OnConnect(info->peerMap[event.peer]);
			}// we continue just in case there is some data with the connect message, then we bust that out into a message

			case ENET_EVENT_TYPE_RECEIVE:
			{
				if(!event.packet || event.packet->dataLength <2)
					break;

				CNetworkMessage	message;
				message.SetDataFromPacket(event.packet);
							
				if (info->messageProcessor)
					info->messageProcessor->OnMessage(info->peerMap[event.peer],message);
			}
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
				if (info->messageProcessor)
					info->messageProcessor->OnDisconnect(info->peerMap[event.peer]);

				// remove us from the peer map
				tvPeerMap::iterator itr = info->peerMap.find(event.peer);

				if (itr != info->peerMap.end())
					info->peerMap.erase(itr);

			break;
		}
	}
}

void CNetworkServer::KickUser ( CNetworkPeer &peer )
{
	tvPeerMap::iterator itr = info->peerMap.find((ENetPeer*)peer.GetPeer());
	if (itr != info->peerMap.end())
		info->peerMap.erase(itr);

	enet_peer_disconnect((ENetPeer*)peer.GetPeer());
}

void CNetworkServer::FlushUsers ( void )
{
	tvPeerMap::iterator itr = info->peerMap.begin();

	while (itr != info->peerMap.end())
	{
		enet_peer_disconnect((ENetPeer*)itr->second.GetPeer());
		itr++;
	}
	enet_host_flush(info->host);

	info->peerMap.clear();
}


// _---------------------------------------------client-------------------------------
struct CNetworkClient::trNetClientInfo
{
	ENetHost *	host;
	CNetworkMessageProcessor *messageProcessor;
};

CNetworkClient::CNetworkClient()
{
	info = new trNetClientInfo;
	info->host = NULL;
	info->messageProcessor = NULL;
}

CNetworkClient::~CNetworkClient()
{
	if (!info)
		return;

	Disconect();
	delete(info);		
}

void CNetworkClient::Connect ( trNetAddress & host )
{
	Disconect();
	info->host = enet_host_create(NULL,1,0,0);
	ENetPeer *peer = enet_host_connect(info->host,(ENetAddress*)&host,1);

	server.SetPeer(peer);
}

void CNetworkClient::Connect ( const char * host, int port )
{
	trNetAddress	address;

	if (enet_address_set_host((ENetAddress*)&address,host) != 0)
		return;

	address.port = (unsigned short)port;
	Connect(address);
}

bool CNetworkClient::Connected ( void )
{
	return (info->host != NULL) && (server.GetPeer()!=NULL);
}


void CNetworkClient::Disconect ( void )
{
	if (!info->host)
		return;

	enet_host_flush(info->host);
	enet_peer_disconnect((ENetPeer*)server.GetPeer());
	enet_host_destroy(info->host);
	info->host = NULL;
	server.SetPeer(NULL);
}

void CNetworkClient::SetMesageHandaler ( CNetworkMessageProcessor *messageProcessor )
{
	info->messageProcessor = messageProcessor;
}

void CNetworkClient::ProcessMessages ( void )
{
	if (!info->host)
		return;

	ENetEvent	event;
	unsigned int time = 0;

	while(info->host && (enet_host_service(info->host,&event,time) >0))
	{
		if (event.peer == server.GetPeer())
		{
			switch(event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					if (info->messageProcessor)
						info->messageProcessor->OnConnect(server);
				}
				case ENET_EVENT_TYPE_RECEIVE:
				{
					if(!event.packet || event.packet->dataLength <2)
						break;

					CNetworkMessage	message;
					message.SetDataFromPacket(event.packet);
					
					if (info->messageProcessor)
						info->messageProcessor->OnMessage(server,message);
				}
				break;

				case ENET_EVENT_TYPE_DISCONNECT:
					if (info->messageProcessor)
						info->messageProcessor->OnDisconnect(server);
					Disconect();
				break;
			}
		}
	}
}

