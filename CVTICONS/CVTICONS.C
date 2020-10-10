/*****************************************************************************

  CvtIcons.exe Copyright @ 1991 Jay Giganti All Rights Reserved

              All Options are Mutually Exclusive

         x : Extracts Icons from Windows ICO file(s)
         w : Converts ICO files from PM to Windows
         p : Converts ICO files from Windows to PM
         ? : Displays this screen

        Usage CVTICONS -option File (Wild Cards Accepted)

        examples cvticons -x c:\windows\*.*
                 cvticons -p *.ico

        example 1 : will extract all icons from the files in c:\windows
                    directory and will create them in your current directory

        example 2 : will convert all ICO files in your current directory
                    into ICP files in the PM format in your current directory
        
This program currently does three things.
Extracts Icons:
  It will extract Icons from windows EXE and DLL files.  It scans through
  these files and will create a file with and extension of .ICO based upon
  2 things.  First it will look in the resource table for any Names, if it
  finds a name it will use up to 8 characters of it to create the file
  name.  If There were no names found in the resource table, or there were
  more icons than names, (ie -- several Icons in one directory entry) then
  the icon name will containg the first 5 characters if the program it is
  stripping it from, and then append a 3 digit number,m thus you can process
  a file with up 1000 icons in it.  
  side note:  I stripped a file form the MS-DOS program  under windows and
              found a 64x64 OS/2 1.1 icon format.

Converts Icons To PM Format:
  This will translate an ICO file into a PM format.  It creates the file
  with an .ICP extension.  Currently I will only copy one Icon from each ICO
  file, since this is how I write them.  If Needed I will add multiple
  icons per ICO file handling.  The format for the icon will be written
  in its current dimensions, ie: a 32x32 stays 32x32. Under OS/2 1.2 it
  seems that if they are not 64x64 then it is not in a device independent (DI)
  format, thus when loaded into the Icon Editor, you are told the It has
  created on for you, if you save the file, then this 64x64 DI format will
  be written to disk along with the origional 32x32 format.  I have the Icon
  Editor do all translation.  In the furture I might do the streching required
  to create a 64x64 bitmap and thus the resulting PM file will be smaller 
  and the icon editor won't complain.  The reason it would be smaller is 
  because there would only be one format stored in it.

Converts Icons To Windows 3.0 Format:
  This will translate a PM Icon file into a Windows format.  It will translate
  all Icons in the file, as long as they are <= 32x32.  I might add the
  functionality to squeeze 64x64 to 32x32 but the Icon edititor does a good
  job.  Thus if you have a 64x64 then select a VGA type and it will be 
  converted to a 32x32 format which I can read and save in a file.  The
  file will be named with the filename and the extensions .ICW. If there
  is more than one Icon which is <= 32x32 then the next filename will be
  created with the first 6 characters of the filename plus a 2 digit number
  and then .ICW appended to it.

  In the future I will port this to a Windows and PM app but this does the
  job for now.  It is a dos program.  Also I will probably add the ability
  to read and write multiple icons per ICO file, and the ability to extract
  icons from windows files and convert them directly to PM format.  Currently
  you have to extract and then convert.  I also will probably add the ability
  to extract Icons from OS/2 files.  Until then I hope this utility helps.

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <conio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "stdefs.h"
#include "newexe.h"
#include "rsrcico.h"
#include "rsrcbmp.h"
#include "cvticons.h"
#include "os2defs.h"

extern int  GetOpt(int nArgc, char **szArgv, char *szOpts);
extern int  gnOptInd;
extern char *gszOptArg;

                /* Global Variables */
