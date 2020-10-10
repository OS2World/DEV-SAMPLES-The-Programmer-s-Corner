/* Copyright 1988 Thomas R. Prodehl, Lattice, Inc.
Permission is granted for private non-commercial use only. */

/* This program starts an Idle class command line interpreter.  Useful for 
when you don't want to slow down detached compiles, for example.
Any program started under this shell will also have a default priority of
Idle. */

/* This program was compiled with the Lattice C Compiler 3.3. */

#include <doscalls.h>
#include <stdlib.h>
#include <os2.h>

unsigned long retcode;
unsigned ret;
char objname[64];

void main()
{
	ret = DOSSETPRTY(0, IDLE, 16, 0);
	ret = DOSEXECPGM((char far *)objname, 64, ASYNCH_NOSAVE,
			(char far *)0L, (char far *)0L,
			(unsigned long far *)&retcode,
			(char far *)"c:\\cmd.exe");
	if (ret) printf("exec program error %d\n", ret);
	DOSEXIT(1,0);
/* this program exits but the spawned shell continues. */
}
