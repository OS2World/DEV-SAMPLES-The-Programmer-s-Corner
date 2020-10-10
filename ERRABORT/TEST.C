/* test.c - test my 'errabort ()' DLL routine. */

#define INCL_DOS

#include <os2.h>
#include <stdio.h>
#include <os2local.h>

/* The following variable must be this data type and this name.  In
 * addition, it must be global in scope -- although for this small
 * program, with only a single source file and a single function
 * (main) it does not matter, it is necessary for more typical programs.
 */
static PSZ pszProgramName = "test.c" ;


void main (int argc, char **argv)
{
	/* These first two variables must be declared as shown, with the
	 * same data type and same name.  In practice, I usually declare
	 * 'usRC' globally and let every function use the same one; however,
	 * if you do that, be sure you declare it locally for any threads
	 * you create.
	 */
	static PSZ pszFunctionName = "main" ;
	USHORT usRC ;

	/* The rest of these variables are unrelated to 'errabort ()'; they
	 * are simply what this particular program requires.
	 */
	HDIR hdir ;
	FILEFINDBUF findbuf ;
	USHORT usSearchCount ;
	USHORT cbBytes ;
	static UCHAR szHello [] = "\r\nTest routine for 'errabort ()' has just begun.\r\n" ;
	static UCHAR szHuh [] = "\r\nHuh?  'errabort ()' should have worked.\r\n" ;

	/* Even though we initialized 'pszProgramName' above to what we believe
	 * is the program name at the time we are compiling it, we can get a
	 * more up-to-date name now, at least with OS/2 1.1 and Microsoft C
	 * 5.1; other combinations may need to omit the next statement and
	 * rely on the initialization above.
	 */
	pszProgramName = argv [0] ;

	/* We are ready to make our first OS/2 call.  It is important that
	 * the return code from the call be assigned to 'usRC'.
	 */
	usRC = DosWrite (STDERR, szHello, sizeof (szHello) - 1, &cbBytes) ;

	/* Now we invoke the 'ERRCHK' macro.  Note that we can code this
	 * macro as if it were a function (as has been done here), with a
	 * terminating semicolon.  Purists may prefer to omit the semicolon
	 * to emphasize that 'ERRCHK' is a macro and not a function; that
	 * works too.
	 */
	ERRCHK (DosWrite) ;

	/* Now create an error that 'errabort ()' should handle. */
	usSearchCount = 1 ;			/* This is OK. */
	hdir = HDIR_CREATE ;		/* So is this. */

	/* However, we are relying on your directory not having a file
	 * named "abc.xyz".  If it does, then we will find it, and you
	 * will be left wondering whether 'errabort ()' actually works
	 * or not.  Ask someone who uses more likely names for their files.
	 */
	usRC = DosFindFirst ("abc.xyz", &hdir, FILE_NORMAL, &findbuf,
	  sizeof (findbuf), &usSearchCount, 0L) ;
	ERRCHK (DosFindFirst) ;

	/* We shouldn't get to this statement, because the preceeding one
	 * should have resulted in a call to 'errabort ()', which in turn
	 * should have done 'DosExit (EXIT_PROCESS, 1)'.  If you see the
	 * following message, you may be programming in the Twilight Zone...
	 */
	usRC = DosWrite (STDERR, szHuh, sizeof (szHuh) - 1, &cbBytes) ;
	DosExit (EXIT_PROCESS, 1) ;
}
