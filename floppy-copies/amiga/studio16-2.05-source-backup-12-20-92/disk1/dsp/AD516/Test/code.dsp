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
.var/dm/ram/circ/seg=extern_dm VolumeLeft;
.var/dm/ram/circ/seg=extern_dm VolumeRight;
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
.var/dm/ram	HiLo_LEFT_Input;
.var/dm/ram	HiLo_LEFT_Chan1;
.var/dm/ram	HiLo_LEFT_Chan2;
.var/dm/ram	HiLo_LEFT_Chan3;
.var/dm/ram	HiLo_LEFT_Chan4;
.var/dm/ram	HiLo_LEFT_Output;

.var/dm/ram	HiLo_RIGHT_Input;
.var/dm/ram	HiLo_RIGHT_Chan1;
.var/dm/ram	HiLo_RIGHT_Chan2;
.var/dm/ram	HiLo_RIGHT_Chan3;
.var/dm/ram	HiLo_RIGHT_Chan4;
.var/dm/ram	HiLo_RIGHT_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	Vol_LEFT_Input;
.var/dm/ram	Vol_LEFT_Chan1;
.var/dm/ram	Vol_LEFT_Chan2;
.var/dm/ram	Vol_LEFT_Chan3;
.var/dm/ram	Vol_LEFT_Chan4;
.var/dm/ram	Vol_LEFT_Output;
.var/dm/ram	Vol_RIGHT_Input;
.var/dm/ram	Vol_RIGHT_Chan1;
.var/dm/ram	Vol_RIGHT_Chan2;
.var/dm/ram	Vol_RIGHT_Chan3;
.var/dm/ram	Vol_RIGHT_Chan4;
.var/dm/ram	Vol_RIGHT_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	Val_LEFT_Input;
.var/dm/ram	Val_LEFT_Chan1;
.var/dm/ram	Val_LEFT_Chan2;
.var/dm/ram	Val_LEFT_Chan3;
.var/dm/ram	Val_LEFT_Chan4;
.var/dm/ram	Val_LEFT_Output;

