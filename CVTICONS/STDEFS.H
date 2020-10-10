#define UNKNOWN_FORMAT		1
#define OPEN_FILE		2
#define OUT_OF_MEM		3
#define RESOURCE_ERR		4

#define MSG_SIZE		128
#define TMP_SIZE		20

#define FALSE		    	0
#define TRUE		    	1

#define FAR		    	far
#define NEAR		    	near
#define LONG		    	long
#define VOID		    	void
#define PASCAL		    	pascal

#ifndef max
#define max(a,b)            	(((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            	(((a) < (b)) ? (a) : (b))
#endif

#define LOWORD(l)	    	((WORD)(l))
#define HIWORD(l)	    	((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)	   	((BYTE)(w))
#define HIBYTE(w)	    	((BYTE)(((WORD)(w) >> 8) & 0xFF))

typedef int		    	BOOL;
typedef unsigned char	    	BYTE;
typedef unsigned short	    	WORD;
typedef unsigned long	    	DWORD;
typedef char near	    	*PSTR;
typedef char near	    	*NPSTR;
typedef char far	    	*LPSTR;
typedef BYTE near	    	*PBYTE;
typedef BYTE far	    	*LPBYTE;
typedef short near	    	*PINT;
typedef short far 	    	*LPINT;
typedef WORD near	    	*PWORD;
typedef WORD far	    	*LPWORD;
typedef long near	    	*PLONG;
typedef long far	    	*LPLONG;
typedef DWORD near	    	*PDWORD;
typedef DWORD far	        *LPDWORD;
typedef void far	        *LPVOID;

typedef struct tagRECT
{
  int 	left;
  int 	top;
  int 	right;
  int 	bottom;
} RECT;

typedef struct tagPOINT
{
  int 	x;
  int 	y;
} POINT;
