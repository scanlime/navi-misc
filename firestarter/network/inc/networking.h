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

#ifndef _UDP_NETWORKING_H_
#define _UDP_NETWORKING_H_

// init/end functions. Call these at the start of your app, and at the end.
void  InitNetwork ( void );
void  FreeNetwork ( void );

// pack utills
// read/write data from/to network byte order
void  WriteInt32 ( void *pData, int iData );
int	 ReadInt32 ( void *pData );

void  WriteInt16 ( void *pData, int iData );
int	 ReadInt16 ( void *pData );

void  WriteFloat32 ( void *pData, float fData );
float  ReadFloat32  ( void *pData );

// basic address structure;
typedef struct
{
   unsigned int host;
   unsigned short port;
} trNetAddress;

typedef enum
{
	eUnknown,
	eBool,
	eChar,
	eString,
	eShort,
	eInt,
	eFloat,
	eVector,
	eRaw
}teChunkDataTypes;

// a class that takes N data items, and allows them to be packed in, then enumerated out.
// will not let you read or write over memory.
// should make one of these that takes item names for each field
class  CDataChunk
{
public:	
	CDataChunk();
	CDataChunk(const CDataChunk& cunk);
	~CDataChunk();

	CDataChunk& operator = (const CDataChunk& cunk);

	void SetType ( unsigned short type );
	void SetType ( const char *type );
	unsigned short GetType ( void );

	void AddB ( bool data );
	void AddC ( unsigned char data );
	void AddStr ( const char *data );
	void AddS ( unsigned short data );
	void AddI ( unsigned int data );	
	void AddF ( float data );	
	void AddV ( float data[3] );	
	void AddN ( unsigned int size, void *data );	

	int GetElements ( void );
	void SetElelemnt ( void );
	teChunkDataTypes GetNextType ( void );
	unsigned int GetNextSize ( void );

	bool ReadB ( void );
	unsigned char ReadC ( void );
	const char* ReadStr ( void );
	unsigned short ReadS ( void );
	unsigned int ReadI ( void );	
	float ReadF ( void );	
	void ReadV ( float data[3] );
	void ReadN ( void *data );	

	unsigned int NetReadData( void *data );
	unsigned int NetWriteData( void *data );

private:
	void NextElement ( void );
	
	struct trDataChunkInfo;
	trDataChunkInfo *info;
};

// abstraction of a network connection.
// class reporesents some other machine on a network, ether the server you are connected to, or a client connect to a server.
class  CNetworkPeer
{
public:
	CNetworkPeer();
	CNetworkPeer(const CNetworkPeer& peer);
	~CNetworkPeer();

	CNetworkPeer& operator = (const CNetworkPeer& peer);
	
	// a user defined value,
	// can be used to store class based data.
	// will be stored in master peer list
	void* GetData (void);
	void SetData ( void *param);

	// an app should never need to call this.
	// but it makes a nice key for client maps
	void* GetPeer (void);
	void SetPeer ( void *peer);

	// the address of the connection
	trNetAddress GetAddress ( void ) {return address;}
	void SetAddress (trNetAddress & addy) {address = addy;}

	const char* GetDNSName ( void );

private:
	trNetAddress	address;
	struct trNetPeerInfo;
	trNetPeerInfo	*info;
};

// abstration of the basic network message.
// most functions are geared to a message that has a 
// short as a type ID, then some data.
// class has data pack functions, but is NOT mem secure.
// you must be sure that you know what you are reading.
// read methods will not start past the end of data
// add methods will automaticly resize the data.
// if needed, there are raw functions to get/set the pure data.
class  CNetworkMessage
{
public:
	CNetworkMessage();
	CNetworkMessage(const CNetworkMessage& message);
	~CNetworkMessage();
	CNetworkMessage& operator = (const CNetworkMessage& message);
	
	// send this message to a peer;
	// will dupe the data, and posibly break into chuncks on send.
	void Send ( CNetworkPeer &peer, bool relyable );

	// type is 2 bytes, ether as a short, or 2 chars.
	void SetType ( unsigned short type );
	void SetType ( const char *type );
	unsigned short GetType ( void );

	// size of the data portion of the message
	unsigned int GetSize ( void );

	// sets the message from a transport layer packet
	// users should not call.
	void SetDataFromPacket( void *packet );

	// raw data IO
	// access to the base data, does not asume 2 byte type header.
	unsigned int GetSizeRaw ( void );
	void SetDataFromMem( void *packet, unsigned int size );
	void GetDataMem( void *packet );

	// data pack IO
	// adds the item to the data, will auto size
	void AddB ( bool data );
	void AddC ( unsigned char data );
	void AddStr ( const char *data );
	void AddS ( unsigned short data );
	void AddI ( unsigned int data );	
	void AddF ( float data );	
	void AddV ( float data[3] );	
	void AddN ( unsigned int size, void *data );	
	void AddChunk ( CDataChunk &chunk );

	// sets the data read counter to the begining ( default )
	void ResetRead ( void );
	// true if there is more data to read, false if there is not
	bool MoreToRead ( void );
	// returns how much data is left to read.
	unsigned long DataLeft ( void );

	// reads the item from the data, and increments by that data's size.
	// will not start after the end of the data
	// will try REALLY hard to not read over the end of the data
	bool ReadB ( void );
	unsigned char ReadC ( void );
	const char* ReadStr ( void );
	unsigned short ReadS ( void );
	unsigned int ReadI ( void );	
	float ReadF ( void );	
	void ReadV ( float data[3] );
	void ReadN ( unsigned int size, void *data );	
	void ReadChunk ( CDataChunk &chunk );	

	// resize the data to this size, will maintain as much data as it can, will truncate at new length if data is now smaller
	void SetDataSize ( int iSize );

	// clear all the data in the message, and set the type to 0
	void ClearData ( void );

private:
	struct trNetMessageInfo;
	trNetMessageInfo	*info;
};

class  CNetworkMessageProcessor
{
public:
	CNetworkMessageProcessor() {return;}
	virtual ~CNetworkMessageProcessor() {return;}

	virtual void OnConnect ( CNetworkPeer &peer ) {return;}
	virtual void OnDisconnect ( CNetworkPeer &peer ) {return;}
	virtual void OnMessage ( CNetworkPeer &peer, CNetworkMessage &message ) {return;}
};

class  CNetworkServer 
{
public:
	CNetworkServer();
	~CNetworkServer();

	void Start ( int iClients, int iMaxUp, int iMaxDown, int iPort );
	void Stop ( void );

	void SetMesageHandaler ( CNetworkMessageProcessor *messageProcessor );

	// if we can go multi-thread, we don't need this
	void ProcessMessages ( void );

	void KickUser ( CNetworkPeer &peer );
	void FlushUsers ( void );

private:
	struct trNetServerInfo;
	trNetServerInfo	*info;
};

class  CNetworkClient 
{
public:
	CNetworkClient();
	~CNetworkClient();

	void Connect ( trNetAddress & host );
	void Connect ( const char * host, int port );

	void Disconect ( void );

	bool Connected ( void );

	void SetMesageHandaler ( CNetworkMessageProcessor *messageProcessor );

	// if we can go multi-thread, we don't need this
	void ProcessMessages ( void );

	CNetworkPeer& GetServerPeer ( void ) {return server;}

private:
	CNetworkPeer	server;
	struct trNetClientInfo;
	trNetClientInfo	*info;
};

#endif//_UDP_NETWORKING_H_

