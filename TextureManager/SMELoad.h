typedef enum
{
	eSMEModeBitmap,			// 1 bit image
	eSMEModeGrayScale,		// 8 or 16 bit grayscale
	eSMEModeIndexedColor,	// 8 bit palleted image
	eSMEModeRGBColor,		// 8 or 16 bit RGB (A)
	eSMEModeCMYKColor,		// 8 or 16 bit CYMK (A)
	eSMEModeHSLColor,		// Other colorspaces from photoshop
	eSMEModeHSBColor,
	eSMEModeMultichannel,
	eSMEModeDuotone,
	eSMEModeLabColor,
	eSMEModeGray16,
	eSMEModeRGB48,
	eSMEModeLab48,
	eSMEModeCMYK64,
	eSMEModeDeepMultichannel,
	eSMEModeDuotone16
}teSMEMode;

typedef struct
{
	int			width;		// x dimension in pixels
	int			height;		// y dimension in pixels
	int			panes;		// number of image panes ( AKA channels, 3 for RGB images)
	int			depth;		// bit depth of each channel ( usualy 8 )
	teSMEMode	mode;		// colorspace mode of data ( see above )
	void		*ref;		// internal refrence to image data used by reader and free functions
}trSMEHeaderInfo;

// image data stuctures for 16 and 8 bit image channels
typedef struct
{
	unsigned char	*data;
	unsigned int	size;
}trSME8BitData;

typedef struct
{
	unsigned short	*data;
	unsigned int	size;
}trSME16BitData;

// reads in a SME and loads up it's header info
bool LoadSME(const char* szFileName, trSMEHeaderInfo *pSMEHeader);

// frees all internal memory for a image ( but not for any trSMExxBitData structures that were passed in )
bool ReleaseSME(trSMEHeaderInfo *pSMEHeader);

// Retreves the image data from a image refrenced by it's header
// and coies it into the pData pointer
// Will allocate memory in the "data" member of the pointer.
// Caller is respodible for freeing this memory 
// bInterlaced = true will give data in the format of:
//		RGBA RGBA
// bInterlaced false will give the data like this:
//		RRRRR GGGGG BBBBB AAAAA

bool GetSME8BitData(trSMEHeaderInfo *pSMEHeader, trSME8BitData *pData, bool bInterlaced = true);
bool GetSME16BitData(trSMEHeaderInfo *pSMEHeader, trSME16BitData *pData, bool bInterlaced = true);





