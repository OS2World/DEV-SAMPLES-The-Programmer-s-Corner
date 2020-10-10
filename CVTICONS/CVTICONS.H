#define NONE    0
#define EXTRACT 1
#define TO_PM   2
#define TO_WIN  3
#define DISPLAY 4

short main(short nArgc, char **szArgv);
void  DisplayUsage(void);
void  Fatal(char *szMesg);
void  ExtractFileInfo(void);
void  ConvertToPM(void);
void  ConvertToWin(void);
BYTE  GetFileType(OLDEXEHDR *Hdr);
void  ProcessResources(DWORD dwOfs, WORD wSize);
void  GetResourceNames(long lOfs);
void  ProcessIcons(WORD wEntries,  WORD wShiftCount, RSRCENTRY *re);
void  WriteIconDIB(PBITMAPINFOHEADER pRscr);
WORD  DIBNumColors(WORD wClrUsed, WORD wBitCount);
void  GetFileName(char * FullName);