static short gfhInFile;
static short gfhOutFile;
static char  gIconNames[1000][9];
static WORD  gwNames;
static char  gszFileName[13];
/***************************************************************************
   Main -- Entry point
****************************************************************************/
short main(short nArgc, char **szArgv)
{
  int         nCntr;
  BYTE        Flag = NONE;
  char        *Name;
  
  if (nArgc < 2)
    DisplayUsage();
  
  nCntr = GetOpt(nArgc, szArgv, "?pwx");
  switch(nCntr)
  {
    case 'x' :
      Flag = EXTRACT;
      break;

    case 'p' :
      Flag = TO_PM;
      break;
           
    case 'w' :
      Flag = TO_WIN;
      break;
    
    case '?' :
      DisplayUsage();
  }     
  if (Flag == NONE)
  {
    DisplayUsage();
    return 0;
  }
  for(nCntr = 0; gnOptInd < nArgc; ++gnOptInd)
  {    
    if (nCntr++ == 23)
    {
      printf("\t\t\t==> Hit Any Key To Continue <==");
      (void) getche();
      printf("\n\n");
      nCntr = 0;
    }    
    gfhInFile = open(szArgv[gnOptInd], O_BINARY | O_RDONLY); 
    if (gfhInFile == -1)
    {
      printf("Could Not Open File\n");
      continue;
    }
    Name = GetFileName(szArgv[gnOptInd]);
    strcpy(gszFileName, Name);
    printf("%-12s : ", gszFileName);
    switch (Flag)
    {
      case EXTRACT:
        ExtractFileInfo();
        break;
        
      case TO_PM:
        ConvertToPM();
        break;
        
      default:
        ConvertToWin();
        break;
    }
    close(gfhInFile);
  }
  return 0;
}
/***************************************************************************/
/* Get File Name : Gets Just the Filename -- Strips Drive and Directory    */
/***************************************************************************/
void GetFileName(char *FullName)
{
  char *pCh;
  
  pCh = FullName;
  while (*pCh != '\0')
    ++pCh;
  --pCh;           
                    /* Locate Start Of File Name */
  while (pCh > FullName && *pCh != '\\' && *pCh != ':' && *pCh != '/')
    --pCh;
  if (pCh > FullName)
    ++pCh;         /* Move To Right Of Pathname Separator */

  return pCh;
}

