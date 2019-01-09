Here is the C code for the DUMP.C program to demonstrate semaphores,
multiple threads, etc. from high level.  



/*
        DUMP.C          Displays the binary contents of a file in
                        hex and ASCII on the standard output device.
 
                        Program has been deliberately complicated 
                        to demonstrate direct calls from C to 
                        operating system, use of multiple threads,
                        and synchronization with semaphores.
 
        Usage is:       C>DUMP unit:path\filename.ext [ >destination ]
 
        Compile with:   C>CL /AL /Zi /Gs /F 2000 DUMP.C
*/
 
#include <stdio.h>
#include <malloc.h>
#include <doscalls.h>
 
#define REC_SIZE 16                     /* size of file records */
#define STK_SIZE 1024                   /* stack size for threads */
 
char Buf1[REC_SIZE];                    /* first disk buffer */
unsigned Buf1Len;                       /* amount of data in buffer */
 
char Buf2[REC_SIZE];                    /* second disk buffer */
unsigned Buf2Len;                       /* amount of data in buffer */
 
unsigned Handle;                        /* file Handle from DOSOPEN */ 
long filptr;                            /* file offset in bytes */
 
unsigned long ExitSem;                  /* semaphore for process exit */
unsigned long Buf1FullSem;              /* semaphores for disk buffer #1 */
unsigned long Buf1EmptySem;
unsigned long Buf2FullSem;              /* semaphores for disk buffer #2 */
unsigned long Buf2EmptySem;
 
main(int argc, char *argv[])
{
    void far DisplayThr();              /* entry point for Display Thread */
    void far DiskThr();                 /* entry point for Disk Thread */
 
    unsigned DisplayThrID;              /* receives Thread ID */
    unsigned DiskThrID;                 /* receives Thread ID */
 
    char DisplayThrStk[STK_SIZE];       /* allocate stacks for threads */
    char DiskThrStk[STK_SIZE];
    int action;                         /* receives DOSOPEN result */
    int openflag=0x01;                  /* fail open if file not found */
    int openmode=0x40;                  /* read only, deny none */
 
    filptr=0L;                          /* initialize file pointer */
 
    ExitSem=0L;                         /* initialize semaphores */
    Buf1EmptySem=Buf1FullSem=0L;
    Buf2EmptySem=Buf2FullSem=0L;
    DOSSEMSET((long) &ExitSem);
    DOSSEMSET((long) &Buf1FullSem);
    DOSSEMSET((long) &Buf2FullSem);
 
    if (argc < 2)                       /* check command tail */
    {   fprintf(stderr,"\ndump: missing file name\n");
        exit(1);
    }
                                        /* open file or exit */
    if (DOSOPEN(argv[1],&Handle,&action,0L,0,openflag,openmode,0L)) 
    {   fprintf(stderr,"\ndump: can't find file %s\n",argv[1]);
        exit(1);
    }  
                                        /* create Disk Thread */
    if (DOSCREATETHREAD(DiskThr,&DiskThrID,DiskThrStk+STK_SIZE))
    {   fprintf(stderr,"\ndump: can't create Disk Thread");
        exit(1);
    }
                                        /* Create Display Thread */
    if (DOSCREATETHREAD(DisplayThr,&DisplayThrID,DisplayThrStk+STK_SIZE))
    {   fprintf(stderr,"\ndump: can't create Display Thread");
        exit(1);
    }
 
    DOSSEMWAIT((long) &ExitSem,-1L);    /* wait for exit signal */
 
    DOSSUSPENDTHREAD(DiskThrID);        /* suspend other threads */
    DOSSUSPENDTHREAD(DisplayThrID);
    DOSCLOSE(Handle);                   /* close file */
    DOSEXIT(1,0);                       /* terminate all threads */     
}
 
/* 
        The Disk Thread reads the disk file, alternating between 
        Buf1 and Buf2.  This thread gets terminated externally
        when the other threads see end of file has been reached.
*/
void far DiskThr()
{   
    while(1)    
    {   DOSREAD(Handle,Buf1,REC_SIZE,&Buf1Len); /* read disk */
        SemFlip(&Buf1EmptySem,&Buf1FullSem);    /* mark buffer 1 full */
        DOSSEMWAIT((long) &Buf2EmptySem,-1L);   /* wait for buffer 2 empty */
        DOSREAD(Handle,Buf2,REC_SIZE,&Buf2Len); /* read disk */
        SemFlip(&Buf2EmptySem,&Buf2FullSem);    /* mark buffer 2 full */
        DOSSEMWAIT((long) &Buf1EmptySem,-1L);   /* wait for buffer 1 empty */
    }
}
 
/* 
        The Display Thread formats and displays the data in the 
        disk buffers, alternating between Buf1 and Buf2.
*/
void far DisplayThr()
{   
    while(1)    
    {   DOSSEMWAIT((long) &Buf1FullSem,-1L);    /* wait for buffer 1 full */
        DumpRec(Buf1,Buf1Len);                  /* format and display it */     
        SemFlip(&Buf1FullSem,&Buf1EmptySem);    /* mark buffer 1 empty */
        DOSSEMWAIT((long) &Buf2FullSem,-1L);    /* wait for buffer 2 full */
        DumpRec(Buf2,Buf2Len);                  /* format and display it */     
        SemFlip(&Buf2FullSem,&Buf2EmptySem);    /* mark buffer 2 empty */
    }
}
 
/*
        Display record in hex and ASCII on standard output.
        Clear exit semaphore and terminate thread if record length=0.
*/
DumpRec(char *buffer,int length)
{   
    int i;                              /* index to current record */
 
    if (length==0)                      /* check if record length = 0 */
    {   DOSSEMCLEAR((long) &ExitSem);   /* yes, signal main thread  */
        DOSEXIT(0,0);                   /* and terminate this thread! */
    }
 
    if (filptr % 128 == 0)              /* maybe print heading */
        printf("\n\n       0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
 
    printf("\n%04lX ",filptr);          /* file offset */
 
    for (i = 0; i < length; i++)        /* print hex equiv. of each byte */
       printf( " %02X", (unsigned char) buffer[i] );
 
                                        /* space over if partial record */
    if (length != 16) for(i=0; i<(16-length); i++) printf("   ");
 
    printf("  ");
 
    for (i = 0; i < length; i++)        /* print ASCII equiv. of bytes */
        {  if (buffer[i] < 32 || buffer[i] > 126) putchar('.');
           else putchar(buffer[i]);
        }
 
    filptr += REC_SIZE;                 /* update file offset */
} 
 
/* 
        Since there is no operation to wait until a semaphore
        is set, we must maintain two semaphores to control each 
        buffer and flip them atomically.
*/
SemFlip(long *sem1, long *sem2)
{   DOSENTERCRITSEC();                  /* block other threads */
    DOSSEMSET((long) sem1);             /* set the first semaphore */
    DOSSEMCLEAR((long) sem2);           /* clear the second semaphore */
    DOSEXITCRITSEC();                   /* unblock other threads */
}

