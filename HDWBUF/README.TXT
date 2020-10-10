This routine will turn on or off the Hardware buffering capabilities of 
NS16550-A UARTS when running with OS/2 1.2.  To use the program just 
invoke it as shown below:

hdwbuff COM1 ON
or
hdwbuff COM2 OFF

The first param is the com port to use.  The second param is the operation
to do. ( ON, or OFF ).  

This program must be invoked from OS/2 before you invoke the DOS mode serial
program.  If you have any comments please make them known in the Programming
forum of PCMAGNET.  

Paul Montgomery CIS 71500,3525
