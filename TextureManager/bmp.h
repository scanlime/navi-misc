#ifndef __BITMAP_H__
#define __BITMAP_H__

#ifndef WIN32
#define HANDLE_PRAGMA_PACK_PUSH_POP 1
#pragma pack(push, 2)

typedef struct 
{
	unsigned short			bfType;
	unsigned long			bfSize;
	unsigned short			bfReserved1;
	unsigned short			bfReserved2;
	unsigned long			bfOffBits;
} BITMAPFILEHEADER;

typedef struct
{
        unsigned long			biSize;
        long				biWidth;
        long				biHeight;
        unsigned short			biPlanes;
        unsigned short			biBitCount;
        unsigned long			biCompression;
        unsigned long			biSizeImage;
        long				biXPelsPerMeter;
        long				biYPelsPerMeter;
        unsigned long			biClrUsed;
        unsigned long			biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop)

#endif // WIN32

#endif
