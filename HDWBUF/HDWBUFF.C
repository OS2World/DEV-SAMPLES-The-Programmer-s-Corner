/* ----------------------------------------------------------------------
.context BUFFOFF
.category UTILITY
VOID main(INT argv, CHAR *argc[]);

Description: 
     A short program that turns off the hardware bufferring of the COMM
port

Parameter     Description
-------------------------------------------------------------------------
argv          number of parameters
argc          parms:
              1st parm = commport to change ( COM1: COM2: COM3: ... )
              2nd parm = state to set it too ( ON, OFF );

Returns: 
     A fail state to command.com

Comments: 

References: 

See Also: 
.ref 

Development History: 
  Date         Programmer          Description of modification   
  10/10/1989   Paul Montgomery     Initial development           
-------------------------------------------------------------------- */
#define INCL_DOSFILEMGR
#define INCL_DOSDEVIOCTL
#define INCL_DOSDEVICES
#include <os2.h>
#include <stdio.h>
#include <string.h>

VOID main ( INT argc, CHAR *argv[] )
{
   HFILE hf;
   USHORT usAction;
   DCBINFO   dcb;

   if (argc < 3)
      {
      printf("\n\
usage: hdwbuff COM1 ON   ; to turn on hardware buffering for COM1\n");
      printf(
"       hdwbuff COM2 OFF  ; to turn off hardware buffering for COM2\n");
      DosExit(EXIT_PROCESS, 1);
      }

   // open the port specified
   DosOpen(argv[1],
      &hf,         
      &usAction,   
      0L,        
      FILE_NORMAL, 
      FILE_OPEN, 
      OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYNONE,
      0L);

   if (hf)
      {
      // get the current state of things with that port
      DosDevIOCtl(&dcb,
         NULL,         
         0x73,         
         1,            
         hf);

      // figure out what the user wants
      if ((strcmp("ON",argv[2])==0) || (strcmp("on",argv[2]) == 0))
         {
         // change the hardware buffering to ENABLED
         dcb.fbTimeout = dcb.fbTimeout & 0xE7;
         dcb.fbTimeout = dcb.fbTimeout | 0x08;
         printf("\n Extended Hardware Buffering Enabled\n");
         }
      else if ((strcmp("OFF",argv[2])==0) || (strcmp("off",argv[2]) == 0))
         {
         // change the hardware bufferring to DISABLED
         dcb.fbTimeout = dcb.fbTimeout & 0xE7;
         printf("\n Extended Hardware Buffering Disabled\n");
         }
      else
         {
         printf("\navailable options are ON, OFF\n");
         DosClose(hf);
         DosExit(EXIT_PROCESS, 1);
         }

      // make the change.
      DosDevIOCtl(NULL,
         &dcb,         
         0x53,         
         1,            
         hf);

      // close the port
      DosClose(hf);
      }
   else
      {
      printf("\n The open on %s did not work.",argv[1]);
      printf("\n\
usage: hdwbuff COM1 ON   ; to turn on hardware buffering for COM1\n");
      printf(
"       hdwbuff COM2 OFF  ; to turn off hardware buffering for COM2\n");
      DosExit(EXIT_PROCESS, 1);

      }
   DosExit(EXIT_PROCESS, 0);
}

