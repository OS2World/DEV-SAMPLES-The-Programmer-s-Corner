 DUMP.ASM

        name    dump
        page    55,132
        title   DUMP --- Display File Contents
        .286c
;
; DUMP.ASM --- a OS/2 utility to display the contents of a 
; file on the standard output in hex and ASCII format.
;
; Copyright (C) 1987 Ray Duncan
;
; Usage:  C>DUMP path\filename.ext  [ >device ]
;
; This program has been intentionally complicated
; to demonstrate the use of multiple threads and semaphores
; in a MASM application.  For a roadmap to what is going
; on in this program, see its counterpart DUMP.C.
;

cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII line feed
blank   equ     20h             ; ASCII space code
tab     equ     09h             ; ASCII tab code

recsize equ     16              ; size of input file records
stksize equ     2048            ; size of stack for threads     

stdout  equ     1               ; handle of standard output device
stderr  equ     2               ; handle of standard error device

        extrn   DOSOPEN:far     ; references to OS/2 services
        extrn   DOSREAD:far
        extrn   DOSWRITE:far
        extrn   DOSCLOSE:far
        extrn   DOSEXIT:far
        extrn   DOSSEMCLEAR:far
        extrn   DOSSEMSET:far
        extrn   DOSSEMWAIT:far
        extrn   DOSALLOCSEG:far
        extrn   DOSCREATETHREAD:far
        extrn   DOSSUSPENDTHREAD:far
        extrn   DOSENTERCRITSEC:far
        extrn   DOSEXITCRITSEC:far
        extrn   DOSGETENV:far

DGROUP  group   _DATA

_DATA           segment word public 'DATA'

ExitSem         dd      0               ; storage for RAM semaphores
Buf1FullSem     dd      0
Buf1EmptySem    dd      0
Buf2FullSem     dd      0
Buf2EmptySem    dd      0

DisplayThrID    dw      0               ; Display thread ID
DiskThrID       dw      0               ; Disk I/O thread ID

Buf1            db      recsize dup (0) ; disk I/O buffer #1
Buf1Len         dw      0               ; length of buffer #1 data 

Buf2            db      recsize dup (0) ; disk I/O buffer #2    
Buf2Len         dw      0               ; length of buffer #2 data 

fname           db      64 dup (0)      ; ASCIIZ name of input file

fhandle         dw      0               ; handle for input file

filptr          dw      0               ; relative address in file 

status          dw      0               ; receives status of DOSOPEN

selector        dw      0               ; receives segment selector 
                                        ; from DOSALLOCSEG

                                        ; formatting area for output
output          db      'nnnn',blank,blank
outputa         db      16 dup ('nn',blank)
                db      blank
outputb         db      16 dup (blank),cr,lf
output_len      equ     $-output

heading         db      cr,lf           ; heading for each 128 bytes
                db      7 dup (blank)
                db      '0  1  2  3  4  5  6  7  '
                db      '8  9  A  B  C  D  E  F',cr,lf
heading_len     equ     $-heading

msg1            db      cr,lf
                db      'dump: file not found'
                db      cr,lf
msg1_len        equ     $-msg1

msg2            db      cr,lf
                db      'dump: missing file name'
                db      cr,lf
msg2_len        equ     $-msg2

msg3            db      cr,lf
                db      'dump: memory allocation error'
                db      cr,lf
msg3_len        equ     $-msg3   

msg4            db      cr,lf
                db      'dump: create thread failed'
                db      cr,lf
msg4_len        equ     $-msg4  

_DATA           ends    


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

dump    proc    far                     ; entry point from OS/2

        call    argc                    ; is filename present?
        cmp     ax,2
        je      dump1                   ; yes, proceed 

        mov     dx,offset msg2          ; missing or illegal filespec,
        mov     cx,msg2_len
        jmp     dump9                   ; print error message and exit.

dump1:                                  ; copy filename to local buffer
        mov     ax,1                    ; get ES:BX = filename
        call    argv
        mov     cx,ax                   ; set CX = length
        mov     di,offset fname         ; DS:DI = local buffer
