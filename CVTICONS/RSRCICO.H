/* Icon file Definition */
typedef struct tagICOHDR
{
  WORD wReserved,
       wType,
       wCount;
} ICOHDR;
typedef ICOHDR	   *PICOHDR;
typedef ICOHDR far *LPICOHDR;

typedef struct tagICODIR
{
  BYTE	byWidth,
	byHeight,
	byColors,
	byReserved1;
  WORD	wReserved2,
	wReserved3;
  DWORD dwDIBSize,
	dwDIBOfs;
} ICODIR;
typedef ICODIR     *PICODIR;
typedef ICODIR far *LPICODIR;

typedef struct tagEXEICODIR
{
  BYTE	byWidth,
	byHeight,
	byColors,
	byReserved1;
  WORD	wBitCount,
	wPlanes;
  DWORD dwDIBSize;
  WORD  wDIBOfs;
} EXEICODIR;

typedef struct tagOLDICOBMH
{
  BYTE byType,
       byFormat;
  WORD wWidth,
       wHeight,
       wSizeBM1,
       wSizeBM2,
       wSizeMult;
} OLDICOBMH;
