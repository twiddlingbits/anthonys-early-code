/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define SetVolume	0x0001
#define SetFreq		0x0003
#define SetRate		0x0005
#define	SetTimer	0x0007

#define SetChannelVol	0x000F
#define SetVol0		0x000F
#define SetVol1		0x010F
#define SetVol2		0x020F
#define SetVol3		0x030F
#define SetVolMon	0x040F

/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define DelayOn		0x0011
#define DelayOff	0x0013
#define DelayBuff	0x0019

#define HILOoff		0x0415
#define HILOon		0x0417
#define SMPTEoff	0x0215
#define SMPTEon		0x0217
#define MonitorOff	0x001d
#define MonitorOn	0x001f
#define PlayOff		0x1015
#define PlayOn		0x1017
#define RecordOff	0x0035
#define RecordOn	0x0033

/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define VAR0lo		0x0021
#define VAR0hi		0x0023
#define VAR1lo		0x0025
#define VAR1hi		0x0027
#define VAR2lo		0x0029
#define VAR2hi		0x002B
#define VAR3lo		0x002D
#define VAR3hi		0x002F
/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define MovePlayPtr0	0x0031
#define MovePlayPtr1	0x0131
#define MovePlayPtr2	0x0231
#define MovePlayPtr3	0x0331

#define SMPTEreq	0x003B
/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define ModeOff		0x003d
#define	ModeOn		0x003e
#define	LoadCode	0x003f

#define RDOK68 0x0200
#define WROK68 0x0100
#define CLEAR	0
#define STATUS	0
#define DATA	1

#define HALF_CLK 5000000

#define		MsgRecData	0x0003
#define		MsgPlayData	0x0005
