// SMELoad.cpp : Defines the entry point for the DLL application.
//

#ifdef WIN32
#include <windows.h>
#endif
#include "SMELoad.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct Point
{
  short v;
  short h;
} Point;

typedef struct Rect
{
  short top;
  short left;
  short bottom;
  short right;
} Rect;


typedef struct
{
	char identifier [8];
	signed short mode;
	signed short depth;
	signed short rows;
	signed short cols;
	signed short planes;
	
	signed long resourceLength;
	
}FileHeader;

typedef struct
{
	unsigned char	r,g,b;
}Pixel;

typedef Pixel PILookUpTable [256];

typedef struct
{	
	FileHeader		sFileHeader;
	PILookUpTable	sPalletData;
	char			*aPanes[32];
	void			*pHeaderResourceData;
}trSMEImage;


bool LoadSME(const char* szFileName, trSMEHeaderInfo *pSMEHeader)
{
	if (!pSMEHeader)
		return false;

	pSMEHeader->ref = NULL;

	FILE	*fp = fopen(szFileName,"rb");
	if(!fp)
		return false;
	
	trSMEImage	*pImage = (trSMEImage*)malloc(sizeof(trSMEImage));

	// read in the header
	fread(&pImage->sFileHeader,sizeof(FileHeader),1,fp);

	pSMEHeader->depth = pImage->sFileHeader.depth;
	pSMEHeader->height = pImage->sFileHeader.rows;
	pSMEHeader->width = pImage->sFileHeader.cols;
	pSMEHeader->mode = (teSMEMode)pImage->sFileHeader.mode;
	pSMEHeader->panes = pImage->sFileHeader.planes;
	pSMEHeader->depth = pImage->sFileHeader.depth;

	if (pSMEHeader->panes > 32)
		return false;
	
	pSMEHeader->ref = pImage;

	// read in any mac resource shit
	if ( pImage->sFileHeader.resourceLength > 0 ) // mac resource shit ( icons and stuff )
	{
		pImage->pHeaderResourceData = malloc(pImage->sFileHeader.resourceLength);
		fread(pImage->pHeaderResourceData,pImage->sFileHeader.resourceLength,1,fp);
	}

	if (pSMEHeader->mode == eSMEModeIndexedColor) // gets the table data
		fread(pImage->sPalletData,256*3,1,fp);

	char*	pPanePtr;
	char*	pRowPtr;
	int		i = 0;
	for (i = 0; i < pSMEHeader->panes; i ++)
	{
		pPanePtr = (char*)malloc(pSMEHeader->width * pSMEHeader->height * (pSMEHeader->depth/8) );
		memset(pPanePtr,0,pSMEHeader->width * pSMEHeader->height * (pSMEHeader->depth/8));
		pImage->aPanes[i] = pPanePtr;
	}

	unsigned int iRowBites = pSMEHeader->width * (pSMEHeader->depth/8);		// read it in backwards
	for (i = 0; i < pSMEHeader->panes; i ++)
	{
		pPanePtr = (char*)pImage->aPanes[i];
		for (int r = pSMEHeader->height-1; r >=0; r--)
		{
			pRowPtr = pPanePtr + (iRowBites * r);
			fread(pRowPtr,iRowBites,1,fp);
		}
	}

	fclose(fp);
	return true;
}

bool ReleaseSME(trSMEHeaderInfo *pSMEHeader)
{
	if (!pSMEHeader)
		return false;

	trSMEImage	*Pointer = (trSMEImage*)(pSMEHeader->ref);

	if (!Pointer)
		return false;

	if (Pointer->pHeaderResourceData)
		free(Pointer->pHeaderResourceData);

	for (int i = 0; i < pSMEHeader->panes; i ++)
		free(Pointer->aPanes[i]);

	free(Pointer);
	pSMEHeader->ref = NULL;
	return true;
}

bool GetSME8BitData(trSMEHeaderInfo *pSMEHeader, trSME8BitData *pData, bool bInterlaced)
{
	if (!pSMEHeader || !pData)
		return false;

	trSMEImage	*pImage = (trSMEImage*)(pSMEHeader->ref);

	if (!pImage)
		return false;

	pData->size = (pSMEHeader->depth/8) * pSMEHeader->height * pSMEHeader->width * pSMEHeader->panes;
	pData->data = (unsigned char*)malloc(pData->size);

	unsigned char	*pPixelPtr = pData->data;
	unsigned char	*pPanePtr;
	unsigned int	iRowBites = pSMEHeader->width * (pSMEHeader->depth/8);

	if (!bInterlaced)
	{
		for (int i = 0; i < pSMEHeader->panes; i ++)
		{
			pPanePtr = (unsigned char*)pImage->aPanes[i];
			for (int r = 0; r < pSMEHeader->width; r++)
			{
				memcpy(pPixelPtr,pPanePtr,iRowBites);
				pPanePtr += iRowBites;
				pPixelPtr += iRowBites;
			}
		}
	}
	else //interlaced
	{
		for (int y = 0; y < pSMEHeader->height; y++)
		{
			for (int x = 0; x < pSMEHeader->width; x++)
			{
				for (int i = 0; i < pSMEHeader->panes; i ++)
				{	
					pPanePtr = (unsigned char*)pImage->aPanes[i];
					*pPixelPtr = (unsigned char)pPanePtr[(y*iRowBites)+x];
					pPixelPtr++;
				}
			}
		}
	}
	return true;
}

bool GetSME16BitData(trSMEHeaderInfo *pSMEHeader, trSME16BitData *pData, bool bInterlaced)
{
	if (!pSMEHeader || !pData)
		return false;

	trSMEImage	*pImage = (trSMEImage*)(pSMEHeader->ref);

	if (!pImage)
		return false;

	pData->size = (pSMEHeader->depth/8) * pSMEHeader->height * pSMEHeader->width * pSMEHeader->panes;
	pData->data = (unsigned short*)malloc(pData->size);

	unsigned short	*pPixelPtr = pData->data;
	unsigned short	*pPanePtr;
	unsigned int	iRowBites = pSMEHeader->width * (pSMEHeader->depth/8);

	if (!bInterlaced)
	{
		for (int i = 0; i < pSMEHeader->panes; i ++)
		{
			pPanePtr = (unsigned short*)pImage->aPanes[i];
			for (int r = 0; r < pSMEHeader->width ; r++)
			{
				memcpy(pPixelPtr,pPanePtr,iRowBites);
				pPanePtr += iRowBites;
				pPixelPtr += iRowBites;
			}
		}
	}
	else //interlaced
	{
		for (int y = 0; y < pSMEHeader->height; y++)
		{
			for (int x = 0; x < pSMEHeader->width; x++)
			{
				for (int i = 0; i < pSMEHeader->panes; i ++)
				{	
					pPanePtr = (unsigned short*)pImage->aPanes[i];
					*pPixelPtr = (unsigned short)pPanePtr[(y*iRowBites)+x];
					pPixelPtr++;
				}
			}
		}
	}
	return true;
}