dump15: mov     al,es:[bx]              ; copy it byte by byte
        mov     [di],al
        inc     bx
        inc     di
        loop    dump15  

        push    ds                      ; set ES = DS
        pop     es

dump2:                                  ; now try to open file...
        push    ds                      ; ASCIIZ file name
        push    offset DGROUP:fname
        push    ds                      ; receives handle
        push    offset DGROUP:fhandle
        push    ds                      ; receives handle
        push    offset DGROUP:status
        push    0                       ; file size (ignored)
        push    0
        push    0                       ; file attribute = normal
        push    1                       ; OpenFlag = fail if doesn't exist
        push    40h                     ; OpenMode = deny none,read only
        push    0                       ; DWORD reserved
        push    0
        call    DOSOPEN                 ; transfer to OS/2
        or      ax,ax                   ; test status
        jz      dump3                   ; jump if open succeeded

        mov     dx,offset msg1          ; open of input file failed,
        mov     cx,msg1_len
        jmp     dump9                   ; print error msg and exit.

dump3:                                  ; initialize semaphores
        push    ds
        push    offset DGROUP:ExitSem
        call    DOSSEMSET

        push    ds
        push    offset DGROUP:Buf1FullSem
        call    DOSSEMSET

        push    ds
        push    offset DGROUP:Buf2FullSem
        call    DOSSEMSET

                                        ; allocate Disk Thread stack
        push    stksize                 ; size of stack
        push    ds                      ; receives selector for
        push    offset DGROUP:selector  ;   allocated block
        push    0                       ; 0 = segment not shareable
        call    DOSALLOCSEG             ; transfer to OS/2
        or      ax,ax                   ; test status
        jz      dump5                   ; jump if allocation succeeded

dump4:  mov     dx,offset DGROUP:msg3   ; display message
        mov     cx,msg3_len             ; 'memory allocation error'
        jmp     dump9                   ; and exit

dump5:                                  ; create Disk Thread
        push    cs                      ; thread's entry point
        push    offset _TEXT:DiskThread
        push    ds                      ; receives thread ID
        push    offset DGROUP:DiskThrID 
        push    selector                ; thread's stack base
        push    stksize
        call    DOSCREATETHREAD         ; transfer to OS/2
        or      ax,ax                   ; test status
        jz      dump7                   ; jump if create succeeded

dump6:  mov     dx,offset DGROUP:msg4   ; create of thread failed,
        mov     cx,msg4_len             ; display error message
        jmp     dump9                   ; and exit

dump7:                                  ; allocate Display Thread stack
        push    stksize                 ; size of stack
        push    ds                      ; receives selector for
        push    offset DGROUP:selector  ;   allocated block
        push    0                       ; 0 = segment not shareable
        call    DOSALLOCSEG             ; transfer to OS/2
        or      ax,ax                   ; test status
        jnz     dump4                   ; jump if allocation failed

                                        ; create Display Thread
        push    cs                      ; thread's entry point
        push    offset _TEXT:DisplayThread
        push    ds                      ; receives thread ID
        push    offset DGROUP:DisplayThrID      
        push    selector                ; thread's stack base
        push    stksize
        call    DOSCREATETHREAD         ; transfer to OS/2
        or      ax,ax                   ; test status
        jnz     dump6                   ; jump if create failed

        push    ds                      ; now wait on exit semaphore
        push    offset DGROUP:ExitSem   ; (it will be triggered
        push    -1                      ; by routine DumpRec when
        push    -1                      ; end of file is reached)
        call    DOSSEMWAIT              ; transfer to OS/2

        push    DiskThrID               ; suspend Disk Thread
        call    DOSSUSPENDTHREAD        ; transfer to OS/2

        push    DisplayThrID            ; suspend Display Thread
        call    DOSSUSPENDTHREAD        ; transfer to OS/2

        push    fhandle                 ; close the input file
        call    DOSCLOSE                ; transfer to OS/2

        push    1                       ; terminate all threads
        push    0                       ; return code 0 for success
        call    DOSEXIT                 ; final exit to OS/2

