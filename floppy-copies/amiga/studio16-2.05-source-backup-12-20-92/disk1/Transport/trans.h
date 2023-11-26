/******* Transport Flags ********/
extern int Pflags;

#define RECORD_OUTPUT (1<<0)
#define MANUAL_RATE (1<<1)
#define MANUAL_VOLUME (1<<2)
#define SMPTE_START (1<<3)

#define AUTO_STOP (1<<4)
#define SMPTE_STOP (1<<(4+8))

