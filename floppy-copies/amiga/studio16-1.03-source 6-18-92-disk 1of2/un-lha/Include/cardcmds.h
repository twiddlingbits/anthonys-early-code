/** COPYRIGHT 1992 SunRize Industries **/

#define ON		0xFF00
#define OFF		0x0000
/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define ReadPM		0x0001
#define SetFreq		0x0003
#define SetRate		0x0005
#define	SetTimer	0x0007
#define GainUp		0x0009
#define GainDown	0x000B
#define RecDataReq	0x000D
/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define CONVERSION	0x0013
#define PLAY_BLOCK	0x0015
#define SMPTE		0x0017
#define MODE		0x0019
#define HiLowReq	0x001F

/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define RecordInOn	0x0033
#define RecordOutOn	0x0035
#define RecordOff	0x0037

/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define VAR0lo		0x0021
#define VAR0hi		0x0023
#define VAR1lo		0x0025
#define VAR1hi		0x0027
#define VAR2lo		0x0029
#define VAR2hi		0x002B
#define SetMix		0x0029
#define SetDelay2	0x002B
#define SetDelay	0x002D
#define SetFeedBack	0x002F
/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define MovePlayPtr0	0x0031
#define MovePlayPtr1	0x0131
#define MovePlayPtr2	0x0231
#define MovePlayPtr3	0x0331
/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define ChanVol		0x001d
#define ChanVol0	0x011d
#define ChanVol1	0x021d
#define ChanVol2	0x031d
#define ChanVol3	0x041d
/*[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]*/
#define SMPTEreq	0x0039
#define	GetRev		0x003d
#define	LoadCode	0x003f

#define RDOK68 0x0200
#define WROK68 0x0100
#define CLEAR	0
#define STATUS	0
#define DATA	1
#define FIFO	2

#define HALF_CLK 5000000

#define		MsgRecData	0x0003
#define		MsgPlayData	0x0005


#define		SerialNum	0x03f0
#define		BoardRev	0x03f1
#define		CFlags		0x03f2
#define		Xtra		0x03f3
