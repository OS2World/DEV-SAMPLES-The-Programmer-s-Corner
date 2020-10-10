/* errabort.c - A debugging tool for OS/2 programmers.
 *
 * This DLL is to be called after first calling any OS/2 function and
 * testing for expected return codes.  "Testing for expected return
 * codes" means, for example, testing for zero (successful completion)
 * IN EVERY CASE, and testing for errors that your program anticipates
 * and has code to handle, such as "File not found" when calling
 * 'DosOpen ()'.
 *
 * Any other error return presumably is one that your program did not
 * anticipate and cannot properly handle.  In that case, a call to this
 * DLL will result in the display of an error message, and a termination
 * of your program.  The error message includes your program's name,
 * the name of the function within your program that made the call to an
 * OS/2 function, the line number where the call to this program originated
 * (which should be the line immediately following the call to the OS/2
 * function that returned an unexpected error code), the name of the OS/2
 * function being called, the number of the error code, and a message
 * associated with the error code.
 *
 * If you are not presently testing EVERY OS/2 function call's return
 * value, then you may be surprised by some of the error messages (and
 * program aborts) that calls to this program will give you.  I have
 * found return codes that are not documented as being among those
 * returned by certain functions, for example.  In other cases, return
 * codes that I anticipated (such as 'ERROR_FILE_NOT_FOUND' and
 * 'ERROR_PATH_NOT_FOUND' from 'DosFindFirst ()') were NOT returned;
 * instead I got 'ERROR_NO_MORE_FILES' (it surprised me; maybe it
 * wouldn't surprise you).  Since this program is a DLL, it is certainly
 * cheap enough to include a call to it after every  OS/2 call, at least
 * during debugging.  I modify the ERRCHK macro (included as part of this
 * package) to conditionally generate code depending on the state of a
 * global debugging flag, which makes it easy to remove even the calls.
 *
 * This program with all its associated materials is explicitly placed into
 * the public domain.  Feel free to use it, modify it, or incorporate it
 * into your own programs, whether for your own use or for sale; in short,
 * enjoy.  And if you have something of your own that is useful but was
 * tedious to produce, share it with the rest of us.
 *
 * If you have questions or comments about this program, I am:
 *
 * Author:  Wayne Kovsky				BIX:  			wkovsky
 *			Pinnacle Software			CompuServe:		76164,3504
 *			299 Village Street
 *			Millis, MA  02054
 */

#define INCL_DOS

#include <os2.h>
#include <os2local.h>
/* The '#include os2local.h' statement gets these items included in
 * your program, in case you did not get that file when you got this
 * program.  Your programs that CALL 'errabort ()' will need the first
 * two items as well.
 *
 * extern void EXPENTRY errabort (PSZ pszProgramName, PSZ pszFunctionName,
 *		   PSZ pszCallerName, USHORT usLineNum, USHORT usReturn) ;
 *
 *	#define ERRCHK(os2name)	if (usRC != 0) { \
 *			errabort (pszProgramName, #os2name, pszFunctionName, __LINE__, usRC) ; \
 *		}
 *
 *
 *	#define STDIN	0
 *	#define STDOUT	1
 *	#define STDERR	2
 */

/* Since this is destined to be a DLL, we must provide our own versions
 * of C library functions 'strlen ()' and 'itoa ()'.
 */
static USHORT FAR PASCAL mystrlen (PSZ pszString) ;
static void FAR PASCAL myitoa (USHORT usNumber, PSZ pszMessage) ;

int _acrtused = 0 ;			/* Prevent C startup code being included. */

