
#ifndef HARDWARE_ADKBITS_H
#define HARDWARE_ADKBITS_H
#define  ADKB_SETCLR    15
#define  ADKB_PRECOMP1  14
#define  ADKB_PRECOMP0  13
#define  ADKB_MFMPREC   12
#define  ADKB_UARTBRK   11
#define  ADKB_WORDSYNC  10
#define  ADKB_MSBSYNC   9
#define  ADKB_FAST      8
#define  ADKB_USE3PN    7
#define  ADKB_USE2P3    6
#define  ADKB_USE1P2    5
#define  ADKB_USE0P1    4
#define  ADKB_USE3VN    3
#define  ADKB_USE2V3    2
#define  ADKB_USE1V2    1
#define  ADKB_USE0V1    0
#define  ADKF_SETCLR    (1<<15)
#define  ADKF_PRECOMP1  (1<<14)
#define  ADKF_PRECOMP0  (1<<13)
#define  ADKF_MFMPREC   (1<<12)
#define  ADKF_UARTBRK   (1<<11)
#define  ADKF_WORDSYNC  (1<<10)
#define  ADKF_MSBSYNC   (1<<9)
#define  ADKF_FAST      (1<<8)
#define  ADKF_USE3PN    (1<<7)
#define  ADKF_USE2P3    (1<<6)
#define  ADKF_USE1P2    (1<<5)
#define  ADKF_USE0P1    (1<<4)
#define  ADKF_USE3VN    (1<<3)
#define  ADKF_USE2V3    (1<<2)
#define  ADKF_USE1V2    (1<<1)
#define  ADKF_USE0V1    (1<<0)
#define ADKF_PRE000NS   0
#define ADKF_PRE140NS   (ADKF_PRECOMP0)
#define ADKF_PRE280NS   (ADKF_PRECOMP1)
#define ADKF_PRE560NS   (ADKF_PRECOMP0|ADKF_PRECOMP1)
#endif !HARDWARE_ADKBITS_H
