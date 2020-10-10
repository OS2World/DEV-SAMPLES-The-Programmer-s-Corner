#define DOS			1
#define WIN			2
#define ICO			3

#define RS_CURSOR		1
#define RS_BITMAP		2
#define RS_ICON			3
#define RS_MENU			4
#define RS_DIALOG		5
#define RS_STRING		6
#define RS_FONTDIR		7
#define RS_FONT			8
#define RS_ACCELERATOR		9
#define RS_RESDATA		0xA
#define RS_ICONDIR		0xE
#define RS_RESNAMES 		0xF

#define CODE_DATA	    	0x0007
#define ITERATION	    	0x0008
#define MOVE_FIX	    	0x0010
#define PURITY		    	0x0020
#define LOADING 	    	0x0040
#define READ_WRITE_PRIV	    	0x0080
#define RELOCATIONS	    	0x0100
#define CONFORMITY	    	0x2000
#define PRIVLEGES	    	0xC000
#define DISCARDABLE	    	0x1000
#define CODESEG 	    	0x2000
#define SIZESEG 	    	0x4000

#define DGROUP		    	0x0002
#define INIT		    	0x0007
#define OPMODE		    	0x0008
#define ONLY8086	    	0x0010
#define HAS286		    	0x0020
#define HAS386		    	0x0040
#define HASFLOAT	    	0x0080
#define APPTYPE 	    	0x0700
#define FAMILYAPP	    	0x0800
#define VALID		    	0x2000
#define APP_DLL 	    	0x8000

#define RC_RSRVD1	    	0x000F
#define RC_FIX_MOV	    	0x0010
#define RC_PURITY	    	0x0020
#define RC_LOADING	    	0x0040
#define RC_RSRVD2	    	0x0F80
#define RC_DISCARD	    	0xF000

#define SEG_MOVABLE	    	0xFF
#define SEG_EXPORT	    	0x01
#define SEG_DATA	    	0x02
#define SEG_PARMS	    	0xF8

typedef struct tagOLDEXEHDR
{
  BYTE	bySigByte1,	/* Signature byte 1 'M'				    */
	bySigByte2;	/* Signature byte 2 'Z'                             */
  WORD	wModFilLen,	/* Length of file header and program image MOD 512  */
	wPagFilLen,	/* Number of 512-byte pages of header and prog image*/
	wRelocItems,	/* Number of relocation table items for MS-DOS stub */
	wHeaderSize,	/* size of old EXE header in 16-byte paragraphs     */
	wMinMem,	/* Minimum # of paragraphs of mem needed to run stub*/
	wMaxMem,	/* Maximum # of paragraphs of mem needed to run stub*/
	wWinStubSS,	/* Segement displacement of stub program stack      */
	wWinStubSP,	/* Contents of SP register in stub program entry    */
	wWinStubCRC,	/* Checksum of stub program                         */
	wWinStubIP,	/* Contents of IP register at stub program entry    */
	wWinStubCS,	/* Segment displacement of stub program code        */
	wRelocOfs,	/* 0040h if NEWEXE, offset to first stub prog reloc */
	wOverlayNbr,	/* Overlay number '0'                               */
	wMemoryNeeded,  /*						    */
	wReserved1[3],
	wOEMId, 	/* OEM Identifier                                   */
	wOEMInfo,	/* OEM information                                  */
	wReserved2[10];
  DWORD	dwNewExeOfs;	/* File offset of new EXE Header                    */
 } OLDEXEHDR;