dump9:                                  ; print error message...
        push    stderr                  ; standard error device handle
        push    ds                      ; address of message
        push    dx
        push    cx                      ; length of message
        push    ds                      ; receives bytes written
        push    offset DGROUP:status
        call    DOSWRITE                ; transfer to OS/2
        
        push    1                       ; terminate all threads
        push    1                       ; return code <>0 for error
        call    DOSEXIT                 ; final exit to OS/2
        
dump    endp


DiskThread proc far                     ; this thread performs 
                                        ; the file I/O, alternating
                                        ; between the two buffers

                                        ; fill buffer #1
        push    fhandle                 ; handle for input file
        push    ds                      ; address of buffer #1
        push    offset DGROUP:Buf1      
        push    recsize                 ; record length requested
        push    ds                      ; receives bytes read
        push    offset DGROUP: Buf1Len
        call    DOSREAD

                                        ; signal buffer 1 has data
        mov     si,offset DGROUP:Buf1EmptySem
        mov     di,offset DGROUP:Buf1FullSem
        call    SemFlip
        
        push    ds                      ; wait until buffer 2 empty
        push    offset DGROUP:Buf2EmptySem
        push    -1
        push    -1
        call    DOSSEMWAIT

                                        ; fill buffer #2
        push    fhandle                 ; handle for input file
        push    ds                      ; address of buffer #1
        push    offset DGROUP:Buf2
        push    recsize                 ; record length requested
        push    ds                      ; receives bytes read
        push    offset DGROUP:Buf2Len
        call    DOSREAD

                                        ; signal buffer 2 has data
        mov     si,offset DGROUP:Buf2EmptySem
        mov     di,offset DGROUP:Buf2FullSem
        call    SemFlip
        
        push    ds                      ; wait until buffer 1 empty
        push    offset DGROUP:Buf1EmptySem
        push    -1
        push    -1
        call    DOSSEMWAIT

        jmp     DiskThread              ; do it again...

DiskThread endp


DisplayThread proc far                  ; formats and displays disk
                                        ; data, alternating between
                                        ; the two disk buffers
        
        push    ds                      ; wait until buffer #1 full
        push    offset DGROUP:Buf1FullSem
        push    -1
        push    -1
        call    DOSSEMWAIT

        mov     si,offset DGROUP:Buf1   ; display buffer 1
        mov     cx,Buf1Len
        call    DumpRec

                                        ; signal buffer #1 is emptied
        mov     si,offset DGROUP:Buf1FullSem
        mov     di,offset DGROUP:Buf1EmptySem
        call    SemFlip
        
        push    ds                      ; wait until buffer #2 full
        push    offset DGROUP:Buf2FullSem
        push    -1
        push    -1
        call    DOSSEMWAIT

        mov     si,offset DGROUP:Buf2   ; display buffer 2
        mov     cx,Buf2Len
        call    DumpRec

                                        ; signal buffer #2 is emptied
        mov     si,offset DGROUP:Buf2FullSem
        mov     di,offset DGROUP:Buf2EmptySem
        call    SemFlip
        
        jmp     DisplayThread           ; do it again...        

DisplayThread endp


SemFlip proc    near                    ; Flip status of two 
                                        ; semaphores atomically

        call    DOSENTERCRITSEC         ; protect this code sequence

        push    ds                      ; set semaphore #1
        push    si
        call    DOSSEMSET

        push    ds                      ; clear semaphore #2
        push    di
        call    DOSSEMCLEAR

        call    DOSEXITCRITSEC          ; let other threads run again
        ret

SemFlip endp
 

DumpRec proc    near                    ; formats and displays 
                                        ; contents of buffer
                                        ; DS:SI = buffer, CX = length

        or      cx,cx                   ; anything to format?
        jnz     DumpRec1                ; yes, continue
        
        push    ds                      ; no, clear exit semaphore
        push    offset DGROUP:ExitSem   ; (releasing wait condition
        call    DOSSEMCLEAR             ; for main thread)

        push    0                       ; and terminate this thread
        push    0
        call    DOSEXIT

DumpRec1:                               ; time for a heading?
        test    filptr,07fh             ; if 128 byte boundary
        jnz     DumpRec2                ; no,jump

        push    stdout                  ; standard output device handle
        push    ds                      ; address of heading text
        push    offset DGROUP:heading
        push    heading_len             ; length of heading
        push    ds                      ; receives bytes written
        push    offset DGROUP:status
        call    DOSWRITE        