/* What follows is the error text for every documented error code I could
 * find.  It is a LOT of text, and it accounts for the majority of the
 * bulk the finished DLL contains; there is very little executable code.
 * It is for precisely this reason that this is a DLL:  every program you
 * are working on can use this single copy of all this bulk, and let OS/2
 * manage the memory requirements.
 *
 * Most of these messages originated in the "BSEERR.H" file that is
 * in "OS2TOOLS\INCLUDE" as part of the Microsoft OS/2
 * Programmer's Toolkit.  That include file has all error messages
 * in terse upper case for use in C programs; I converted them to
 * these more descriptive messages.  In addition, there were a few
 * duplicates (i.e., two different "#define" names for the same
 * error code); in those cases I (arbitrarily) selected one and
 * discarded the other.
 *
 * Some errors not documented in the "BSEERR.H" file were found in
 * Ray Duncan's book "Advanced OS/2 Programming", first edition,
 * published by Microsoft Press, 1989, ISBN 1-55615-045-8, in
 * Appendix A, pp. 697-706.  I requested and received Mr. Duncan's
 * permission to include those here.
 *
 * Note that many error messages are simply "RESERVED".  I have attempted
 * to conserve space by leaving "holes" in this table of error messages
 * wherever these "RESERVED" error numbers occur.  This means we cannot
 * use the error number as an index into this table, but instead must
 * search the table for a match.  Since this is an "Oh my God, something
 * horrible must have happened!" kind of program, I have followed my
 * long-standing rule to keep it as bulletproof as possible; thus, I have
 * not even so much as done a binary search here.  For a program that
 * does so little and then kills your process, it seemed to me that
 * performance was not much of an issue ... your mileage may vary.
 */