/***************************************************************************/
/* Fatal -- issues a diagnostic message and terminates			   */
/***************************************************************************/
void Fatal(char *szMesg)
{
  strcat(szMesg, "\n");
  fputs("Error : ", stderr);
  fputs(szMesg, stderr);
  exit(2);
}
/***************************************************************************/
/***************************************************************************/
void DisplayUsage(void)
{
  printf("\n\n\n\t      -- CvtIcons : Copyright @ 1991 Jay Giganti --\n\n");
  printf("\tExtracts Icons from EXE & DLL Window Files Creating ICO files\n");
  printf("\tConverts Icons between Windows and Program Manager formats\n");
  printf("\tCreates the ICO file name from entry in the resource table\n");
  printf("\n\tCurrently Only 1 Icon per WIN ICO File will be converted\n");
  printf("\tWhen Converting, the new file will be created with an\n");
  printf("\tExtension of either ICP or ICW; depending on option\n");
  printf("\tAn Icons dimensions, in PM format, must be 32x32 or less\n\n");
  printf("\n\t\t    All Options are Mutually Exclusive\n\n");
  printf("\t\tx : Extracts Icons from Windows ICO file(s)\n");
  printf("\t\tw : Converts ICO files from PM to Windows\n");
  printf("\t\tp : Converts ICO files from Windows to PM\n");
  printf("\t\t? : Displays this screen\n");
  printf("\n\t      Usage CVTICONS -option File (Wild Cards Accepted)\n");
  exit(1);
}
/***************************************************************************/
/***************************************************************************/
void ExtractFileInfo(void)
{
  OLDEXEHDR OldHdr;
  NEWEXEHDR NewHdr;
  BYTE      byBits;
  
  switch(byBits = GetFileType(&OldHdr))
  {      
    case DOS:
      printf("Is A Non Windows Application\n");
      return;

    case WIN:
      lseek(gfhInFile, OldHdr.dwNewExeOfs, SEEK_SET);
      read(gfhInFile, (PSTR) &NewHdr, sizeof(NewHdr));
      if ((NewHdr.bySigByte1 != 'N') || (NewHdr.bySigByte2 != 'E'))
      {
        printf("Has An Improper Signature for a Windows App\n");
        return;
      }
      ProcessResources(NewHdr.wResourceOfs + OldHdr.dwNewExeOfs,
	               NewHdr.wResidentOfs - NewHdr.wResourceOfs);
      break;          

    default :
      printf("Is An Unknown file format\n");
      return;
  }
}
/***************************************************************************/
/***************************************************************************/
BYTE GetFileType(OLDEXEHDR *Hdr)
{
  read(gfhInFile, (PSTR)Hdr, sizeof *(Hdr));
  if((Hdr->bySigByte1 != 'M') || (Hdr->bySigByte2 != 'Z'))
    return 0;
  if(Hdr->wRelocOfs == 0x40)
    return WIN;
  else
    return DOS;
}
/***************************************************************************/
/***************************************************************************/
void ProcessResources(DWORD dwOfs, WORD wSize)
{
  RSRCTBL    rt;
  WORD       wShiftCount;
  WORD       wBits;
  RSRCENTRY  *re;
  long	     lOfs;
  BYTE       bHasIcons;
  
  bHasIcons = FALSE;
  if (wSize)
  {
    lseek(gfhInFile, dwOfs, SEEK_SET);
    read(gfhInFile, &rt, sizeof(rt));
    wShiftCount = rt.wShiftCount;
    while (rt.wType != 0)
    {      
      if ((re =(RSRCENTRY *)malloc(rt.wFollowing*sizeof(RSRCENTRY))) == NULL)
	Fatal("Unable to allocate memory for Resource Entries");
      read(gfhInFile, re, sizeof(RSRCENTRY) * rt.wFollowing);
      wBits = rt.wType & 0x7FFF;
      if (wBits == RS_ICON || wBits == RS_RESNAMES)
      {
        lOfs = tell(gfhInFile);
        if (wBits == RS_ICON)
        {
          bHasIcons = TRUE;
          if (rt.wFollowing > 1)
            printf("Processing %u Icons\n", rt.wFollowing);
          else
            printf("Processing %u Icon\n", rt.wFollowing);          
          ProcessIcons(rt.wFollowing, wShiftCount, re);
        }
        else
        {
          GetResourceNames((long)((long)re->wOffset << wShiftCount));
          if(gwNames == 1)
            printf("Located 1 Name - ");
          if(gwNames > 1)
            printf("Located %d Names - ", gwNames);
        }
        lseek(gfhInFile, lOfs, SEEK_SET);
      }
      free(re);
      read(gfhInFile, &rt, sizeof(rt));
    }
    if (!bHasIcons)
      printf("Does Not Contain Any Icons\n");
  }
  else
    printf("Does Not Contain Any Resources\n");
}
/***************************************************************************/
/***************************************************************************/
void ProcessIcons(WORD wEntries,  WORD wShiftCount, RSRCENTRY *re)
{ 
  ICOHDR    icoHdr;
  char      szFileName[13];
  char      szBuf[4];
  RSRCENTRY *pre;
  PSTR      pRsrc;
  long      lOfs;  
  WORD      wSize;
  WORD      wCntr;
  char      *pExt;
  int       nEnd;
    
  icoHdr.wReserved = 0x0000;
  icoHdr.wType     = 0x0001;
  icoHdr.wCount    = 0x0001;
  wSize            = sizeof(BITMAPINFOHEADER);  
  if ((pRsrc = (PSTR)malloc(wSize)) == NULL)
    Fatal("Unable to allocate memory for Resource Entry");
    
  for (wCntr = 0; wCntr < wEntries; wCntr++)
  {
    pre  = (re + wCntr);                     /*Set to curent Resource entry */
    lOfs = (long)pre->wOffset << wShiftCount;/*Calculate offset to seek to  */
    lseek(gfhInFile, lOfs,  SEEK_SET);  
    read(gfhInFile,  pRsrc, wSize);
    if (wCntr >= gwNames)
    {
      pExt = strrchr(gszFileName, '.');
      nEnd = strlen(gszFileName) - strlen(pExt);
      if (nEnd > 5)
        nEnd = 5;
      strncpy(szFileName, gszFileName, nEnd);
      szFileName[nEnd] = '\0';
      sprintf(szBuf, "%.3d", wCntr);
      strcat(szFileName, szBuf);
    }
    else
      strcpy(szFileName, gIconNames[wCntr]);
    strcat(szFileName, ".ICO");
    if ((gfhOutFile= open(szFileName, O_BINARY | O_CREAT | O_TRUNC | O_WRONLY,
         S_IREAD  | S_IWRITE)) == -1)
    {
      printf("Could Not Create file %s\n", szFileName);
      Fatal("File IO Error");
    }
    write(gfhOutFile, (PSTR)&icoHdr, sizeof(icoHdr));
    WriteIconDIB((PBITMAPINFOHEADER)pRsrc);
    close(gfhOutFile);
  }
  free(pRsrc);
}
/***************************************************************************/
/***************************************************************************/
void GetResourceNames(long lOfs)
{
  RESNAMES ResNames;
  char     Buff[3];
  WORD     wNbr,
           wLen,
           wCntr = 0;
  char     Name[100];
  
  lseek(gfhInFile, lOfs, SEEK_SET);
  gwNames = 0;
  read(gfhInFile, &ResNames, sizeof(ResNames));
  while (ResNames.wLen)
  {
    wCntr++;
    ResNames.wLen -= 8;
    read(gfhInFile, Buff, 2);
    wNbr = *(WORD *)Buff;
    if (!(ResNames.wType & 0x8000))
      read(gfhInFile, Buff, 1);
    read(gfhInFile, &Name, ResNames.wLen + 1);
    if ((ResNames.wType & 0x7FFF) == RS_ICONDIR)
    { 
      wLen = strlen(Name);
      if (wLen > 8)
        wLen = 8;
      strncpy(gIconNames[gwNames], Name, wLen);
      gIconNames[gwNames][wLen] = '\0';
      if (gwNames++ == 1000)
        Fatal("To Many Icons In This File");
    }    
    read(gfhInFile, &ResNames, sizeof (RESNAMES));
  }
}
/***************************************************************************/
/***************************************************************************/
void WriteIconDIB(PBITMAPINFOHEADER pRsrc)
{
  RGBQUAD  *pRgb;
  WORD     wSize;
  PSTR     pImage;
  ICODIR   icoDir;
  WORD     wColors;
  WORD     wXORSize;
  WORD     wANDSize;
  
  if (pRsrc->biSize != sizeof(BITMAPINFOHEADER))
    printf("Conatins An Old Icon Format\n");
  else
  {
    wColors = DIBNumColors((WORD)pRsrc->biClrUsed,pRsrc->biBitCount);
    icoDir.byWidth     = (BYTE)pRsrc->biWidth;
    icoDir.byHeight    = (BYTE)pRsrc->biHeight / 2;
    icoDir.byColors    = (BYTE)wColors;
    icoDir.byReserved1 = 0x00;               
    icoDir.wReserved2  = (WORD)pRsrc->biPlanes;
    icoDir.wReserved3  = (WORD)pRsrc->biBitCount;
    wANDSize           = (WORD)(icoDir.byWidth * (icoDir.byHeight / 2)) >> 3;
    if (pRsrc->biBitCount > 1)
      wXORSize         = wANDSize * pRsrc->biBitCount;
    else
      wXORSize         = wANDSize * pRsrc->biPlanes;
    pRsrc->biSizeImage = (long)(wANDSize +  wXORSize); 
    icoDir.dwDIBSize   = pRsrc->biSizeImage + pRsrc->biSize + 
                         wColors * sizeof(RGBQUAD);
    icoDir.dwDIBOfs    = sizeof(ICOHDR) + sizeof(ICODIR);    
    pRsrc->biClrUsed   = wColors;
    write(gfhOutFile, (PSTR)&icoDir, sizeof(ICODIR));
    write(gfhOutFile, (PSTR)pRsrc, (WORD)pRsrc->biSize);
    wSize = (WORD)pRsrc->biClrUsed * sizeof(RGBQUAD);
    if ((pRgb = (RGBQUAD *)malloc(wSize)) == NULL)
      Fatal("Unable to allocate memory for Color Table");
    read (gfhInFile, (PSTR)pRgb, wSize);
    write(gfhOutFile, (PSTR)pRgb, wSize);
    free(pRgb);
    if ((pImage = (PSTR)malloc((WORD)pRsrc->biSizeImage)) == NULL)
      Fatal("Unable to allocate memory for Icon Image");
    read (gfhInFile, pImage, (WORD)pRsrc->biSizeImage);
    write(gfhOutFile, pImage, (WORD)pRsrc->biSizeImage);
    free(pImage);
  }
}
/***************************************************************************/
/***************************************************************************/
WORD DIBNumColors(WORD wClrUsed, WORD wBitCount)
{
  if(wClrUsed)
    return wClrUsed;

  switch(wBitCount)
  {
    case 1  : return   2;
    case 3  : return   8;
    case 4  : return  16;
    case 8  : return 256;
    default : return  16;
  }
}
/***************************************************************************/
/***************************************************************************/
void ConvertToPM(void)
{
  char                  szFileName[13];
  int                   nLen;
  ICOHDR                icoHdr;
  BITMAPINFOHEADER      Bmi;
  ICODIR                icoDir;
  PSTR                  pXORBits;
  PSTR                  pANDBits;
  RGBQUAD               *pRGB;
  WORD                  wANDSize;
  WORD                  wXORSize;
  WORD                  wSize;
  BITMAPARRAYFILEHEADER bafhHdr;
  OS2_BITMAPFILEHEADER  bfhHdr;
  WORD                  wColors;
  char                  MonoRGBDefaults[]= {0x00,0x00,0x00,0xFF,0xFF,0xFF};
  WORD                  wCntr;
  
  
  read(gfhInFile, (PSTR)&icoHdr, sizeof(icoHdr));
  if (icoHdr.wReserved != 0x0000 && icoHdr.wType != 0x0001)
  {
    printf("Invalid Windows ICO File Format\n");
    return;
  }
  nLen = strlen(gszFileName);
  if (nLen > 12)
    nLen = 12;
  strncpy(szFileName, gszFileName, nLen);
  szFileName[nLen - 1] = 'P';
  szFileName[nLen]     = '\0';
  if ((gfhOutFile = open(szFileName, O_BINARY | O_CREAT | O_TRUNC | O_WRONLY,
                                     S_IREAD  | S_IWRITE)) == -1)
  {
    printf("Could Not Create file %s\n", szFileName);
    Fatal("File IO Error");
  }
  if (icoHdr.wCount > 1)
    printf("%d Images Found : Only Writing 1 - ", icoHdr.wCount);  
  printf("Converting To File %s\n", szFileName);
  
  read(gfhInFile, (PSTR)&icoDir, sizeof(icoDir));  
  lseek(gfhInFile, icoDir.dwDIBOfs, SEEK_SET);
  read(gfhInFile, (PSTR)&Bmi, sizeof(Bmi));
  wColors = DIBNumColors((WORD)Bmi.biClrUsed, Bmi.biBitCount);
  wSize   = wColors * sizeof(RGBQUAD);
  if ((pRGB = (RGBQUAD *)malloc(wSize)) == NULL)
    Fatal("Unable To Allocate Color Table");
  read(gfhInFile, (PSTR)pRGB, wSize);
  wANDSize = (WORD)(Bmi.biWidth * (Bmi.biHeight / 2)) >> 3;
  if (Bmi.biBitCount > 1)
    wXORSize = wANDSize * Bmi.biBitCount;
  else
    wXORSize = wANDSize * Bmi.biPlanes;
  
  if ((pXORBits = (PSTR) malloc(wXORSize)) == NULL)
    Fatal("Unable To Allocate Memory For XOR Bit Map");
  if ((pANDBits = (PSTR) malloc(wANDSize)) == NULL)
    Fatal("Unable To Allocate Memory For AND Bit Map");    
  read(gfhInFile, pXORBits, wXORSize); 
  read(gfhInFile, pANDBits, wANDSize);  
    
  bafhHdr.usType            = BFT_BITMAPARRAY;
  bafhHdr.cbSize            = sizeof(bafhHdr);
  bafhHdr.offNext           = 0L;
  bafhHdr.cxDisplay         = 0;
  bafhHdr.cyDisplay         = 0;
  bafhHdr.bfh.usType        = BFT_COLORICON;
  bafhHdr.bfh.cbSize        = sizeof(bafhHdr.bfh);
  bafhHdr.bfh.xHotspot      = 0;
  bafhHdr.bfh.yHotspot      = 0;
  bafhHdr.bfh.offBits       = sizeof(bafhHdr) + sizeof(bfhHdr) + 
                              (2  +  wColors) * sizeof(RGB);
  bafhHdr.bfh.bmp.cbFix     = sizeof(bafhHdr.bfh.bmp);
  bafhHdr.bfh.bmp.cx        = (WORD)Bmi.biWidth;
  bafhHdr.bfh.bmp.cy        = (WORD)Bmi.biHeight;
  bafhHdr.bfh.bmp.cPlanes   = 1;
  bafhHdr.bfh.bmp.cBitCount = 1;
  
  bfhHdr.usType        = BFT_COLORICON;
  bfhHdr.cbSize        = sizeof(bfhHdr);
  bfhHdr.xHotspot      = 0;
  bfhHdr.yHotspot      = 0;
  bfhHdr.offBits       = bafhHdr.bfh.offBits  + wANDSize * 2;

  bfhHdr.bmp.cbFix     = sizeof(bfhHdr.bmp);
  bfhHdr.bmp.cx        = (WORD)Bmi.biWidth;
  bfhHdr.bmp.cy        = (WORD)Bmi.biHeight / 2;
  bfhHdr.bmp.cPlanes   = 1;
  if (Bmi.biBitCount > 1)
    bfhHdr.bmp.cBitCount = Bmi.biBitCount;
  else
    bfhHdr.bmp.cBitCount = Bmi.biPlanes;
  
  write(gfhOutFile, (PSTR)&bafhHdr,         sizeof(bafhHdr));
  write(gfhOutFile, (PSTR)MonoRGBDefaults,  sizeof(MonoRGBDefaults));
  write(gfhOutFile, (PSTR)&bfhHdr,          sizeof(bfhHdr));
  for (wCntr = 0; wCntr < wColors; wCntr++)
    write(gfhOutFile, (PSTR)(pRGB + wCntr), sizeof(RGB)); 
  write(gfhOutFile, pANDBits, wANDSize);
  write(gfhOutFile, pANDBits, wANDSize);
  write(gfhOutFile, pXORBits, wXORSize);
  free(pANDBits);
  free(pXORBits);
  free(pRGB);  
  close(gfhOutFile);
}