DumpRec2:                               ; format record data...
        push    cx                      ; save record length 

        mov     di,offset output        ; first clear output area
        mov     cx,output_len-2
        mov     al,blank
        rep stosb

        mov     di,offset output        ; convert current file offset
        mov     ax,filptr               ; to ASCII for output
        call    w2hex

        pop     cx                      ; get back record length
        mov     bx,0                    ; initialize record pointer

DumpRec3:                               ; fetch next byte from buffer
        mov     al,[si+bx]
                                        ; store ASCII version of character
        mov     di,offset outputb       ; calculate output string address
        mov     byte ptr [di+bx],'.'    ; if not alphanumeric
        cmp     al,blank                ; just print a dot.     
        jb      DumpRec4                ; jump, not alphanumeric.
        cmp     al,7eh          
        ja      DumpRec4                ; jump, not alphanumeric.
        mov     [di+bx],al              ; else store ASCII character.

DumpRec4:                               ; now convert binary byte
                                        ; to hex ASCII equivalent
        mov     di,offset outputa       ; calc. position in output string
        add     di,bx                   ; base addr + (offset*3)
        add     di,bx
        add     di,bx                   ; convert data in AL to hex
        call    b2hex                   ; ASCII and store into output

        inc     bx                      ; bump data pointer and loop
        loop    DumpRec3                ; until entire record converted

                                        ; now display formatted data
        push    stdout                  ; standard output device handle
        push    ds                      ; address of text
        push    offset DGROUP:output                    
        push    output_len              ; length of text
        push    ds
        push    offset DGROUP:status    ; receives bytes written
        call    DOSWRITE

        add     word ptr filptr,recsize ; update file pointer

        ret                             ; return to caller

DumpRec endp


argc    proc    near                    ; count command line arguments
                                        ; returns count in AX

        enter   4,0                     ; make room for local variables
                                        ; and give them names...
envseg  equ     [bp-2]                  ; environment segment
cmdoffs equ     [bp-4]                  ; command line offset   

        push    es                      ; save original ES,BX, and CX
        push    bx
        push    cx

        push    ss                      ; get selector for environment 
        lea     ax,envseg               ; and offset of command line 
        push    ax
        push    ss
        lea     ax,cmdoffs
        push    ax
        call    DOSGETENV               ; transfer to OS/2      
        or      ax,ax                   ; check operation status
        mov     ax,1                    ; force argc >= 1
        jnz     argc3                   ; inexplicable failure

        mov     es,envseg               ; set ES:BX = command line
        mov     bx,cmdoffs

argc0:  inc     bx                      ; ignore useless first field
        cmp     byte ptr es:[bx],0      
        jne     argc0

argc1:  mov     cx,-1                   ; set flag = outside argument

argc2:  inc     bx                      ; point to next character 
        cmp     byte ptr es:[bx],0
        je      argc3                   ; exit if null byte
        cmp     byte ptr es:[bx],blank
        je      argc1                   ; outside argument if ASCII blank
        cmp     byte ptr es:[bx],tab    
        je      argc1                   ; outside argument if ASCII tab

                                        ; otherwise not blank or tab,
        jcxz    argc2                   ; jump if already inside argument

        inc     ax                      ; else found argument, count it
        not     cx                      ; set flag = inside argument
        jmp     argc2                   ; and look at next character

argc3:  pop     cx                      ; restore original BX, CX, ES
        pop     bx
        pop     es
        leave                           ; discard local variables
        ret                             ; return AX = argument count

argc    endp


argv    proc    near                    ; get address and length
                                        ; of command line arguments
                                        ; call with AX = arg. no.
                                        ; return ES:BX = address of
                                        ; argument string, CX = length

        enter   4,0                     ; make room for local variables

        push    cx                      ; save original CX and DI 
        push    di

        push    ax                      ; save argument number

        push    ss                      ; get selector for environment 
        lea     ax,envseg               ; and offset of command line 
        push    ax
        push    ss
        lea     ax,cmdoffs
        push    ax
        call    DOSGETENV               ; transfer to OS/2      
        or      ax,ax                   ; test operation status
        pop     ax                      ; restore argument number
        jnz     argv7                   ; jump if DOSGETENV failed

        mov     es,envseg               ; set ES:BX = command line
        mov     bx,cmdoffs

        or      ax,ax                   ; is requested argument=0?
        jz      argv8                   ; yes, jump to get program name