static struct ErrorList {
	USHORT usErrorNum ;
	PSZ pszErrorText ;
} ErrorArray [] = {
	0, "No error",				/* Note that this still ends in abort! */
	1, "Invalid function number",
	2, "File not found",
	3, "Path not found",
	4, "Too many open files",
	5, "Access denied",
	6, "Invalid handle",
	7, "Memory control blocks destroyed",
	8, "Not enough memory",
	9, "Invalid memory block address",
	10, "Invalid environment",
	11, "Invalid format",
	12, "Invalid access code",
	13, "Invalid data",
	14, "Unknown unit",			/* From Duncan - but see error 20. */
	15, "Invalid disk drive",
	16, "Cannot remove current directory",
	17, "Not same device",
	18, "No more files",
	19, "Disk write-protected",
	20, "Invalid unit",			/* From BSEERR.H - but see error 14. */
	21, "Drive not ready",
	22, "Unknown command",
	23, "Data error (CRC)",
	24, "Bad request structure length",
	25, "Seek error",
	26, "Not DOS disk",
	27, "Sector not found",
	28, "Printer out of paper",
	29, "Write fault",
	30, "Read fault",
	31, "General failure",
	32, "Sharing violation",
	33, "Lock violation",
	34, "Invalid disk change",
	35, "FCB unavailable",
	36, "Sharing buffer exceeded",
#if defined INCLUDE_RESERVED
	37, "RESERVED",
	38, "RESERVED",
	39, "RESERVED",
	40, "RESERVED",
	41, "RESERVED",
	42, "RESERVED",
	43, "RESERVED",
	44, "RESERVED",
	45, "RESERVED",
	46, "RESERVED",
	47, "RESERVED",
	48, "RESERVED",
	49, "RESERVED",
#endif /* #if defined INCLUDE_RESERVED */
	50, "Unsupported network request",

	/* Error codes from 51 through 72 from Ray Duncan's book cited above. */
	51, "Remote machine not listening",
	52, "Duplicate name on network",
	53, "Network name not found",
	54, "Network busy",
	55, "Device no longer exists on network",
	56, "NetBIOS command limit exceeded",
	57, "Error in network adapter hardware",
	58, "Incorrect response from network",
	59, "Unexpected network error",
	60, "Remote adapter incompatible",
	61, "Print queue full",
	62, "Insufficient memory for print file",
	63, "Print file canceled",
	64, "Network name deleted",
	65, "Network access denied",
	66, "Incorrect network device type",
	67, "Network name not found",
	68, "Network name limit exceeded",
	69, "NetBIOS session limit exceeded",
	70, "File sharing temporarily paused",
	71, "Network request not accepted",
	72, "Print or disk redirection paused",
#if defined INCLUDE_RESERVED
	73, "RESERVED",
	74, "RESERVED",
	75, "RESERVED",
	76, "RESERVED",
	77, "RESERVED",
	78, "RESERVED",
	79, "RESERVED",
#endif /* #if defined INCLUDE_RESERVED */
	80, "File already exists",
	81, "Duplicate FCB",
	82, "Cannot make directory",
	83, "Fail on INT 24H (critical error)",
	84, "Out of structures (too many redirections)",
	85, "Already assigned (duplicate redirection)",
	86, "Invalid password",
	87, "Invalid parameter",
	88, "Network write fault",
	89, "No process slots available",
	90, "Not frozen",
	91, "Timer service table overflow",
	92, "Timer service table duplicate",
	93, "No items to work on",
#if defined INCLUDE_RESERVED
	94, "RESERVED", 
#endif /* #if defined INCLUDE_RESERVED */
	95, "Interrupted system call",
#if defined INCLUDE_RESERVED
	96, "RESERVED", 
	97, "RESERVED", 
	98, "RESERVED", 
	99, "RESERVED", 
#endif /* #if defined INCLUDE_RESERVED */
	100, "Too many semaphores",
	101, "Exclusive semaphore already owned",
	102, "Semaphore is set",
	103, "Too many exclusive semaphore requests",
	104, "Operation invalid at interrupt time",
	105, "Semaphore owner died",
	106, "Semaphore user limit exceeded",
	107, "Disk change (insert drive B disk into drive A)",
	108, "Drive locked",
	109, "Broken pipe (write on pipe with no reader)",
	110, "Open failed due to explicit fail command",
	111, "Buffer overflow",
	112, "Disk full",
	113, "No more search handles",
	114, "Invalid target handle",
	115, "Protection violation",
	116, "Vio/Kbd request error",
	117, "Invalid category",
	118, "Invalid verify switch",
	119, "Driver does not support DosDevIOCtl",
	120, "Call not implemented",
	121, "Semaphore timeout",
	122, "Insufficient data in buffer",
	123, "Invalid name",
	124, "Invalid level",
	125, "No volume label",
	126, "Module not found",
	127, "Procecure not found",
	128, "Wait no children",
	129, "Child not complete",
	130, "Invalid handle for direct disk access",
	131, "Negative seek",
	132, "Seek on device or pipe",
	133, "Drive has previously joined drives",
	134, "Drive is already joined",
	135, "Drive is already substituted",
	136, "Drive is not joined",
	137, "Drive is not substituted",
	138, "Cannot join to joined drive",
	139, "Cannot substitute to substituted drive",
	140, "Cannot join to substituted drive",
	141, "Cannot substitute to joined drive",
	142, "Drive is busy",
	143, "Cannot join or substitute to directory on same drive",
	144, "Must be subdirectory of root",
	145, "Joined directory must be empty",
	146, "Path is already used in substitute",
	147, "Path is already used in join",
	148, "Path is being used by another process",
	149, "Target directory is already substitute target",
	150, "System trace error",
	151, "Invalid event count",
	152, "Too many DosMuxSemWait waiters",
	153, "Invalid list format",
	154, "Volume label too long",
	155, "Too many TCBs",
	156, "Signal refused",
	157, "Segment is discarded",
	158, "Segment was not locked",
	159, "Bad thread ID address",
	160, "Bad arguments",
	161, "Bad pathname",
	162, "Signal already pending",
	163, "Uncertain media",
	164, "No more threads available",
	165, "Monitors not supported",
	166, "UNC driver not installed",
	167, "Lock failed",
	168, "Swap I/O failed",
	169, "Swapin failed",
	170, "Busy",
#if defined INCLUDE_RESERVED
	171, "RESERVED",
	172, "RESERVED",
	173, "RESERVED",
	174, "RESERVED",
	175, "RESERVED",
	176, "RESERVED",
	177, "RESERVED",
	178, "RESERVED",
	179, "RESERVED",
#endif /* #if defined INCLUDE_RESERVED */
	180, "Invalid segment number",
	181, "Invalid call gate",
	182, "Invalid ordinal",
	183, "Already exists",
	184, "No child process",
	185, "Child process is still alive",
	186, "Invalid flag number",
	187, "Semaphore not found",
	188, "Invalid starting code segment",
	189, "Invalid stack segment",
	190, "Invalid module type",
	191, "Invalid EXE signature",
	192, "EXE marked invalid",
	193, "Invalid EXE format",
	194, "Iterated data exceeds 64k",
	195, "Invalid minimum allocation size",
	196, "DYNLINK from invalid ring",
	197, "IOPL not enabled in CONFIG.SYS",
	198, "Invalid segment descriptor privilege level (SEGDPL)",
	199, "Automatic data segment exceeds 64k",
	200, "Ring 2 segment must be movable",
	201, "Relocation chain exceeds segment limit",
	202, "Infinite loop in relocation chain",
	203, "Environment variable not found",
	204, "Not current country",
	205, "No signal sent",
	206, "Filename exceeds size range",
	207, "Ring 2 stack in use",
	208, "Meta expansion too long",
	209, "Invalid signal number",
	210, "Thread 1 inactive",
	211, "File system information not available",
	212, "Locked",
	213, "Bad dynamic link",
	214, "Too many modules",
	215, "Nesting not allowed",
	216, "Cannot shrink Ring 2 stack",		/* Duncan */
#if defined INCLUDE_RESERVED
	217, "RESERVED",
	218, "RESERVED",
	219, "RESERVED",
	220, "RESERVED",
	221, "RESERVED",
	222, "RESERVED",
	223, "RESERVED",
	224, "RESERVED",
	225, "RESERVED",
	226, "RESERVED",
	227, "RESERVED",
	228, "RESERVED",
	229, "RESERVED",
#endif /* #if defined INCLUDE_RESERVED */
	230, "Bad pipe",
	231, "Pipe busy",
	232, "No data",
	233, "Pipe not connected",
	234, "More data",
#if defined INCLUDE_RESERVED
	235, "RESERVED",
	236, "RESERVED",
	237, "RESERVED",
	238, "RESERVED",
	239, "RESERVED",
#endif /* #if defined INCLUDE_RESERVED */
	240, "Virtual circuit disconnected",
#if defined INCLUDE_RESERVED
	241, "RESERVED",
	242, "RESERVED",
	243, "RESERVED",
	244, "RESERVED",
	245, "RESERVED",
	246, "RESERVED",
	247, "RESERVED",
	248, "RESERVED",
	249, "RESERVED",
	250, "RESERVED",
	251, "RESERVED",
	252, "RESERVED",
	253, "RESERVED",
	254, "RESERVED",
	255, "RESERVED",
	256, "RESERVED",
	257, "RESERVED",
	258, "RESERVED",
	259, "RESERVED",
	260, "RESERVED",
	261, "RESERVED",
#endif /* #if defined INCLUDE_RESERVED */
	262, "Stack too large",			/* Duncan */
#if defined INCLUDE_RESERVED
	263, "RESERVED",
	264, "RESERVED",
	265, "RESERVED",
	266, "RESERVED",
	267, "RESERVED",
	268, "RESERVED",
	269, "RESERVED",
	270, "RESERVED",
	271, "RESERVED",
	272, "RESERVED",
	273, "RESERVED",
	274, "RESERVED",
	275, "RESERVED",
	276, "RESERVED",
	277, "RESERVED",
	278, "RESERVED",
	279, "RESERVED",
	281, "RESERVED",
	282, "RESERVED",
	283, "RESERVED",
	284, "RESERVED",
	285, "RESERVED",
	286, "RESERVED",
	287, "RESERVED",
	288, "RESERVED",
	289, "RESERVED",
	290, "RESERVED",
	291, "RESERVED",
	292, "RESERVED",
	293, "RESERVED",
	294, "RESERVED",
	295, "RESERVED",
	296, "RESERVED",
	297, "RESERVED",
	298, "RESERVED",
	299, "RESERVED",
	300, "RESERVED",
	301, "RESERVED",
	302, "RESERVED",
#endif /* #if defined INCLUDE_RESERVED */
	303, "Invalid process ID",
	304, "Invalid priority level delta",
	305, "Not descendant",
	306, "Requestor not Session Manager",
	307, "Invalid priority class",
	308, "Invalid scope",
	309, "Invalid thread ID",
	310, "Cannot shrink DosSubSet segment",
	311, "Out of memory",
	312, "Memory block overlap",
	313, "Invalid size parameter",
	314, "Invalid flag parameter",
	315, "Invalid segment selector",
	316, "Message too long",
	317, "Message ID not found",
	318, "Unable to access message file",
	319, "Invalid message file format",
	320, "Invalid insertion variable count",
	321, "Unable to perform function",
	322, "Unable to wakeup",
	323, "Invalid system semaphore handle",
	324, "No timers available",
#if defined INCLUDE_RESERVED
	325, "RESERVED",
#endif /* #if defined INCLUDE_RESERVED */
	326, "Invalid timer handle",
	327, "Invalid date/time",
	328, "Internal system error",
	329, "Current queue name does not exist",
	330, "Currrent process not queue owner",
	331, "Current process owns queue",
	332, "Duplicate queue name",
	333, "Queue element does not exist",
	334, "Insufficient queue memory",
	335, "Invalid queue name",
	336, "Invalid queue priority",
	337, "Invalid queue handle",
	338, "Queue link not found",
	339, "Queue memory error",
	340, "Previous queue record was at end of queue",
	341, "Process has no access to queues",
	342, "Queue empty",
	343, "Queue name does not exist",
	344, "Queues not initialized",
	345, "Unable to access queues",
	346, "Unable to add new queue",
	347, "Unable to initialize queues",
#if defined INCLUDE_RESERVED
	348, "RESERVED",
#endif /* #if defined INCLUDE_RESERVED */
	349, "Invalid mask",
	350, "Invalid pointer to parameter",
	351, "VIO aptr",
	352, "VIO rptr",
	353, "VIO cptr",
	354, "VIO lptr",
	355, "Unsupported video mode",
	356, "Invalid cursor width value",
	357, "Invalid attribute",
	358, "Invalid row value",
	359, "Invalid column value",
	360, "Invalid top row value",
	361, "Invalid bottom row value",
	362, "Invalid right column value",
	363, "Invalid left column value",
	364, "SCS call",
	365, "SCS value",
	366, "Invalid wait flag setting",
	367, "Screen not previously locked",
	368, "SGS not Session Manager",

	/* BSEERR.H has 2 definitions for 369 through 371. */
	369, "Invalid session ID",
	370, "No sessions available",
	371, "Session not found",

	372, "Title cannot be changed",
	373, "Invalid parameter",
	374, "No device",
	375, "Invalid wait parameter",
	376, "Invalid length",
	377, "Invalid echo mode mask",
	378, "Invalid input mode mask",
	379, "Invalid monitor parameters",
	380, "Invalid device name",
	381, "Invalid device handle",
	382, "Buffer too small",
	383, "Buffer empty",
	384, "Data record too large",
	385, "No MOUSE device",
	386, "Invalid handle",
	387, "Invalid parameters",
	388, "Can't reset",
	389, "Invalid display parameters",
	390, "Invalid module",
	391, "Invalid entry point",
	392, "Invalid function mask",
	393, "No MOUSE no data",
	394, "No MOUSE pointer drawn",
	395, "Invalid frequency",
	396, "Cannot find COUNTRY.SYS file",
	397, "Cannot open COUNTRY.SYS file",

	/* BSEERR.H has 2 definitions for 398. */
	398, "No country or codepage",

	399, "NLS table truncated",
	400, "NLS invalid type",
	401, "NLS type not found",
	402, "Function for Task Manager only",
	403, "Invalid ASCIIZ string",
	404, "VioDeRegister not allowed",
	405, "No pop-up screen allocated",
	406, "Pop-up already on screen",
	407, "Function for Task Manager only",
	408, "Invalid ASCIIZ string",
	409, "Invalid replacement mask",
	410, "KbdRegister not allowed",
	411, "KbdDeRegister not allowed",
	412, "Function for Task Manager only",
	413, "Invalid ASCIIZ string",
	414, "Invalid replacement mask",
	415, "MouRegister not allowed",
	416, "MouDeRegister not allowed",
	417, "Invalid action",
	418, "INIT called more than once",
	419, "Screen group not found",
	420, "Caller is not shell",
	421, "Invalid parameters",
	422, "Save/restore function already owned",
	423, "Thread unblock by VioModeUndo or VioSavRedrawUndo",
	424, "Invalid function",
	425, "Function for Task Manager only",
	426, "VioRegister not allowed",
	427, "No VioModeWait thread exists",
	428, "No VioSavRedrawWait thread exists",
	429, "Invalid in background",
	430, "Illegal during popup",
	431, "Caller not baseshell",
	432, "Invalid status request",
	433, "Invalid wait parameter",
	434, "Cannot lock screen",
	435, "Invalid wait parameter",
	436, "Invalid handle",
	437, "Illegal during lock",
	438, "Invalid length",
	439, "Invalid Kbd handle",
	440, "No more Kbd handles",
	441, "Cannot create logical keyboard",
	442, "Code page load incomplete",
	443, "Invalid codepage ID",
	444, "No codepage support",
	445, "Kbd focus required",
	446, "Kbd focus already active",
	447, "Keyboard subsystem busy",
	448, "Invalid codepage",
	449, "Cannot get Kbd focus",
	450, "Session is not selectable",
	451, "Session not in foreground",
	452, "Session not parent of specified child",
	453, "Invalid session start mode",
	454, "Invalid session start option",
	455, "Invalid session bonding option",
	456, "Invalid session select option",
	457, "Session started in background",
	458, "Invalid session stop option",
	459, "Reserved parameters not zero",
	460, "Process not parent",
	461, "Invalid data length",
	462, "Parent session not bound",
	463, "Retry request block allocation",
	464, "Kbd unavailable for detached process",
	465, "Vio unavailable for detached process",
	466, "Mouse unavailable for detached process",
	467, "No font available to support mode",
	468, "User font active",
	469, "Invalid codepage",
	470, "System displays do not support codepage",
	471, "Current display does not support codepage",
	472, "Invalid code page",
	473, "Codepage list is too small",
	474, "Codepage not moved",
	475, "Mode switch initialization error",
	476, "Code page not found",
	477, "Unexpected slot returned",
	478, "Invalid trace option",
	479, "Internal resource error",
	480, "Shell initialization error",
	481, "No Task Manager hard errors",
	482, "Codepage switch incomplete",
	483, "Error during transparent pop-up",
	484, "Critical section overflow",
	485, "Critical section underflow",
	486, "Reserved parameter is not zero",
	487, "Invalid address",
	488, "Zero selectors requested",
	489, "Not enough selectors available",
	490, "Invalid selector",
	491, "Invalid program type",
	492, "Invalid program control",
	493, "Invalid program inheritance option",
	494, "Function not allowed in PM window",
	495, "Function not allowed in non-PM screen group",
	496, "Shield alreay owned",
	497, "No more Vio handles",
	498, "Vio error occurred, see error log",
	499, "Invalid display context",
	500, "No console",
	501, "No console",
	502, "Invalid Mouse handle",
	503, "Invalid debugging parms",
	504, "Function not allowed in PM window",
	505, "Function not allowed in non-PM screen group",
	506, "Invalid icon file"
} ;