/***************************************************************************/
/***************************************************************************/
void ConvertToWin(void)
{
  BITMAPARRAYFILEHEADER bafh;
  OS2_BITMAPFILEHEADER  bfh;
  BYTE                  Done = FALSE;
  char                  szFileName[13];
  int                   nLen;
  ICOHDR                icoHdr;
  BITMAPINFOHEADER      Bmi;
  ICODIR                icoDir;
  PSTR                  pXORBits;
  PSTR                  pANDBits;
  RGB                   *pRGB;
  WORD                  wANDSize;
  WORD                  wXORSize;
  WORD                  wSize;
  WORD                  wColors;
  WORD                  wCntr;
  RGBQUAD               RGBQ;
  WORD                  wIndex = 0;
  char                  szBuf[3];
  char                  *pExt;
  
  printf("Dimensions: ");
  bafh.offNext = 0L;
  while(!Done)
  {    
    lseek(gfhInFile, bafh.offNext, SEEK_SET);
    read(gfhInFile, (PSTR)&bafh, sizeof(bafh));     
    if (bafh.usType != 0x4142)
    {
      printf("Invalid PM ICO Format\n");
      return;
    }
    wSize = 2 * sizeof(RGB);
    lseek(gfhInFile, (long)wSize, SEEK_CUR);
    read(gfhInFile, (PSTR)&bfh, sizeof(bfh));
    printf("%.2dx%.2d", bfh.bmp.cx, bfh.bmp.cy);
    if (bfh.bmp.cx <= 32 && bfh.bmp.cy <= 32)
    {
      printf("* ");
      icoHdr.wReserved = 0x0000;
      icoHdr.wType     = 0x0001;
      icoHdr.wCount    = 0x0001;
      if (wIndex++ == 0)
      {
        nLen = strlen(gszFileName);
        if (nLen > 12)          
          nLen = 12;
        strncpy(szFileName, gszFileName, nLen);        
        szFileName[nLen - 1] = 'W';
        szFileName[nLen]     = '\0';
      }
      else
      {
        pExt = strrchr(gszFileName, '.');
        nLen = strlen(gszFileName) - strlen(pExt);
        if (nLen > 6)
          nLen = 6;
        strncpy(szFileName, gszFileName, nLen);
        szFileName[nLen] = '\0';
        sprintf(szBuf, "%.2d", wIndex);
        strcat(szFileName, szBuf);
        strcat(szFileName, ".ICW");
      }        
      if ((gfhOutFile= open(szFileName, O_BINARY| O_CREAT| O_TRUNC| O_WRONLY,
                                        S_IREAD | S_IWRITE)) == -1)
      {
        printf("Could Not Create file %s\n", szFileName);
        Fatal("File IO Error");
      }
      write(gfhOutFile, (PSTR)&icoHdr, sizeof(icoHdr));
      Bmi.biSize          = sizeof(Bmi);
      Bmi.biWidth         = (DWORD)bafh.bfh.bmp.cx;
      Bmi.biHeight        = (DWORD)bafh.bfh.bmp.cy;
      Bmi.biPlanes        = bfh.bmp.cPlanes;
      Bmi.biBitCount      = bfh.bmp.cBitCount;
      Bmi.biCompression   = 0L;
      Bmi.biXPelsPerMeter = 0L;
      Bmi.biYPelsPerMeter = 0L;
      wColors             = DIBNumColors(0, Bmi.biBitCount);
      Bmi.biClrUsed       = wColors;
      Bmi.biClrImportant  = 0;      
      icoDir.byWidth      = (BYTE)Bmi.biWidth;
      icoDir.byHeight     = (BYTE)Bmi.biHeight;
      icoDir.byColors     = (BYTE)Bmi.biClrUsed;
      icoDir.byReserved1  = 0x00;               
      icoDir.wReserved2   = Bmi.biPlanes;
      icoDir.wReserved3   = Bmi.biBitCount;
      wANDSize            = (WORD)(bfh.bmp.cx * bfh.bmp.cy) >> 3;
      if (Bmi.biBitCount > 1)
        wXORSize          = wANDSize * Bmi.biBitCount;
      else
        wXORSize          = wANDSize * Bmi.biPlanes;
      Bmi.biSizeImage     = (long)(wANDSize +  wXORSize); 
      icoDir.dwDIBSize    = Bmi.biSizeImage + Bmi.biSize + wColors 
                          * sizeof(RGBQUAD);
      icoDir.dwDIBOfs     = sizeof(ICOHDR) + sizeof(ICODIR);
      write(gfhOutFile, (PSTR)&icoDir, sizeof(ICODIR));
      write(gfhOutFile, (PSTR)&Bmi, sizeof(Bmi));
      wSize = wColors * sizeof(RGB);
      if ((pRGB = (RGB *)malloc(wSize)) == NULL)
        Fatal("Unable to allocate memory for Color Table");
      read (gfhInFile, (PSTR)pRGB, wSize);
      for (wCntr = 0; wCntr < wColors; wCntr++)
      {
        memcpy((PSTR)&RGBQ, (pRGB + wCntr), sizeof(RGB));
        RGBQ.rgbReserved = 0x00;
        write(gfhOutFile, (PSTR)&RGBQ, sizeof(RGBQ));
      }      
      free(pRGB);
      if ((pANDBits = (PSTR)malloc(wANDSize)) == NULL)
        Fatal("Unable to allocate memory for AND Bits");
      if ((pXORBits = (PSTR)malloc(wXORSize)) == NULL)
        Fatal("Unable to allocate memory for XOR Bits");      
      lseek(gfhInFile, bafh.bfh.offBits, SEEK_SET);      
      read(gfhInFile, pANDBits, wANDSize);
      lseek(gfhInFile, bfh.offBits, SEEK_SET);
      read(gfhInFile, pXORBits, wXORSize);
      write(gfhOutFile, pXORBits, wXORSize);
      write(gfhOutFile, pANDBits, wANDSize);
      free(pXORBits);
      free(pANDBits);
      close(gfhOutFile);
    }
    else
      printf(" ");
   if (!bafh.offNext)
     Done = TRUE;
  }
  printf("\n");
}