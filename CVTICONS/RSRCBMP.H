/* Bitmap Header Definition */
typedef struct tagBITMAP
  {
    int 	bmType;
    int 	bmWidth;
    int 	bmHeight;
    int 	bmWidthBytes;
    BYTE	bmPlanes;
    BYTE	bmBitsPixel;
    LPSTR	bmBits;
  } BITMAP;
typedef BITMAP	*PBITMAP;

typedef struct tagRGBQUAD {
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER{
  	DWORD	   biSize;
  	DWORD	   biWidth;
  	DWORD	   biHeight;
  	WORD	   biPlanes;
  	WORD	   biBitCount;
	DWORD	   biCompression;
	DWORD	   biSizeImage;
	DWORD	   biXPelsPerMeter;
	DWORD	   biYPelsPerMeter;
	DWORD	   biClrUsed;
	DWORD	   biClrImportant;
} BITMAPINFOHEADER;

typedef BITMAPINFOHEADER *PBITMAPINFOHEADER;

/* constants for the biCompression field */
#define BI_RGB      0L
#define BI_RLE8     1L
#define BI_RLE4     2L

typedef struct tagBITMAPINFO { 
    BITMAPINFOHEADER	bmiHeader;
    RGBQUAD		bmiColors[1];
} BITMAPINFO;
typedef BITMAPINFO *PBITMAPINFO;

typedef struct tagBITMAPFILEHEADER {
	WORD	bfType;
	DWORD	bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
	DWORD	bfOffBits;
} BITMAPFILEHEADER;
typedef BITMAPFILEHEADER *PBITMAPFILEHEADER;