/* Here are some messages that we will use to display the error. */
static UCHAR szUnexpectedMessage [] = "\r\n\aUnexpected error return " ;
static UCHAR szFromOS2Message [] = " from OS/2 function \"" ;
static UCHAR szParensMessage [] = " ()\".\r\n\"" ;
static UCHAR szProgramNameMsg [] = "\"\r\nCaller program:  \"" ;
static UCHAR szPeriod [] = "\"." ;
static UCHAR szFunctionNameMsg [] = "\r\nCaller function:  \"" ;
static UCHAR szNearMessage [] = "\r\nNear line number:  " ;
static UCHAR szPeriod1 [] = ".\r\n" ;
static UCHAR szNoErrorText [] = "(No error text available)" ;

	
	
void EXPENTRY errabort (PSZ pszProgramName, PSZ pszFunctionName,
			   PSZ pszCallerName, USHORT usLineNum, USHORT usReturn)
{
	USHORT cbChars ;		/* Characters we asked 'DosWrite ()' to write. */
	USHORT cbBytes ;		/* Characters actually written by 'DosWrite ()'. */
	USHORT i ;
	UCHAR szMessage [20] ;	/* A result buffer for 'myitoa ()'. */

	/* Nothing much to do here, really.  Just display some 'canned' text,
	 * adding caller-supplied text where appropriate, with the odd
	 * conversion (via 'myitoa ()') or strlen (via 'mystrlen ()').
	 *
	 * I decided to use 'DosWrite ()' instead of 'VioWrtTTY ()' so that
	 * these messages will be captured in the 'stderr' file even if it
	 * is redirected.  Again, performance is believed (by me) not to be
	 * an issue in this program because of its nature, but NEVER missing
	 * a message generated by it IS rather important.  If 'stderr' is not
	 * redirected (the usual case), OS/2 will convert these 'DosWrite ()'
	 * calls into 'VioWrtTTY ()' anyway.
	 */
	DosWrite (STDERR, szUnexpectedMessage, sizeof (szUnexpectedMessage) - 1,
	  &cbBytes) ;
	myitoa (usReturn, szMessage) ;
	if ((cbChars = mystrlen (szMessage)) > 0)
		DosWrite (STDERR, szMessage, cbChars, &cbBytes) ;
	DosWrite (STDERR, szFromOS2Message, sizeof (szFromOS2Message) - 1,
	  &cbBytes) ;
	if ((cbChars = mystrlen (pszFunctionName)) > 0)
		DosWrite (STDERR, pszFunctionName, cbChars, &cbBytes) ;
	DosWrite (STDERR, szParensMessage, sizeof (szParensMessage) - 1,
	  &cbBytes) ;
	for (i = 0 ; i <= sizeof (ErrorArray) ; ++i) {
		if (ErrorArray [i].usErrorNum == usReturn) {
			if ((cbChars = mystrlen (ErrorArray [i].pszErrorText)) > 0)
				DosWrite (STDERR, ErrorArray [i].pszErrorText,
				  cbChars, &cbBytes) ;
			break ;
		}
		else if (ErrorArray [i].usErrorNum > usReturn) {
			DosWrite (STDERR, szNoErrorText, sizeof (szNoErrorText) - 1,
			  &cbBytes) ;
			break ;
		}
	}
	DosWrite (STDERR, szProgramNameMsg, sizeof (szProgramNameMsg) - 1,
	  &cbBytes) ;
	if ((cbChars = mystrlen (pszProgramName)) > 0)
		DosWrite (STDERR, pszProgramName, cbChars, &cbBytes) ;
	DosWrite (STDERR, szPeriod, sizeof (szPeriod) - 1, &cbBytes) ;
	DosWrite (STDERR, szFunctionNameMsg, sizeof (szFunctionNameMsg) - 1,
	  &cbBytes) ;
	if ((cbChars = mystrlen (pszCallerName)) > 0)
		DosWrite (STDERR, pszCallerName, cbChars, &cbBytes) ;
	DosWrite (STDERR, szPeriod, sizeof (szPeriod) - 1, &cbBytes) ;
	DosWrite (STDERR, szNearMessage, sizeof (szNearMessage) - 1,
	  &cbBytes) ;
	myitoa (usLineNum, szMessage) ;
	if ((cbChars = mystrlen (szMessage)) > 0)
		DosWrite (STDERR, szMessage, cbChars, &cbBytes) ;
	DosWrite (STDERR, szPeriod1, sizeof (szPeriod1) - 1, &cbBytes) ;
	DosExit (EXIT_PROCESS, 1) ;
}


