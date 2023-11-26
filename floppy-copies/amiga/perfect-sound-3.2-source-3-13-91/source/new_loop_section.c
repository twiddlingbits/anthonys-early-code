
/* Changes the section of memory that is currently looping **/

void new_loop_section(chan, ptr, len)

int chan, len, ptr;

{
short delay;
unsigned char *base;
unsigned short *dmacon;
unsigned long  *aud0lch;
unsigned short *aud0len;
unsigned long  *aud1lch;
unsigned short *aud1len;
unsigned long  *aud2lch;
unsigned short *aud2len;
unsigned long  *aud3lch;
unsigned short *aud3len;

base=(unsigned char *)0xDFF000;
dmacon = (unsigned short *)(base+0x096);

/* channel 0 chip addresses */
aud0lch=(unsigned long *)(base+0x0A0);
aud0len=(unsigned short *)(base+0x0A4);

/* channel 1 chip addresses */
aud1lch=(unsigned long *)(base+0x0B0);
aud1len=(unsigned short *)(base+0x0B4);

/* channel 2 chip addresses */
aud2lch=(unsigned long *)(base+0x0C0);
aud2len=(unsigned short *)(base+0x0C4);

/* channel 3 chip addresses */
aud3lch=(unsigned long *)(base+0x0D0);
aud3len=(unsigned short *)(base+0x0D4);

len = len/2;
*dmacon = (unsigned short)(1<<chan);   /** clear channel's dma activity **/

switch (chan) {
   case 0:
      *aud0lch = ptr;
      *aud0len = (unsigned short)len;
      break;

   case 1:
      *aud1lch = ptr;
      *aud1len = (unsigned short)len;
      break;

   case 2:
      *aud2lch = ptr;
      *aud2len = (unsigned short)len;
      break;

   case 3:
      *aud3lch = ptr;
      *aud3len = (unsigned short)len;
      break;
   }
for (delay=0; delay < 100; delay++) ;  /** pause to reset pointers **/
*dmacon = (unsigned short)(0x0A000|(1<<chan));   /** turn on dma again **/
}