typedef struct tagNEWEXEHDR
{
  BYTE	bySigByte1,	/* Signature byte 1 'N'                             */
	bySigByte2,	/* Signature byte 2 'E'                             */
	byLinkMajVer,	/* Linker major version number                      */
	byLinkMinVer;	/* Linker minor version number                      */
  WORD	wEntryOfs,	/* Offset of entry table relative to start of new ex*/
	wEntrySize;	/* Length of entry table                            */
  DWORD dwFileLoadCRC;	/* 32-bit Checksum of entire file                   */
  WORD	wModuleFlags,	/* Module Flags                                     */
	wAutoDataSeg,	/* Segment Number of DGROUP                         */
	wHeap,		/* Initial Heap Size                                */
	wStack,		/* Initial Stack Size                               */
	wProgIP,	/* Contents of IP register at program entry         */
	wProgCS,	/* Segment number containing entry point            */
	wProgSP,	/* Contents of SP register at program entry         */
	wProgSS,	/* Segment number containg stack                    */
	wSegEntries,	/* Entries in segment table                         */
	wResNameSize,	/* Entries in module reference table                */
	wNonResNameSize,/* Length of NonResident names table                */
	wSegOfs,	/* Offset of Segment Table relative to new EXE hdr  */
	wResourceOfs,	/* Offset of Resource Table relative to new EXE hdr */
	wResidentOfs,	/* Offset of Resident Names Table rel to new EXE hdr*/
	wModuleOfs,	/* Offset of Module refernce table rel to new EXE hd*/
	wImportNameOfs; /* Offset of Imported names table rel to new EXE hdr*/
  DWORD dwNonResNameOfs;/* Offset of NonResident names table rel start of fi*/
  WORD	wMoveEntryPtCnt,/* Number of moveable entry points                  */
	wLogSectorAlign,/* Size of file alignment unit (power of 2)         */
	wResourceSegs;	/* Number of resource table entries		   */
  BYTE	byTargetSystem,	/*  Target operating
				  0: UnKnown	   1: OS/2
				  2: Windows	   3: MS-DOS 4
			*/
	byBytesLeft;	/* Number of Bytes that are valid in rsrvd following*/
  WORD	wReserved[4];
} NEWEXEHDR;
/*********************** Module Flags Bit Significance **********************
  Bit(s)		Significance
  0 - 1 		DGROUP characteristics
			    0 : no DGROUP
			    1 : single DGROUP (shared)
			    2 : multiple DGROUPs (unshared)
  2			0 : Global Initilization (DLLs)
			1 : Instance Initilization
  3			0 : Real or Protected Mode
			1 : Protected Mode Only
  4			1 : Only contains 8086 instructions
  5			1 : contains 80286 instructions
  6			1 : contains 80386 instructions
  7			1 : contains floating-point instructions
  8 - 10		Application type
			    0 : UnKnown
			    1 : Full Screen
			    2 : Vio App, can run in a window
			    3 : Presntation Manager / Windows
  11			0 : Not a Family Application
			1 : Family Application
  12			RESERVED
  13			0 : File is executable
			1 : invalid load module (errors in linking)
  14			RESERVED
  15			0 : Application
			1 : DLL or Driver
****************************************************************************/
typedef struct tagSEGTABLE
{
  WORD wSectorOfs,	/* Offset of beginning of segment within file
			    Expressed as multiple of file unit size        */
       wSegmentLen,	/* Length of segment ( 0 = 65,536 )                */
       wSegFlags,	/* Segment Flags
			    Bit(s)	Significance
			    0 - 2	Segment Type
					    0 : Code
					    1 : Data
			    3		0 : Noniterated Data
					1 : Iterated Data
			    4		0 : Fixed
					1 : Moveable
			    5		0 : Impure or Nonshareable
					1 : Pure or Shareable
			    6		0 : Load-on-Call
					1 : Preload
			    7		0 : Execute/read if code, r/w if data
					1 : Execute-only if code, read if data
			    8		0 : No relocation table
					1 : Relocation table present
			    9		0 : Nonconforming
					1 : Conforming
			    10 - 11	Privilege level
			    12		0 : Nondiscardable
					1 : discardable
			    13		0 : 16-bit code segment
					1 : 32-bit code segment
			    14		0 : normal segment
					1 : huge segment
			    15		RESERVED
			*/
       wAllocSize;	/* Minimum allocation size for segment (0 = 65,536 )*/
} SEGTABLE;

typedef struct tagRSRCTBL
{
  WORD wShiftCount,	/* power of 2 which determines resource unit size   */
       wType,		/* Resource Type                                    */
       wFollowing,	/* Number of Resources follwing                     */
       wReserved;
} RSRCTBL;
/**********************     Resource Types    ******************************
	1	--  Cursor
	2	--  Bitmap
	3	--  Icon
	4       --  Menu
	5	--  Dialog
	6	--  String
	7	--  Font Dir
	8	--  Font
	9	--  Accelerator
	A	--  Resource Data
	B       --
	C	--
	D	--
	E	-- ICO file header / directory entry
	F	-- Resource Name
****************************************************************************/

typedef struct tagRSRCENTRY
{
  WORD  wReserved1,
	wOffset,    /* Offset to resource from start of file		   */
	wLength,    /* Length of resource			           */
	wRsrcFlags, /* Resource flags
			Bit(s)	    Significance
			0 - 3	    RESERVED
			4	    0 : Fixed
				    1 : Moveable
			5	    0 : Impure or Nonshareable
				    1 : Pure or Shareable
			6	    0 : Load-on-Call
				    1 : Preload
			7  - 11	    RESERVED
			12 - 15     Discard Priority
		     */
	wRsrcNum,   /* Resource Number if bit 15 set offset to string      */
	wReserved2;
} RSRCENTRY;
typedef RSRCENTRY     *PRSRCENTRY;
typedef RSRCENTRY far *LPRSRCENTRY;

typedef struct tagMOVSEG
{
  BYTE byEntryFlags;	/*  Bit(s)     Significancs
			    0          0 : Entry point not exported
				       1 : Entry point Exported
			    1          0 : Entry Point uses Instance data
				       1 : Entry Point uses Single data
			    2          RESERVED
			    3 - 7      Number of stack parameter words
			*/
  WORD w3FIns;          /*  Int 3FH instruction (CDH 3FH)       */
  BYTE bySegNbr;        /*  Segment number of entry point       */
  WORD wSegOfs;		/* Offset of entry point within segment */
} MOVSEG;

typedef struct tagFIXSEG
{
  BYTE byEntryFlags;  /* Entry point flags same as above      */
  WORD wSegOfs;	      /* Offset of entry point within segment */
} FIXSEG;

typedef struct tagRESNAMES
{
  WORD wLen,
       wType;
} RESNAMES;
typedef RESNAMES *PRESNAMES;