.var/dm/ram	Val_RIGHT_Input;
.var/dm/ram	Val_RIGHT_Chan1;
.var/dm/ram	Val_RIGHT_Chan2;
.var/dm/ram	Val_RIGHT_Chan3;
.var/dm/ram	Val_RIGHT_Chan4;
.var/dm/ram	Val_RIGHT_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	RATE_OR;
.var/dm/ram	SAVE_RATE;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	SerTest;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	SAMPLCNTR; {4 SMPTESAMPLINGRATE}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}

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
	rti;		ENA SEC_REG;	call SmpteInterupt;	DIS SEC_REG;	{DATA AND SMPTE}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	jump setupcontrol;rti;		rti;		rti;	{Transmit}
	ENA SEC_REG;	call ConversInt;DIS SEC_REG;	rti;	{Recieve}
	rti;		rti;		rti;		rti;	{Timer}
{////////////////////////////////////////////////////////////////////}
{/////			Start Execution				/////}
{////////////////////////////////////////////////////////////////////}
start:

	i7=^buffer;
	L7=BUFFERSIZE;
	M5=1;

	ar=0;
	ay0=0x000F;
	ax0=0;

	cntr=4;

	do BankLup UNTIL CE;
	ChangeBank(cntr);

	cntr=BUFFERSIZE;
	do BeepLup UNTIL CE;

	ar=ar+ay0;
	ax0=ar;
	ay1=0x0FFF;
	ar=ar and ay1;
	DM(i7,m5)=ar;

	ay1=ax0;
	af=ar-ay1;
	if EQ jump BPskip;

	si=ar;
	ar=ay0+1;
	ay0=ar;
	ar=si;
BPskip:

BeepLup:nop;

BankLup:nop;

	call CntlReg_inits;	{ set up SPORTS, TIMER, etc }
	ar=DM(Read68);

	ar=0;
	DM(RATE_OR)=ar;

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
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ChanVol:
	ay0=^Vol_LEFT_Input;
	ar=ax1+ay0;
	i7=ar;

	Wait(READOK);
	ax0=DM(Read68);		{LEFT}
	DM(i7,m6)=ax0;

	Wait(READOK);
	ay0=DM(Read68);		{RIGHT}
	DM(i7,m6)=ay0;

	ar=ax1;
	ay1=5;
	ar=ar-ay1;
	if eq rts;

	ar=ax1;
	ar=PASS ar;
	if eq rts;

	ay1=1;
	ar=ax1-ay1;
	ChangeBank(ar);
	DM(VolumeLeft)=ax0;
	DM(VolumeRight)=ay0;
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
	i6=^Val_LEFT_Input;
	i7=^HiLo_LEFT_Input;

	cntr=12;
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
	i6=^HiLo_LEFT_Input;
	i7=^Vol_LEFT_Input;

	ax0=0;
	cntr=6;
	do HLoop UNTIL CE;
	sr0=DM(I6,m4);		{ Get HiLow Info }
	DM(I6,m6)=ax0;		{ Zero it out }

	my0=DM(i7,m6);
	MR=sr0*my0 (SU);
	if mv sat mr;

	Wait(WRITEOK);
	ar=abs mr1;
	DM(Write68)=AR;	{ Write Left}
{-----------RIGHT--------------}
	sr0=DM(I6,m4);		{ Get HiLow Info }
	DM(I6,m7)=ax0;		{ Zero it out }

	my0=DM(i7,m7);
	MR=sr0*my0 (SU);
	if mv sat mr;

	Wait(WRITEOK);
	ar=abs mr1;
HLoop:	DM(Write68)=AR;	{ Write Right}

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
	Wait(WRITEOK);
	DM(Write68)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ModeOn:
	imask=b#000000;
	call DataMode;
	Wait(WRITEOK);
	DM(Write68)=ar;
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
	my0=DM(Vol_LEFT_Output);
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
	ar=DM(Val_LEFT_Input);
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
	I7=0x3FFF;
	DM(I7,M4)=0x0C18;	{SerialOff}
	rts;


	i7=^PlayJump;
	ar=PM(i7,m4);
	i7=^LChan3;		{ I HATE writting self modifying code }
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

	I7=0x3FFF;
	DM(I7,M4)=0x0418;	{SerialOff}
	rts;

	i7=^RecordInJump;
	jump RecordOn;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOutOn:
	I7=0x3FFF;
	DM(I7,M4)=0x0018;	{SerialOff}
	rts;

	i7=^RecordOutJump;
	jump RecordOn;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOn:
	ChangeBank(3);
	DM(InPointer)=i3;

	ar=PM(i7,m4);
	i7=^LChan3;		{ I HATE writting self modifying code }
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
PlayJump:	Call StandardPlayLeft;
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
{  XX				SMPTE				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		SMPTEsyncword=0xBFFC;	{0011 1111 1111 1101}

.var/dm/ram	SMPTEdata1;
.var/dm/ram	SMPTEdata2;
.var/dm/ram	SMPTEdata3;
.var/dm/ram	SMPTEdata4;
.var/dm/ram	SMPTEdata5;
.var/dm/ram	SMPTEcntr;

.var/dm/ram	SAVEframe;
.var/dm/ram	SAVEsec;
.var/dm/ram	SAVEmin;
.var/dm/ram	SAVEhour;

.var/dm/ram	SMPTEsecframe;
.var/dm/ram	SMPTEhourmin;
.var/dm/ram	OLDhourmin;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SmpteInterupt:
{*********************************}
	I7=^SMPTEdata1;
	se=16;
{*********************************}
	ar=DM(Bank);			{ GET NEW SMPTE BIT }
	sr=LSHIFT ar by 15 (HI);
{*********************************}
	ar=DM(I7,m4);			{SMPTEdata1}
	sr=sr OR LSHIFT ar by -1 (hi);
	DM(I7,m5)=sr1,sr=LSHIFT sr0 (lo);

	ar=DM(I7,m4);			{SMPTEdata2}
	sr=sr OR LSHIFT ar by -1 (hi);
	DM(I7,m5)=sr1,sr=LSHIFT sr0 (lo);

	ar=DM(I7,m4);			{SMPTEdata3}
	sr=sr OR LSHIFT ar by -1 (hi);
	DM(I7,m5)=sr1,sr=LSHIFT sr0 (lo);

	ar=DM(I7,m4);			{SMPTEdata4}
	sr=sr OR LSHIFT ar by -1 (hi);
	DM(I7,m5)=sr1,sr=LSHIFT sr0 (lo);

	ar=DM(I7,m4);			{SMPTEdata5}
	sr=sr OR LSHIFT ar by -1 (hi);
	DM(I7,m5)=sr1;
{*********************************}
	ay0=DM(I7,m4);			{SMPTEcntr}
	ar=ay0+1;
	DM(I7,m5)=ar;
{*********************************}
SinkSearch:
	ar=SMPTEsyncword;
	ay0=DM(SMPTEdata1);
	ar=ar-ay0;
	if EQ jump ValidSmpte;
{*********************************}
	ar=DM(SMPTEcntr);
	ay0=85;
	ar=ar-ay0;
	if NE rts;
{*********************************}
InValidSmpte:
	ar=0;DM(SMPTEcntr)=ar;
	AddEvent(^SMSM,^SM_SERR);
	rts;
{*********************************}
ValidSmpte:
	ar=DM(SMPTEdata2);DM(SAVEhour)=ar;
	ar=DM(SMPTEdata3);DM(SAVEmin)=ar;
	ar=DM(SMPTEdata4);DM(SAVEsec)=ar;
	ar=DM(SMPTEdata5);DM(SAVEframe)=ar;
	ar=0;DM(SMPTEcntr)=ar;
	AddEvent(^SMSM,^SM_SMS);
	rts;
{*********************************}
SM_SMS:	call Hsmpte;
SM_SERR:call SmpteError;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Hsmpte:
{--------------------------------}
	ax0=DM(SAVEframe);
Frames:	ay0=0x0300;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;
	DM(SMPTEsecframe)=ar;
{--------------------------------}
	ax0=DM(SAVEsec);
Secs:	ay0=0x0700;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;
{--------------------------------}
	sr=LSHIFT ar by 6 (lo);
	ay0=DM(SMPTEsecframe);
	ar=sr0+ay0;
	sr=LSHIFT ar by 4 (lo);
	ay0=0x0003;
	ar=sr0 + ay0;
	DM(SMPTEsecframe)=ar;
{--------------------------------}
	ax0=DM(SAVEmin);
Mins:	ay0=0x0700;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;
	DM(SMPTEhourmin)=ar;
{--------------------------------}
	ax0=DM(SAVEhour);
Hours:	ay0=0x0300;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;
{--------------------------------}
	sr=LSHIFT ar by 6 (lo);
	ay0=DM(SMPTEhourmin);
	ar=sr0+ay0;
	sr=LSHIFT ar by 4 (lo);
	ay0=0x0005;
	ar=sr0 + ay0;
	DM(SMPTEhourmin)=ar;
{--------------------------------}
SendSmpte:
	ax1=DM(SMPTEhourmin);
	ax0=DM(SMPTEsecframe);
{--------------------------------}
SendHourMins:
	ay0=DM(OLDhourmin);
	ar=ax1-ay0;
	if eq jump SendSecFrames;
	InteruptWrite(ax1);
	DM(OLDhourmin)=ay0;
{--------------------------------}
SendSecFrames:
	InteruptWrite(ax0);
{--------------------------------}
	ar=0;
	ay0=imask;
	imask=0;
	ax1=DM(SAMPLCNTR);
	DM(SAMPLCNTR)=ar;
	imask=ay0;
	Wait(WRITEOK);
	DM(Write68)=ax1;
{--------------------------------}
	RemoveEvent(^SMSM);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SmpteError:
	ax1=0xFFF3;
	InteruptWrite(ax1);
{--------------------------------}
	ar=0;
	ay0=imask;
	imask=0;
	ax1=DM(SAMPLCNTR);
	DM(SAMPLCNTR)=ar;
	imask=ay0;
	Wait(WRITEOK);
	DM(Write68)=ax1;
{--------------------------------}
	RemoveEvent(^SMSM);
	rts;
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
	ar=DM(BANKsave);
	DM(VFBsv)=ar;
	DM(SaveI7)=I7;
{нннннннннннннннннннннннннннннннн}
	i2=^Val_LEFT_Input;
{нннннннннннннннннннннннннннннннн}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{||                      LEFT                         ||}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
	DM(i2,m1)=ax0;{Val_LEFT_Input}
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
Pskip:
	ar=ax0;{Input LEFT}
	my0=DM(Vol_LEFT_Input);		{Monitor}
	MR=ar*my0 (SU);
{нннннннннннннннннннннннннннннннн}
	ay0=0;		{ Do Not NonZero this register till modify}
	DM(Bank)=ay0;
LChan0:	Call StandardPlayLeft;	{SMC}

	ar=1;	DM(Bank)=ar;
LChan1:	Call StandardPlayLeft;	{SMC}

	ar=2;	DM(Bank)=ar;
LChan2:	Call StandardPlayLeft;	{SMC}

	ar=3;	DM(Bank)=ar;
LChan3:	Call StandardPlayLeft;	{SMC}

{	Modify(i3,m1);}
{нннннннннннннннннннннннннннннннн}
	if mv sat mr;
	DM(i2,m1)=mr1;	{Val_LEFT_Output}
	my0=DM(Vol_LEFT_Output);
	MR=mr1*my0 (SU);
	if mv sat mr;
	ax0=MR1;	{DAC_L}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{||                      RIGHT                        ||}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
	DM(i2,m1)=ax1;	{Val_RIGHT_Input}
	ar=ax1;	{Input RIGHT}
	my0=DM(Vol_RIGHT_Input);		{Monitor}
	MR=ar*my0 (SU);
{нннннннннннннннннннннннннннннннн}
	ay0=0;		{ Do Not NonZero this register till modify}
	DM(Bank)=ay0;
RChan0:	Call StandardPlayRight;	{SMC}

	ar=1;	DM(Bank)=ar;
RChan1:	Call StandardPlayRight;	{SMC}

	ar=2;	DM(Bank)=ar;
RChan2:	Call StandardPlayRight;	{SMC}

	ar=3;	DM(Bank)=ar;
RChan3:	Call StandardPlayRight;	{SMC}

	Modify(i3,m1);
{нннннннннннннннннннннннннннннннн}
	if mv sat mr;
	DM(i2,m1)=mr1; {Val_RIGHT_Output}
	my0=DM(Vol_RIGHT_Output);
	MR=mr1*my0 (SU);
	if mv sat mr;
	ax1=MR1;	{DAC_R}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{||                      OTHER                        ||}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннн}
	ay1=DM(SAMPLCNTR);
	ar=ay1+1;
	DM(SAMPLCNTR)=ar;
{нннннннннннннннннннннннннннннннн}
	ar=DM(Status);
	ay0=0x0080;
	ar=ar and ay0;
	if EQ call SmpteInterupt;
{нннннннннннннннннннннннннннннннн}
	I7=DM(SaveI7);
	ar=DM(VFBsv);
	DM(Bank)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
StandardPlayLeft:
	ar=DM(I3,M0);		{GET SAMP}
	my0=DM(VolumeLeft);	{GET VOL}
	DM(i2,m1)=ar {4 HILO} , MR=MR+ar*my0 (SU);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
StandardPlayRight:
	ar=DM(I3,M0);		{GET SAMP}
	my0=DM(VolumeRight);	{GET VOL}
	DM(i2,m1)=ar;		 {4 HILO}
	DM(I3,M0)=ay0 {ZERO SAMP} , MR=MR+ar*my0 (SU);
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
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX			Control/Data Mode			 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ControlMode:
	I7=0x3FFF;
	DM(I7,M4)=0x0418;	{SerialOff}

	ax0=0;
	DM(ModeSel)=ax0;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	DMcntr;
DataMode:
	Call ControlMode;

	ar=0x2FFF;Call Delay;
	ar=0x2FFF;Call Delay;

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
	l7=0;
{3fef}	DM(I7,M5)=B#0000001010000111; {0x0283}	{aport1 autobuffer cntrl reg}
{3ff0}	DM(I7,M5)=383;{383}		{rfsdiv0}
{3ff1}	DM(I7,M5)=SCLK;	{849;}		{sclkdiv0}
{3ff2}	DM(I7,M5)=B#0100000100011111; {sport1 cntr reg: internal sclk&rfs} {normal framing mode,} {frame sync not inverted} {16 bit word len}

	ICNTL=B#00000;
	IMASK=B#000100;		{only sport transmit int enabled}
	ax0=DM(I1,M1);		{send first 16 bits of ctrl word}
	TX1=ax0;

	I7=0x3fff;
{3fff}	DM(I7,M4)=0x0C18;	{system cntrl reg: sport 1 enabled}
{++++++++++++++++}
	ar=0x7F00; DM(DMcntr)=ar;

wait1:
	ay0=DM(DMcntr);
	ar=ay0-1;
	if EQ jump DataMode;
	DM(DMcntr)=ar;

	ar=0xFF;Call Delay;
	ar=DM(DmodeFlg);
	ar=PASS ar;
	if EQ jump wait1;
{++++++++++++++++}
GO_DMODE:
	ar=0x2FFF;Call Delay;
	I0=^DataIn+2;
	I1=^DataOut+2;

	ax0=DM(I1,M1);TX1=ax0;
	IFC=B#000000111111;
	IMASK=B#000010;


	I7=0x3FFF;
	DM(I7,M4)=0x0418;	{SerialOff}

	I7=0x3FF2;
{	DM(I7,M5)=B#0100000100011111;}
{3ff2}	DM(I7,M5)=B#0000000100011111; {sport1 cntr reg: internal sclk&rfs} {normal framing mode,} {frame sync not inverted} {16 bit word len}

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
	ax0=0x0C18;			{disable sport1}
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
Delay:
	cntr=ar;
	DO DelayLp UNTIL CE;
		nop;
DelayLp:	nop;
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
	M6=6;
	M7=-5;

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
	DM(I1,M1)=B#0010001000000010; {MCK=3,FSEL=0,MS=1,TXDIS=0,ENL=0,ADL=0}
	DM(I1,M1)=B#0000000000000000; {pio=0}
	DM(I1,M1)=B#0000000000000000; {REVID=NC}

	I1=^DataOut;
	DM(I1,M1)=B#1100000000000000;	{OM0=1,OM1=1,L0=0,SM=0,R0=0}
	DM(I1,M1)=B#1100010111110101;	{PI0=11,OVR=0,IS=0,LG=5,MA=15,RG=5}
	DM(I1,M1)=0x0000;
	DM(I1,M1)=0x0000;

	rts;

.endmod;