argv0:  inc     bx                      ; scan off first field
        cmp     byte ptr es:[bx],0      
        jne     argv0

        xor     ah,ah                   ; initialize argument counter

argv1:  mov     cx,-1                   ; set flag = outside argument

argv2:  inc     bx                      ; point to next character 
        cmp     byte ptr es:[bx],0
        je      argv7                   ; exit if null byte
        cmp     byte ptr es:[bx],blank
        je      argv1                   ; outside argument if ASCII blank
        cmp     byte ptr es:[bx],tab    
        je      argv1                   ; outside argument if ASCII tab

                                        ; if not blank or tab...
        jcxz    argv2                   ; jump if already inside argument

        inc     ah                      ; else count arguments found
        cmp     ah,al                   ; is this the one we need?
        je      argv4                   ; yes, go find its length
        not     cx                      ; no, set flag = inside argument
        jmp     argv2                   ; and look at next character

argv4:                                  ; found desired argument, now
                                        ; determine its length...
        mov     ax,bx                   ; save param. starting address 

argv5:  inc     bx                      ; point to next character
        cmp     byte ptr es:[bx],0
        je      argv6                   ; found end if null byte
        cmp     byte ptr es:[bx],blank
        je      argv6                   ; found end if ASCII blank
        cmp     byte ptr es:[bx],tab    
        jne     argv5                   ; found end if ASCII tab

argv6:  xchg    bx,ax                   ; set ES:BX = argument address
        sub     ax,bx                   ; and AX = argument length
        jmp     argvx                   ; return to caller

argv7:  xor     ax,ax                   ; set AX = 0, argument not found
        jmp     argvx                   ; return to caller

argv8:                                  ; special handling for argv=0
        xor     di,di                   ; find the program name by
        xor     al,al                   ; first skipping over all the
        mov     cx,-1                   ; environment variables...
        cld
argv9:  repne scasb                     ; scan for double null (can't use
        scasb                           ; (SCASW since might be odd addr.)
        jne     argv9                   ; loop if it was a single null
        mov     bx,di                   ; save program name address
        mov     cx,-1                   ; now find its length... 
        repne scasb                     ; scan for another null byte
        not     cx                      ; convert CX to length 
        dec     cx
        mov     ax,cx                   ; return length in AX

argvx:                                  ; common exit point
        pop     di                      ; restore original CX and DI
        pop     cx
        leave                           ; discard stack frame
        ret                             ; return to caller

argv    endp


w2hex   proc    near                    ; convert word to hex ASCII
                                        ; call with AX=binary value
                                        ;           DI=addr to store string
                                        ; returns AX, DI destroyed
        push    ax
        mov     al,ah
        call    b2hex                   ; convert upper byte    
        pop     ax
        call    b2hex                   ; convert lower byte
        ret                             ; back to caller

w2hex   endp


b2hex   proc    near                    ; convert byte to hex ASCII
                                        ; call with AL=binary value
                                        ;           DI=addr to store string
                                        ; returns   AX, DI destroyed

        push    cx                      ; save CX for later
        sub     ah,ah                   ; clear upper byte
        mov     cl,16
        div     cl                      ; divide binary data by 16
        call    ascii                   ; the quotient becomes the first
        stosb                           ; ASCII character
        mov     al,ah
        call    ascii                   ; the remainder becomes the
        stosb                           ; second ASCII character
        pop     cx                      ; restore contents of CX
        ret

b2hex   endp


ascii   proc    near                    ; convert value 0-0FH in AL 
                                        ; into a "hex ASCII" character
        add     al,'0'                  
        cmp     al,'9'
        jle     ascii2                  ; jump if in range 0-9,
        add     al,'A'-'9'-1            ; offset it to range A-F,
ascii2: ret                             ; return ASCII char. in AL.

ascii   endp

_TEXT   ends
        
        end     dump
