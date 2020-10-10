/* "os2local.h" for OS/2 1.1 and Microsoft C Version 5.1 compiler */

#if defined _os2local
	/* Do nothing. */
#else
	extern void EXPENTRY errabort (PSZ pszProgramName, PSZ pszFunctionName,
		   PSZ pszCallerName, USHORT usLineNum, USHORT usReturn) ;

	#define ERRCHK(os2name)	if (usRC != 0) { \
			errabort (pszProgramName, #os2name, pszFunctionName, \
			  __LINE__, usRC) ; \
		}

	/* Standard devices. */
	#define STDIN	0
	#define STDOUT	1
	#define STDERR	2

    #define _os2local
#endif /* #if defined _os2local */
/* end of insert file "os2local.h" */