/* We don't want to include 'strlen ()', so we rolled our own.
 * Of course, the astute and experienced reader will recognize the
 * code as a variant of one of the three presented in the original
 * edition of K&R; however, they won't admit it, as it will show
 * their age ...
 */
static USHORT FAR PASCAL mystrlen (PSZ pszString)
{
	USHORT cbCount = 0 ;

	while (*pszString++)		/* Count characters. */
		++cbCount ;
	return cbCount ;
}


/* This private version of 'itoa ()' is not strictly compatible with
 * ANSI 'itoa ()', unlike the relationship between 'strlen ()' and
 * 'mystrlen ()' above.  If you ever decide to use the library
 * version instead of this one, it is not as simple as just changing
 * the name.  Be sure you look up 'itoa ()' first.
 */
static void FAR PASCAL myitoa (USHORT usNumber, PSZ pszMessage)
{
	USHORT usWork, i, j ;
	UCHAR ucChar ;

	usWork = usNumber ;
	i = 0 ;
	*pszMessage = '\0' ;

	/* First, generate digits in reverse order. */
	do {
		pszMessage [++i] = (UCHAR) (usWork % 10) + '0' ;
	} while ((usWork /= 10) != 0) ;

	/* Now reverse the string to correct the order. */
	usWork = i ;
	for (j = 0 ; j < usWork ; ++j, --i) {
		ucChar = pszMessage [i] ;
		pszMessage [i] = pszMessage [j] ;
		pszMessage [j] = ucChar ;
	}
}
