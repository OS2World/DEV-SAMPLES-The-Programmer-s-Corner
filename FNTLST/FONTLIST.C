/*--------------------------------------------------------------
   FONTLIST.C -- PM Font List Program (c) 1989, Charles Petzold
  --------------------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define LCID_MYFONT 1L

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "FontList" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_VERTSCROLL ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
                                     0L, NULL, 0, &hwndClient) ;

     if (hwndFrame != NULL)
          {
          WinSendMsg (hwndFrame, WM_SETICON,
                      WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                      NULL) ;

          while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
               WinDispatchMsg (hab, &qmsg) ;

          WinDestroyWindow (hwndFrame) ;
          }
     else
          WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
                         "Cannot allocate memory for FONTMETRICS structures.",
                         szClientClass, 0, MB_OK | MB_ICONEXCLAMATION) ;

     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

LONG RightJustifyStringAt (HPS hps, POINTL *pptl, LONG lLength, CHAR *pchText)
     {
     POINTL aptlTextBox[TXTBOX_COUNT] ;

     GpiQueryTextBox (hps, lLength, pchText, TXTBOX_COUNT, aptlTextBox) ;
     pptl->x -= aptlTextBox[TXTBOX_CONCAT].x ;
     return GpiCharStringAt (hps, pptl, lLength, pchText) ;
     }

VOID ShowFontMetrics (HPS hps, PFONTMETRICS pfmIn, SHORT cyClient)
     {
     static FONTMETRICS  fm ;
     static struct {
                   SHORT sCol ;
                   SHORT sRow ;
                   CHAR  *szLabel ;
                   SHORT sFormat ;
                   VOID  *pItem ;
                   }
                   disp [] =
                   {
                   0,  0, "szFamilyname"       , 0,  fm.szFamilyname       ,
                   0,  1, "szFacename"         , 0,  fm.szFacename         ,
                   0,  2, "idRegistry"         , 1, &fm.idRegistry         ,
                   0,  3, "usCodePage"         , 2, &fm.usCodePage         ,
                   0,  4, "lEmHeight"          , 3, &fm.lEmHeight          ,
                   0,  5, "lXHeight"           , 3, &fm.lXHeight           ,
                   0,  6, "lMaxAscender"       , 3, &fm.lMaxAscender       ,
                   0,  7, "lMaxDescender"      , 3, &fm.lMaxDescender      ,
                   0,  8, "lLowerCaseAscent"   , 3, &fm.lLowerCaseAscent   ,
                   0,  9, "lLowerCaseDescent"  , 3, &fm.lLowerCaseDescent  ,
                   0, 10, "lInternalLeading"   , 3, &fm.lInternalLeading   ,
                   0, 11, "lExternalLeading"   , 3, &fm.lExternalLeading   ,
                   0, 12, "lAveCharWidth"      , 3, &fm.lAveCharWidth      ,
                   0, 13, "lMaxCharInc"        , 3, &fm.lMaxCharInc        ,
                   0, 14, "lEmInc"             , 3, &fm.lEmInc             ,
                   0, 15, "lMaxBaselineExt"    , 3, &fm.lMaxBaselineExt    ,
                   0, 16, "sCharSlope"         , 2, &fm.sCharSlope         ,
                   1,  2, "sInlineDir"         , 2, &fm.sInlineDir         ,
                   1,  3, "sCharRot"           , 2, &fm.sCharRot           ,
                   1,  4, "usWeightClass"      , 1, &fm.usWeightClass      ,
                   1,  5, "usWidthClass"       , 1, &fm.usWidthClass       ,
                   1,  6, "sXDeviceRes"        , 2, &fm.sXDeviceRes        ,
                   1,  7, "sYDeviceRes"        , 2, &fm.sYDeviceRes        ,
                   1,  8, "sFirstChar"         , 2, &fm.sFirstChar         ,
                   1,  9, "sLastChar"          , 2, &fm.sLastChar          ,
                   1, 10, "sDefaultChar"       , 2, &fm.sDefaultChar       ,
                   1, 11, "sBreakChar"         , 2, &fm.sBreakChar         ,
                   1, 12, "sNominalPointSize"  , 2, &fm.sNominalPointSize  ,
                   1, 13, "sMinimumPointSize"  , 2, &fm.sMinimumPointSize  ,
                   1, 14, "sMaximumPointSize"  , 2, &fm.sMaximumPointSize  ,
                   1, 15, "fsType"             , 1, &fm.fsType             ,
                   1, 16, "fsDefn"             , 1, &fm.fsDefn             ,
                   2,  0, "fsSelection"        , 1, &fm.fsSelection        ,
                   2,  1, "fsCapabilities"     , 1, &fm.fsCapabilities     ,
                   2,  2, "lSubscriptXSize"    , 3, &fm.lSubscriptXSize    ,
                   2,  3, "lSubscriptYSize"    , 3, &fm.lSubscriptYSize    ,
                   2,  4, "lSubscriptXOffset"  , 3, &fm.lSubscriptXOffset  ,
                   2,  5, "lSubscriptYOffset"  , 3, &fm.lSubscriptYOffset  ,
                   2,  6, "lSuperscriptXSize"  , 3, &fm.lSuperscriptXSize  ,
                   2,  7, "lSuperscriptYSize"  , 3, &fm.lSuperscriptYSize  ,
                   2,  8, "lSuperscriptXOffset", 3, &fm.lSuperscriptXOffset,
                   2,  9, "lSuperscriptYOffset", 3, &fm.lSuperscriptYOffset,
                   2, 10, "lUnderscoreSize"    , 3, &fm.lUnderscoreSize    ,
                   2, 11, "lUnderscorePosition", 3, &fm.lUnderscorePosition,
                   2, 12, "lStrikeoutSize"     , 3, &fm.lStrikeoutSize     ,
                   2, 13, "lStrikeoutPosition" , 3, &fm.lStrikeoutPosition ,
                   2, 14, "sKerningPairs"      , 2, &fm.sKerningPairs      ,
                   2, 15, "sReserved"          , 2, &fm.sReserved          ,
                   2, 16, "lMatch"             , 3, &fm.lMatch
                   } ;
     CHAR                szBuffer [32] ;
     INT                 i ;
     LONG                cxChar, cyChar, cyDesc ;
     POINTL              ptl ;

     GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
     cxChar = fm.lAveCharWidth ;
     cyChar = fm.lMaxBaselineExt ;
     cyDesc = fm.lMaxDescender ;

     fm = *pfmIn ;

     for (i = 0 ; i < sizeof disp / sizeof disp[0] ; i++)
          {
          ptl.x = 32 * cxChar * disp[i].sCol ;
          ptl.y = cyClient - cyChar * (1 + disp[i].sRow) + cyDesc ;

          GpiCharStringAt (hps, &ptl, (LONG) strlen (disp[i].szLabel),
                          disp[i].szLabel) ;

          switch (disp[i].sFormat)
               {
               case 0:                       // CHAR
                    ptl.x += 20 * cxChar ;
                    GpiCharStringAt (hps, &ptl,
                                     (LONG) strlen (disp[i].pItem),
                                     disp[i].pItem) ;
                    break ;

               case 1:                       // USHORT
                    ptl.x += 30 * cxChar ;
                    RightJustifyStringAt (hps, &ptl,
                         (LONG) sprintf (szBuffer, "%u",
                                         * (USHORT *) disp[i].pItem),
                         szBuffer) ;
                    break ;

               case 2:                       // SHORT
                    ptl.x += 30 * cxChar ;
                    RightJustifyStringAt (hps, &ptl,
                         (LONG) sprintf (szBuffer, "%u",
                                         * (SHORT *) disp[i].pItem),
                         szBuffer) ;
                    break ;

               case 3:                       // LONG
                    ptl.x += 30 * cxChar ;
                    RightJustifyStringAt (hps, &ptl,
                         (LONG) sprintf (szBuffer, "%u",
                                         * (LONG *) disp[i].pItem),
                         szBuffer) ;
                    break ;
               }
          }
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static FATTRS      fat ;
     static HWND        hwndVScroll ;
     static FONTMETRICS *pfm ;
     static SHORT       cxClient, cyClient ;
     static USHORT      usNumberFonts, usCurrentFont ;
     HPS                hps ;
     LONG               lRequestFonts, lNumberFonts ;
     POINTL             ptl ;

     switch (msg)
          {
          case WM_CREATE:
                              // Get all FONTMETRICS structures in pfm

               hps = WinGetPS (hwnd) ;
               lRequestFonts = 0 ;
               lNumberFonts = GpiQueryFonts (hps, QF_PUBLIC, NULL,
                                             &lRequestFonts, 0L, NULL) ;

               if (lNumberFonts > USHRT_MAX)
                    return 1 ;

               if (lNumberFonts * sizeof (FONTMETRICS) > UINT_MAX)
                    return 1 ;

               pfm = malloc ((UINT) (lNumberFonts * sizeof (FONTMETRICS))) ;

               if (pfm == NULL)
                    return 1 ;

               GpiQueryFonts (hps, QF_PUBLIC, NULL, &lNumberFonts,
                              (LONG) sizeof (FONTMETRICS), pfm) ;

               usNumberFonts = (USHORT) lNumberFonts ;
               WinReleasePS (hps) ;

                              // Initialize scroll bar

               hwndVScroll = WinWindowFromID (
                                 WinQueryWindow (hwnd, QW_PARENT, FALSE),
                                 FID_VERTSCROLL) ;

               WinSendMsg (hwndVScroll, SBM_SETSCROLLBAR,
                           MPFROM2SHORT (usCurrentFont, 0),
                           MPFROM2SHORT (0, usNumberFonts - 1)) ;
               return 0 ;

          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_CHAR:
               return WinSendMsg (hwndVScroll, msg, mp1, mp2) ;

          case WM_VSCROLL:
               switch (SHORT2FROMMP (mp2))
                    {
                    case SB_LINEUP:
                         usCurrentFont -= 1 ;
                         break ;

                    case SB_LINEDOWN:
                         usCurrentFont += 1 ;
                         break ;

                    case SB_PAGEUP:
                         usCurrentFont -= 10 ;
                         break ;

                    case SB_PAGEDOWN:
                         usCurrentFont += 10 ;
                         break ;

                    case SB_SLIDERPOSITION:
                         usCurrentFont = SHORT1FROMMP (mp2) ;
                         break ;

                    default:
                         return 0 ;
                    }
               usCurrentFont = max (0, min (usCurrentFont, usNumberFonts - 1));

               if (usCurrentFont != (USHORT) WinSendMsg (hwndVScroll,
                                                  SBM_QUERYPOS, NULL, NULL))
                    {
                    WinSendMsg (hwndVScroll, SBM_SETPOS,
                                MPFROM2SHORT (usCurrentFont, 0), NULL) ;

                    WinInvalidateRect (hwnd, NULL, FALSE) ;
                    }
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               ShowFontMetrics (hps, pfm + usCurrentFont, cyClient) ;

               fat.usRecordLength = sizeof fat ;
               fat.lMatch         = pfm[usCurrentFont].lMatch ;
               fat.fsSelection    = 0 ;
               strcpy (fat.szFacename, pfm[usCurrentFont].szFacename) ;
               GpiCreateLogFont (hps, NULL, LCID_MYFONT, &fat) ;
               GpiSetCharSet (hps, LCID_MYFONT) ;

               ptl.x = 0 ;
               ptl.y = pfm[usCurrentFont].lMaxDescender ;

               GpiCharStringAt (hps, &ptl,
                                pfm[usCurrentFont].fsDefn & 1 ? 6L : 52L,
                                "AaBbCcDdEeFfGgHhIiJjKkLlMm"
                                "NnOoPpQqRrSsTtUuVvWwXxYyZz") ;

               GpiSetCharSet (hps, LCID_DEFAULT) ;
               GpiDeleteSetId (hps, LCID_MYFONT) ;
               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               free (pfm) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
                                                                                        
