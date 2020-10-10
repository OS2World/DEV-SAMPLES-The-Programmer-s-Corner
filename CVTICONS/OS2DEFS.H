
typedef struct _BITMAPINFOHEADER {      /* bmp */
    DWORD  cbFix;
    WORD   cx;
    WORD   cy;
    WORD   cPlanes;
    WORD   cBitCount;
} OS2_BITMAPINFOHEADER;

typedef struct _BITMAPFILEHEADER { /* bfh */
	WORD	  usType;
	DWORD	  cbSize;
	short	  xHotspot;
	short	  yHotspot;
	DWORD	  offBits;
	OS2_BITMAPINFOHEADER bmp;
} OS2_BITMAPFILEHEADER;

        /* This is the 1.2 device independent format header */

typedef struct _BITMAPARRAYFILEHEADER { /* bafh */
	WORD	  usType;
	DWORD	  cbSize;
	DWORD	  offNext;
	WORD	  cxDisplay;
	WORD	  cyDisplay;
	OS2_BITMAPFILEHEADER bfh;
} BITMAPARRAYFILEHEADER;

/*
 * These are the identifying values that go in the usType field of the
 * BITMAPFILEHEADER and BITMAPARRAYFILEHEADER. (BFT_ => Bit map File Type)
 */

#define BFT_ICON		0x4349	/* 'IC' */
#define BFT_BMAP		0x4d42	/* 'BM' */
#define BFT_POINTER		0x5450	/* 'PT' */
#define BFT_COLORICON		0x4943	/* 'CI' */
#define BFT_COLORPOINTER	0x5043	/* 'CP' */
#define BFT_BITMAPARRAY 	0x4142	/* 'BA' */

/* RGB data for _BITMAPINFO struct */
typedef struct _RGB {           /* rgb */
    BYTE bBlue;
    BYTE bGreen;
    BYTE bRed;
} RGB;