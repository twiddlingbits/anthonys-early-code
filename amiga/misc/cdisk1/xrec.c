#include <stdio.h>
#define ABORT -1
#define GOOD 0
#define BLKSIZE 1024*5
#define RETRY 5
#define NAK 999
#define SOH 4

/* XREC -- xmodem receive */
/* receives a file using the 'Modem7' protocol*/
/* Copyright 1986 by Anthony Wood */
/* last update 01-02-86 */


main()

{
FILE *fp,*out;

printf("Starting file transfer...\n");
fp=Open("SER:",1006);
out=Open("Download.file",1006);
xrec(fp,out);
printf("Finished file transfer.\n");
}


/*********************************/
/**  xmodem receive one file    **/
/*********************************/

xrec(ser,savefile)
FILE *ser,*savefile;
{
int byte,kount,rec,comp,chksum,error;
UBYTE blk[BLKSIZE];
top: /* send a NAK to sync, and wait for 1st SOH */
kount=0;
do {
   send(ser,NAK);
   printf("Trying to establish a communications link...\n");
   } while (kount++!=RETRY && (byte=receive(ser))==-1);
if (kount==RETRY) return(ABORT);
if (byte!=SOH) goto top;
next: /* SOH received, now get rest of block */
error=0;
do {
   if (error>1) send(ser,NAK);
   if ((rec=receive(ser))==-1) return(ABORT);  /* get blk # */
   if ((comp=receive(ser))==-1) return(ABORT); /* get blk 1's comp. */
   printf("Receiving block number %d\n",rec);
   kount=0;
   do while(kount!=BLKSIZE) {  /* get data and compute checksum */
      if ((blk(kount)=receive(ser))==-1) return(ABORT);
      cksum=cksum+blk(kount);
      }
   if((byte=receive(ser))==-1) return(ABORT);
   } while(error++!=RETRY && ((rec&comp)!=1 || cksum!=byte));
if (error==RETRY) return(ABORT);
Write(savefile,blk,blksize);   /* write blk to disk */
kount=0;
do {
   send(ser,ACK); /* send ack to signal 'send next block' */
   } while(kount++!=RETRY  && (byte=receive(ser))==-1);
if (kount==RETRY) return(ABORT);
if (byte==SOH) goto next;     /* if start of header, get new block */
if (byte!=EOT) return(ABORT); /* should only be 'end of transmision' */
send(ser,ACK);                /* signal 'eot received ok' */
return(GOOD);                 /* File received--nothing to it */
}

/********************/
/** send 1 byte    **/
/********************/

send(ser,byte)

FILE *ser;
UBYTE byte;
{
Write(ser,&byte,1);
}

/********************/
/** receive 1 byte **/
/********************/

receive(ser)
FILE *ser;
{
int here;
UBYTE byte;

here=WaitForChar(ser,1000000);   /* wait 1 second */
if (here==0) return (-1);
Read(ser,&byte,1);
return(byte);
}
