{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{/////				Handler				/////}
{/////				for AD0516			/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
.module/ram/abs=0/boot=0		Handler;
.const		Srate=60;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.port		ModeSel;	{2000W}
.port 		Rate;		{2002W}
.port		Bank;		{2004W}
.port		Status;		{2006R}
.port		Read68;		{2008R}
.port		Write68;	{200AW}
.port		Interupt68;	{200cW}
.port		FIFO;		{200eWR}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.global		Status	,Read68;
.external	LoadCode,WaitRead;
.entry		Begin;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/pm/ram/abs=0x03f0 Sware;
.var/pm/ram/abs=0x03f1 EprmRev;
.var/pm/ram/abs=0x03f2 BoardRev;
.var/pm/ram/abs=0x03f3 Flags;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.init EprmRev: 0x000100;
{++++++++++++++++++++++++}
{++++++++++++++++++++++++}
.init BoardRev: 0x010000;
{ Board Revision 0xbb.bbcd   b=board revision c=AutoConfigpal(U15) rev  d=pal1(U24) rev}
{++++++++++++++++++++++++}
.init Flags: 0x000000;
{memory     bits  0-7       1=8k  4=32k  F=128k}
{filter     bits  8-11      0000=Bessel LTC1064-3  0001=Butterworth LTC1064-2}
{           bits 12-23     unused}
{++++++++++++++++++++++++}
.init Sware: 0x010005;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		REC_FIFO_EMPTY=0x0040;
.const		PLAY_FIFO_HALF_FULL=0x0020;
.const		REC_FIFO_HALF_FULL=0x0010;
.const		IN_TEST_RIG=0x0008;
.const		AMIGAINT=0x0004;
.const		WRITEOK=0x0002;
.const		READOK=0x0001;
.const		IntStat=0x0004;
.const		InTestRig=0x0008;
.const		SampRate=0x3FF1;
.const		SCLK=825;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		BLOCKSIZE=0x0400;
.const		BUFFERSIZE=0x1800;
.const		NUMBUFFERS=0x0006;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		MsgRecData=0x0009;
.const		MsgPlayData=0x000F;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram/circ/seg=extern_dm buffer[BUFFERSIZE];
.var/dm/ram/circ/seg=extern_dm InPointer;
.var/dm/ram/circ/seg=extern_dm DelayTime;
.var/dm/ram/circ/seg=extern_dm DelayFeedback;
.var/dm/ram/circ/seg=extern_dm Volume;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	PlayCntr;
.var/dm/ram	RecCntr;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/circ	CtrlIn[4];
.var/circ	CtrlOut[4];
.var/circ	DataIn[4];
.var/circ	DataOut[4];
.var/dm/ram	FirstFlg;
.var/dm/ram	DCBFlg;
.var/dm/ram	DmodeFlg;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	HiLo_Input;
.var/dm/ram	HiLo_Chan1;
.var/dm/ram	HiLo_Chan2;
.var/dm/ram	HiLo_Chan3;
.var/dm/ram	HiLo_Chan4;
.var/dm/ram	HiLo_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	Vol_Input;
.var/dm/ram	Vol_Chan1;
.var/dm/ram	Vol_Chan2;
.var/dm/ram	Vol_Chan3;
.var/dm/ram	Vol_Chan4;
.var/dm/ram	Vol_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	Val_Input;
.var/dm/ram	Val_Chan1;
.var/dm/ram	Val_Chan2;
.var/dm/ram	Val_Chan3;
.var/dm/ram	Val_Chan4;
.var/dm/ram	Val_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	RATE_OR;
.var/dm/ram	SAVE_RATE;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	SerTest;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	SAMPLCNTR; {4 SMPTESAMPLINGRATE}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	SM_TEST;
.const		SMPTEsyncword=0xBFFC;	{0011 1111 1111 1101}
.var/dm/ram	SMPTEmode;	{bak}	{1011 1111 1111 1100}
.var/dm/ram	SMPTEdata;

.var/dm/ram	SMPTEframe;
.var/dm/ram	SMPTEsec;
.var/dm/ram	SMPTEmin;

.var/dm/ram	SAVEframe;
.var/dm/ram	SAVEsec;
.var/dm/ram	SAVEmin;
.var/dm/ram	SAVEhour;

.var/dm/ram	SMPTEctr;

.var/dm/ram	SMPTEsecframe;
.var/dm/ram	SMPTEhourmin;
.var/dm/ram	OLDhourmin;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram BANKsave;
.MACRO ChangeBank(%0);
	ar=%0;
	DM(BANKsave)=ar;
	DM(Bank)=ar;
.ENDMACRO;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.MACRO Wait(%0);
.LOCAL Wat;
	ar=%0;
	af=PASS ar;
Wat:	ar=DM(Status);
	ar=ar and af;
	if EQ jump Wat;
.ENDMACRO;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.MACRO InteruptWrite(%0);
.LOCAL Wt;
	ar=WRITEOK;
	af=PASS ar;
Wt:	ar=DM(Status);
	ar=ar and af;
	if EQ jump Wt;

	DM(Write68)=%0;
	DM(Interupt68)=ar;

.ENDMACRO;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{		Macro trashes ar, px, i7		}
.MACRO RemoveEvent(%0);
	i7=%0;
	ar=0;
	px=0;
	PM(i7,m4)=ar;
.ENDMACRO;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{		Macro trashes ar, px, i7		}
.MACRO AddEvent(%0,%1);
	i7=%1;
	ar=PM(i7,m4);
	i7=%0;
	PM(i7,m4)=ar;
.ENDMACRO;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{////////////////////////////////////////////////////////////////////}
{/////			Interupt Vectors			/////}
{////////////////////////////////////////////////////////////////////}
Begin:	DIS SEC_REG;	ENA AR_SAT;	nop;		jump start;	{Reset Vector}
	rti;		ENA SEC_REG;	call Smpte;	DIS SEC_REG;	{DATA AND SMPTE}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	jump setupcontrol;rti;		rti;		rti;	{Transmit}
	ENA SEC_REG;	call ConversInt;DIS SEC_REG;	rti;	{Recieve}
	rti;		rti;		rti;		rti;	{Timer}
{////////////////////////////////////////////////////////////////////}
{/////			Start Execution				/////}
{////////////////////////////////////////////////////////////////////}
start:	call CntlReg_inits;	{ set up SPORTS, TIMER, etc }
	ar=DM(Read68);

	ar=0;
	DM(RATE_OR)=ar;
	ar=1;
	DM(SMPTEmode)=ar;
	DM(SMPTEctr)=ar;

{	ar=0;
	DM(SmpteCNTR)=ar;
	AddEvent(^SMSM,^SM_SMS);}

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Main_Loop:
SMSM:	nop;	{Call Hsmpte}
PBSM:	nop;	{Call Hplay - Play/Record Block Completed}
	ay1=DM(Status);ar=READOK;
	ar=ar and ay1;
	if EQ jump ML_Skip;
	call Command;	{READ DATA}
ML_Skip:call HLchanVAL;
	jump Main_Loop;

{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/			Get Command from 68000			/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Command:ay1=0x0001;
	ax0=DM(Read68);
	ar=ax0 and ay1;
	if EQ jump PlayFifo;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
	ar=ax0;
	sr=ASHIFT ar by 8 (LO);
	ay1=0x00FF;			{UNNESSECARY?}
	ar=sr1 and ay1;			{ ax1 = Data }
	ax1=ar;

	ar=ax0;
	sr=LSHIFT ar by -1 (LO);
	ay1=0x007F;
	ar=sr0 and ay1;			{ ar = command}

	ay1=^JumpTable;
	ar=ar+ay1;
	I7=ar;
	jump (I7);
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}

JumpTable:
	rts;			{0x__01}
rts;{	jump SetFreq;}		{0xbF03}
	jump SetSamplingRate;	{0x__05}
rts;{	jump SetTimer;}		{0x__07}
rts;{	jump GainUp;}		{0x__09}
rts;{	jump GainDown;}		{0x__0B}
rts;{	jump RecordSend;}	{0x__0D}
	rts;			{0x_C0f}

	rts;			{0x__11}
	rts;			{0x__13}
	jump PB_STATE;		{0x__15}
	jump SM_STATE;		{0x__17}
	jump MODE;		{0x__19}
	rts;			{0x__1B} 
	jump ChanVol;		{0x__1D}
	jump HL_SEND;		{0x__1F}

	rts;			{0xdd21}
	rts;			{0xdd23}
	rts;			{0xdd25}
	rts;			{0xdd27}
	rts;			{0xdd29  USED}
	rts;			{0xdd2B  USED}
	rts;			{0xdd2D  USED}
	rts;			{0xdd2F  USED}

	jump MovePlayPtr;	{0x__31}
	jump RecordInOn;	{0x__33}
	jump RecordOutOn;	{0x__35}
	jump RecordOff;		{0x__37}
rts;{	jump SMPTEreq;}		{0x__39}
	rts;			{0x__3B} 	{ MUST REMAIN UNUSED 4 error resyncing}
	jump SendRev;		{0x__3D}
	jump NewCode;		{0x--3f loadcode}
	jump Peek;		{0x__41}
	jump Poke;		{0x__43}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ChanVol:
	ay0=^Vol_Input;
	ar=ax1+ay0;
	i7=ar;
	Wait(READOK);
	ax0=DM(Read68);
	DM(i7,m4)=ax0;

	ar=ax1;
	ay0=5;
	ar=ar-ay0;
	if eq rts;

	ar=ax1;
	ar=PASS ar;
	if eq rts;

	ay0=1;
	ar=ax1-ay0;
	ChangeBank(ar);
	DM(Volume)=ax0;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SetSamplingRate:
	Wait(READOK);
	ar=DM(Read68);

	ay1=0x3FFF;
	ar=ar and ay1;
	DM(SAVE_RATE)=ar;

	ay1=DM(RATE_OR);
	ar=ar or ay1;
	DM(Rate)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
HLchanVAL:
	i6=^Val_Input;
	i7=^HiLo_Input;

	cntr=6;
	do HLop UNTIL CE;
	ar=DM(i6,m4);
	af= ABS ar;
	ar=dm(i7,m4);
	ar=ar-af;
	if GT jump Nxt;
	ar=PASS af;
	dm(i7,m4)=ar;
Nxt:	Modify(i7,m5);
HLOP:	Modify(i6,m5);

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
HL_SEND:
	i6=^HiLo_Input;
	i7=^Vol_Input;

	ax0=0;
	cntr=6;
	do HLoop UNTIL CE;
	sr0=DM(I6,m4);		{ Get HiLow Info }
	DM(I6,m5)=ax0;		{ Zero it out }

	my0=DM(i7,m5);
	MR=sr0*my0 (SU);
	if mv sat mr;

	Wait(WRITEOK);
	ar=abs mr1;
HLoop:	DM(Write68)=AR;	{ Write it }

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
MODE:	ar=ax1; ar= PASS ar;
	if NE jump ModeOn;
{	jump ModeOff;}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ModeOff:
	Call ControlMode;
	imask=b#000000;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ModeOn:	imask=b#000000;
	call DataMode;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ControlMode:
	   I7=0x3FFF;
{ 0x3FFF } DM(I7,M4)=0x0418;	{SerialOff}

	ax0=0;
	DM(ModeSel)=ax0;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
DataMode:
	Call ControlMode;

	I0=^CtrlIn;
	I1=^CtrlOut;

	ax0=DM(CtrlOut);
	ay1=B#1111101111111111;
	ar=ax0 and ay1;
	DM(CtrlOut)=ar;

	ar=DM(CtrlOut);
	DM(SerTest)=ar;

	ax0=1;
	DM(DCBFlg)=ax0;

	ax0=0;
	DM(DmodeFlg)=ax0;
	DM(FirstFlg)=ax0;


	I7=0x3fef;
{3fef}	DM(I7,M5)=B#0000001010000111; {0x0283}	{aport1 autobuffer cntrl reg}
{3ff0}	DM(I7,M5)=383;{383}		{rfsdiv0}
{3ff1}	DM(I7,M5)=SCLK;		{849;}		{sclkdiv0}
{3ff2}	DM(I7,M5)=B#0100000100011111; {sport1 cntr reg: internal sclk&rfs} {normal framing mode,} {frame sync not inverted} {16 bit word len}

	ICNTL=B#00000;
	IMASK=B#000100;		{only sport transmit int enabled}
	ax0=DM(I1,M1);		{send first 16 bits of ctrl word}
	TX1=ax0;

	I7=0x3fff;
{3fff}	DM(I7,M4)=0x0C18;	{system cntrl reg: sport 1 enabled}
{++++++++++++++++}
wait1:	ar=DM(DmodeFlg);
	ar=PASS ar;
	if EQ jump wait1;
{++++++++++++++++}
GO_DMODE:
	I0=^DataIn+2;
	I1=^DataOut+2;

	ax0=DM(I1,M1);TX1=ax0;
	ar=0xc18;DM(0x3FFF)=ar;
	IFC=B#000000111111;
	IMASK=B#000010;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{ sport1 tx interrupt routine 					     }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SETUPCONTROL:
	ar=DM(FirstFlg);
	ar=PASS ar;
	if NE jump JPNOTFIRST;		{JUMP occures on Second itteration}

	ax0=1;
	DM(FirstFlg)=ax0;
	rti;

JPNOTFIRST:
	ax0=DM(DCBFlg);
	ar=PASS ax0;
	if EQ Jump DCBFLG_SET;

	ax0=DM(CtrlIn);			{DCB flag has not been set yet}
	ay1=DM(SerTest);
	ar=ax0 XOR ay1;			{same as we sent out?}
	if NE rti;			{set flag if DCB was 0}

	ax0=0;
	DM(DCBFlg)=ax0;

	ax0=DM(CtrlOut);
	ay1=B#0000010000000000;
	ar=ax0 or ay1;
	DM(CtrlOut)=ar;
	rti;

DCBFLG_SET:
	ax0=DM(CtrlIn);			{DCBFlag was set}
	ay1=B#0000010000000000;
	ar=ax0 AND ay1;			{only check for DCB bit}
	if EQ rti;			{if DCB=1 ready for datamode}

	IMASK=0;
	ax0=0x0418;			{disable sport1}
	DM(0x3FFF)=ax0;

	ax0=0x001f;
	DM(0x3ff2)=ax0;			{sport 1 cntrl: internal tfs, external sclk & rfs}
	ax0=1;
	DM(ModeSel)=ax0;		{set D/C high}
	DM(DmodeFlg)=ax0;

	rti;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
NewCode:imask=b#000000;

	   I0=0x3FFF;
{ 0x3FFF } DM(I0,M1)=0x0418;	{BOOT PAGE 0}
	m7=1;l7=0;
	Wait(READOK);
	ar=DM(Read68);
	i7=ar;
	Wait(READOK);
	ar=DM(Read68);
	CNTR=ar;
	jump LoadCode;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Poke:	Wait(READOK);
	ar=DM(Read68);
	i7=ar;
	Wait(READOK);
	ar=DM(Read68);
	DM(i7,m4)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Peek:	Wait(READOK);
	ar=DM(Read68);
	i7=ar;
	Wait(WRITEOK);
	ar=DM(i7,m4);
	DM(Write68)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SendRev:
	ay1=WRITEOK;
	cntr=4;
	i7=0x03f0;

	do SPLoop UNTIL CE;
	ar=PM(I7,m5);

Waitt9:	ax1=DM(Status);
	af=ax1 and ay1;
	if EQ jump Waitt9;

	DM(Write68)=ar;

Waitt8:	ax1=DM(Status);
	af=ax1 and ay1;
	if EQ jump Waitt8;

	DM(Write68)=px;

SPLoop:	NOP;
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
CntlReg_inits:
{++++++++++++++++}
	ax0=0;
	DM(ModeSel)=ax0;

	ax0=0x120C;		{ 3.6K samp rate w/ 1.8432MHZ clk }
	DM(Rate)=ax0;		{ CLKIN=900*Rate @1.8432MHZ}
	DM(SAVE_Rate)=ax0;

{++++++++++++++++}

	L7=0;
	M5=1;
	I7=0x3fef;
{3fef}	DM(I7,M5)=B#0000001010000111; {0x0283}	{aport1 autobuffer cntrl reg}
{3ff0}	DM(I7,M5)=383;		{rfsdiv0}
{3ff1}	DM(I7,M5)=SCLK;		{sclkdiv0}
{3ff2}	DM(I7,M5)=B#0100000100011111; {sport1 cntr reg: internal sclk&rfs} {normal framing mode,} {frame sync not inverted} {16 bit word len}
	I7=0x3ffb;
{3ffb}	DM(I7,M5)=0x4000;	{TIMER TSCALE Scaling Registe}
{3ffc}	DM(I7,M5)=0x0001;	{TIMER TCOUNT Counting Regist}
{3ffd}	DM(I7,M5)=0xC000;	{TIMER TPERIOD Period Registe}
{3ffe}	DM(I7,M5)=0x1249;	{DM WAIT STATES one for each}{REDUNDANT!}

	I3=^buffer;

	M0=0;
	M1=1;
	M2=-1;
	M3=-1;
	M4=0;
	M5=1;
	M6=-1;
	M7=-1;

	L0=4;		{* DATA IN}
	L1=4;		{* DATA OUT}
	L2=0;		{* PLABACK VALUE}
	L3=BUFFERSIZE;	{* CURRENT PLAYBACK SAMPLE }
	L4=BUFFERSIZE;	{* FILL/RELIEVE BUFFERS}
	L5=0;	{* Used for smpte */
	L6=0;	{*}
	L7=0;	{*}

	I1=^CtrlOut;
	{                     DFR   } {DEFAULT CONTROL PARMS}
	DM(I1,M1)=B#0010000100101100; {DCB=0,AC=1,DFR=5,ST=1,DF=0}
	DM(I1,M1)=B#0011001000000000; {MCK=3,FSEL=0,MS=1,TXDIS=0,ENL=0,ADL=0}
	DM(I1,M1)=B#0000000000000000; {pio=0}
	DM(I1,M1)=B#0000000000000000; {REVID=NC}

	I1=^DataOut;
	DM(I1,M1)=B#1100000000000000;	{OM0=1,OM1=1,L0=0,SM=0,R0=0}
	DM(I1,M1)=B#1100000011110000;	{PI0=11,OVR=0,IS=0,LG=0,MA=15,RG=0}
	DM(I1,M1)=0x0000;
	DM(I1,M1)=0x0000;

	ar=0x8000;
	DM(Vol_Input)=ar;
	DM(Vol_Chan1)=ar;
	DM(Vol_Chan2)=ar;
	DM(Vol_Chan3)=ar;
	DM(Vol_Chan4)=ar;
	DM(Vol_Output)=ar;

	rts;

{////////////////////////////////////////////////////////////////////}
{rx int handler}
{////////////////////////////////////////////////////////////////////}
.var/dm/ram	OldBit;
.var/dm/ram	SaveI7;
.var/dm/ram	VFBsv;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				RECORD				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	MR1save;
RecOutInt:
	if mv sat mr;
	DM(MR1save)=mr1;
	my0=DM(Vol_Output);
	MR=mr1*my0 (SU);
	if mv sat mr;

	DM(I3,M0)=mr1;
	DM(i2,m1)=mr1;
	MR1=DM(MR1save);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecInInt:
	ar=DM(Val_Input);
	DM(I3,M0)=ar;

	DM(i2,m1)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
HrecordStart:
	i7=^HRcall;
	ar=PM(i7,m4);
	i7=^RI;			{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Hrecord:
	ar=DM(RecCntr);
	ar=PASS ar;
	if LE rts;

	ar=DM(Status);
	ay0=REC_FIFO_EMPTY;
	ar=ar and ay0;
	if NE rts;

	Call RecordSend;

	ax1=imask;
	imask=b#000000;		{ disable play interupt, so PlayCntr does not change}
	ay0=DM(RecCntr);
	ar=ay0-1;
	DM(RecCntr)=ar;
	imask=ax1;

	ax1=MsgRecData;
	InteruptWrite(ax1);

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordSend:
	ChangeBank(3);
	I4=DM(InPointer);
	CNTR=BlockSize;
	DO SendBs UNTIL CE;
	ar=DM(I4,m5);
SendBs:	DM(FIFO)=ar;
	DM(InPointer)=i4;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOff:
	i7=^PlayJump;
	ar=PM(i7,m4);
	i7=^PChan3;		{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	
	ar=0;px=0;
	i7=^RI;			{ I HATE writting self modifying code }
	PM(i7,m4)=ar;

	ChangeBank(3);
	ay1=0;
	I7=^buffer;
	L7=BUFFERSIZE;
	cntr=BUFFERSIZE;
	do ClearRec until CE;
	DM(i7,m5)=ay1;
ClearRec:nop;
	L7=0;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordInOn:
	i7=^RecordInJump;
	jump RecordOn;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOutOn:
	i7=^RecordOutJump;
	jump RecordOn;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOn:
	ChangeBank(3);
	DM(InPointer)=i3;

	ar=PM(i7,m4);
	i7=^PChan3;		{ I HATE writting self modifying code }
	PM(i7,m4)=ar;

	ar=-1;
	DM(RecCntr)=ar;

	i7=^HRcall;
	ar=PM(i7,m4);
	i7=^RI;			{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordInJump:	Call RecInInt;
RecordOutJump:	Call RecOutInt;
PlayJump:	Call StandardPlay;
HRScall:	Call HrecordStart;
HRcall:		Call Hrecord;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				PLAY				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
StandardPlay:
	ar=DM(I3,M0);
	my0=DM(Volume);
	DM(i2,m1)=ar;
	DM(I3,M0)=ay0		,	MR=MR+ar*my0 (SU);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
MovePlayPtr:
	ChangeBank(ax1);

	Wait(READOK);		{ Wait For Channel Data Cmd }
	ar=DM(Read68);

	sr=LSHIFT ar by -4 (lo);

	CNTR = sr0;
	I4=I3;
	DO PlayF8 UNTIL CE;
	ar=DM(FIFO);
PlayF8:	DM(I4,m5)=ar;

	DM(InPointer)=i4;

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
PlayFifo:{ax0 = raw data }
	ay0=0x000F;
	ar=ax0 and ay0;
	sr=LSHIFT ar by -1 (lo);
	ChangeBank(sr0);

	ar=ax0;
	sr=LSHIFT ar by -4 (lo);

	i4=DM(InPointer);

	CNTR = sr0;
	DO PlayF3 UNTIL CE;
	ar=DM(FIFO);
PlayF3:	DM(I4,m5)=ar;

	DM(InPointer)=i4;

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
	POP PC;	jump Main_Loop; {jump instead of RTS}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Hplay:				{NEEDED ???? }

RI:	nop;			{ Self Modifying code (which I hate)}
				{, Call to Block Trasfer Record Data}
	ar=DM(PlayCntr);
	ar=PASS ar;
	if EQ rts;

	ax1=imask;
	imask=b#000000;		{ disable play interupt, so PlayCntr does not change}
	ay0=DM(PlayCntr);
	ar=ay0-1;
	DM(PlayCntr)=ar;
	imask=ax1;

	ax1=MsgPlayData;
	InteruptWrite(ax1);

	jump Hplay;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SM_STATE:
	ar=ax1;
	ar=pass ar;

	if NE jump ADD_SMPTE_INTERUPT;
	ar=0;
	DM(RATE_OR)=ar;
	jump RESTOSM;

ADD_SMPTE_INTERUPT:
	ar=0x4000;
	DM(RATE_OR)=ar;
	rts;

RESTOSM:
	ay1=DM(SAVE_RATE);
	ar=ar or ay1;
	DM(RATE)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
PB_STATE:
	ar=ax1; ar= PASS ar;
	if NE jump PBon;
	RemoveEvent(^PBSM);
	rts;
PBon:	AddEvent(^PBSM,^PB_SMS);
	rts;

PB_SMS:	call Hplay;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}

{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				Conversion			 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ConversInt:
{нннннннннннннннннннннннннннннннн}
	DM(DataOut+2)=ax0;{DAC_L}
	DM(DataOut+3)=ax1;{DAC_R}
	ax0=DM(DataIn+2);{ADC_L}
	ax1=DM(DataIn+3);{ACC_R}
{нннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннн}
	ar=DM(BANKsave);
	DM(VFBsv)=ar;
	DM(SaveI7)=I7;
{нннннннннннннннннннннннннннннннн}
	mr=0;				{ Clear Mult (output) }
	i2=^Val_Input;
	ar=ax0;{Input}
{нннннннннннннннннннннннннннннннн}
	DM(i2,m1)=ar;{Val_Input}
	my0=DM(Vol_Input);		{Monitor}
	MR=MR+ar*my0 (SU);
{нннннннннннннннннннннннннннннннн}
	ar=I3;
	ay0=BLOCKSIZE;
	ar=ar and ay0;

	ay1=DM(OldBit);
	af=ar xor ay1;
	if EQ jump Pskip;

	DM(OldBit)=ar;
	ay0=DM(PlayCntr);ar=ay0+1;DM(PlayCntr)=ar;
	ay0=DM(RecCntr);ar=ay0+1;DM(RecCntr)=ar;
{нннннннннннннннннннннннннннннннн}
Pskip:	ay0=0;{ Do Not NonZero this register till modify}
	ar=0;	DM(Bank)=ar;
PChan0:	Call StandardPlay;	{SMC}

	ar=1;	DM(Bank)=ar;
PChan1:	Call StandardPlay;	{SMC}

	ar=2;	DM(Bank)=ar;
PChan2:	Call StandardPlay;	{SMC}

	ar=3;	DM(Bank)=ar;
PChan3:	Call StandardPlay;	{SMC}

	Modify(i3,m1);
{нннннннннннннннннннннннннннннннн}
	if mv sat mr;
	DM(Val_Output)=mr1;
	my0=DM(Vol_Output);
	MR=mr1*my0 (SU);

	if mv sat mr;
{нннннннннннннннннннннннннннннннн}
	ax0=MR1;	{DAC_L}
	ax1=MR1;	{DAC_R}
{нннннннннннннннннннннннннннннннн}
	ay1=DM(SAMPLCNTR);
	ar=ay1+1;
	DM(SAMPLCNTR)=ar;
{нннннннннннннннннннннннннннннннн}
	ar=DM(Status);
	ay0=0x0080;
	ar=ar and ay0;
	if EQ call Smpte;
{нннннннннннннннннннннннннннннннн}
	I7=DM(SaveI7);
	ar=DM(VFBsv);
	DM(Bank)=ar;
	rts;
{нннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				SMPTE				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	SMPTEdata1;
.var/dm/ram	SMPTEdata2;
.var/dm/ram	SMPTEdata3;
.var/dm/ram	SMPTEdata4;
.var/dm/ram	SMPTEdata5;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Smpte:
{*********************************}
	I7=^SMPTEdata1;
	se=16;
{*********************************}
	ar=DM(Bank);
	sr=LSHIFT ar by 15 (HI);

	ar=DM(I7,m4);
	sr=sr OR LSHIFT ar by -1 (hi);
	DM(I7,m5)=sr1,sr=LSHIFT sr0 (lo);

	ar=DM(I7,m4);
	sr=sr OR LSHIFT ar by -1 (hi);
	DM(I7,m5)=sr1,sr=LSHIFT sr0 (lo);

	ar=DM(I7,m4);
	sr=sr OR LSHIFT ar by -1 (hi);
	DM(I7,m5)=sr1,sr=LSHIFT sr0 (lo);

	ar=DM(I7,m4);
	sr=sr OR LSHIFT ar by -1 (hi);
	DM(I7,m5)=sr1,sr=LSHIFT sr0 (lo);

	ar=DM(I7,m4);
	sr=sr OR LSHIFT ar by -1 (hi);
	DM(I7,m5)=sr1;
{*********************************}
SinkSearch:
	ar=SMPTEsyncword;
	ay0=DM(SMPTEdata1);
	ar=ar-ay0;
	if NE rts;

	ar=DM(SMPTEdata2);DM(SAVEhour)=ar;
	ar=DM(SMPTEdata3);DM(SAVEmin)=ar;
	ar=DM(SMPTEdata4);DM(SAVEsec)=ar;
	ar=DM(SMPTEdata5);DM(SAVEframe)=ar;

	AddEvent(^SMSM,^SM_SMS);
	rts;
{*********************************}
{*********************************}
{*********************************}
{*********************************}
SM_SMS:	call Hsmpte;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Hsmpte:	ax0=DM(SAVEframe);
Frames:	ay0=0x0300;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;
	DM(SMPTEsecframe)=ar;

	ax0=DM(SAVEsec);
Secs:	ay0=0x0700;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;

	sr=LSHIFT ar by 6 (lo);
	ay0=DM(SMPTEsecframe);
	ar=sr0+ay0;
	sr=LSHIFT ar by 4 (lo);
	ay0=0x0003;
	ar=sr0 + ay0;
	DM(SMPTEsecframe)=ar;

	ax0=DM(SAVEmin);
Mins:	ay0=0x0700;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;
	DM(SMPTEhourmin)=ar;

	ax0=DM(SAVEhour);
Hours:	ay0=0x0300;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;

	sr=LSHIFT ar by 6 (lo);
	ay0=DM(SMPTEhourmin);
	ar=sr0+ay0;
	sr=LSHIFT ar by 4 (lo);
	ay0=0x0005;
	ar=sr0 + ay0;
	DM(SMPTEhourmin)=ar;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
DoSmpte:
	ax1=DM(SMPTEhourmin);
	ax0=DM(SMPTEsecframe);

	ay0=DM(OLDhourmin);
	ar=ax1-ay0;
	if eq jump FrSe;
	DM(OLDhourmin)=ay0;
	InteruptWrite(ax1);

Frse:	InteruptWrite(ax0);
	ar=0;
	ay0=imask;
	imask=0;
	ax1=DM(SAMPLCNTR);
	DM(SAMPLCNTR)=ar;
	imask=ay0;
	Wait(WRITEOK);
	DM(Write68)=ax1;

	RemoveEvent(^SMSM);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.endmod;
