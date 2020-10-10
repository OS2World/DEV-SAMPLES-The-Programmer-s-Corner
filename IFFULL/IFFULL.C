/*
	Run a program if running full screen in OS/2 v 1.1

	by Dick Lieber
	CIS ID 76267,425
	OnLine MS

	syntax:
		iffull [Program]

	Also sets error level to zero if full screen otherwise non-zero:

	0  Full-screen application
	1  Real-mode process
	2  VIO windowable application
	3  Presentation Manager application
	4  Detached application

	To build iffull use:
	cl /Lp iffull.c
	markexe /pmvio iffull.exe

*/
#define INCL_DOS
#include <os2.h>

int main( argc, argv )
int argc;
char *argv[];
{
	PLINFOSEG pInfoSeg;
	SEL selGlobalSeg, selLocalSeg;

	DosGetInfoSeg(&selGlobalSeg, &selLocalSeg);

	pInfoSeg = MAKEPLINFOSEG(selLocalSeg);

	if( pInfoSeg->typeProcess == 0 )
		if( argc > 1 )
			system( argv[1] );

#if DEBUG
	printf("\npInfoSeg->typeProcess: %d", pInfoSeg->typeProcess );
#endif
	exit( pInfoSeg->typeProcess );

}
