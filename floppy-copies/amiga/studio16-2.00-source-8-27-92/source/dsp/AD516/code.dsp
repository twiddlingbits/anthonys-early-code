{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{/////				Handler				/////}
{/////				for AD0516			/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
.module/ram/abs=0/boot=0		Handler;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		CHANNELS=8;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.port		ModeSel;	{3000W}
.port		REMOVE_ME;	{2002W}
.port		Bank;		{3004W}
.port		SmpteData;	{2004R}
.port		Status;		{2006R}
.port		Read68;		{2008R}
.port		Write68;	{300AW}
.port		Interupt68;	{300cW}
.port		ReadFIFO;	{200eR}
.port		WriteFIFO;	{300eW}
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
{memory     bits  0-4       1=8k  4=32k  F=128k}
{           bits 5-23     unused}
{++++++++++++++++++++++++}
.init Sware: 5161;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		REC_FIFO_EMPTY=0x0040;
.const		PLAY_FIFO_HALF_FULL=0x0020;
.const		REC_FIFO_HALF_FULL=0x0010;
.const		AMIGAINT=0x0004;
.const		WRITEOK=0x0002;
.const		READOK=0x0001;
.const		IntStat=0x0004;
.const		SCLK=1200;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		BLOCKSIZE=0x0400;
.const		BUFFERSIZE=0x1800;
.const		NUMBUFFERS=0x0006;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		MsgRecDataLeft=0x0009;
.const		MsgRecDataRight=0x0007;
.const		MsgPlayData=0x000F;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram/circ/seg=extern_dm buffer[BUFFERSIZE];
.var/dm/ram/circ/seg=extern_dm InPointer;
.var/dm/ram/circ/seg=extern_dm DelayTime;
.var/dm/ram/circ/seg=extern_dm DelayFeedback;
.var/dm/ram/circ/seg=extern_dm RecCntr;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	PlayCntr;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/circ	CtrlIn[4];
.var/circ	CtrlOut[4];
.var/circ	DataIn[4];
.var/circ	DataOut[4];
.var/dm/ram	SUCmode;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	Val_LEFT_Input;
.var/dm/ram	Val_RIGHT_Input;
.var/dm/ram	Val_Chan1;
.var/dm/ram	Val_Chan2;
.var/dm/ram	Val_Chan3;
.var/dm/ram	Val_Chan4;
.var/dm/ram	Val_Chan5;
.var/dm/ram	Val_Chan6;
.var/dm/ram	Val_Chan7;
.var/dm/ram	Val_Chan8;
.var/dm/ram	Val_LEFT_Output;
.var/dm/ram	Val_RIGHT_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	HiLo_LEFT_Input;
.var/dm/ram	HiLo_RIGHT_Input;
.var/dm/ram	HiLo_Chan1;
.var/dm/ram	HiLo_Chan2;
.var/dm/ram	HiLo_Chan3;
.var/dm/ram	HiLo_Chan4;
.var/dm/ram	HiLo_Chan5;
.var/dm/ram	HiLo_Chan6;
.var/dm/ram	HiLo_Chan7;
.var/dm/ram	HiLo_Chan8;
.var/dm/ram	HiLo_LEFT_Output;
.var/dm/ram	HiLo_RIGHT_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/pm/ram	Vol_LEFT_Input;
.var/pm/ram	Vol_LEFT_Chan1;
.var/pm/ram	Vol_LEFT_Chan2;
.var/pm/ram	Vol_LEFT_Chan3;
.var/pm/ram	Vol_LEFT_Chan4;
.var/pm/ram	Vol_LEFT_Chan5;
.var/pm/ram	Vol_LEFT_Chan6;
.var/pm/ram	Vol_LEFT_Chan7;
.var/pm/ram	Vol_LEFT_Chan8;
.var/pm/ram	Vol_LEFT_Output;
.var/pm/ram	Vol_RIGHT_Input;
.var/pm/ram	Vol_RIGHT_Chan1;
.var/pm/ram	Vol_RIGHT_Chan2;
.var/pm/ram	Vol_RIGHT_Chan3;
.var/pm/ram	Vol_RIGHT_Chan4;
.var/pm/ram	Vol_RIGHT_Chan5;
.var/pm/ram	Vol_RIGHT_Chan6;
.var/pm/ram	Vol_RIGHT_Chan7;
.var/pm/ram	Vol_RIGHT_Chan8;
.var/pm/ram	Vol_RIGHT_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	REC_MODE;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	SerTest;
.var/dm/ram	PIMASK;
{*********.var/dm/ram	WCMODE;********}
.var/dm/ram	CDMODE;
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
	ENA SEC_REG;call SmpteInterupt;	DIS SEC_REG;	rti;	{DATA AND SMPTE}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	jump setupcontrol;rti;		rti;		rti;	{Transmit}
ConV:	jump ConversInt;nop;		nop;		nop;	{Recieve}
	rti;		rti;		rti;		rti;	{Timer}
{////////////////////////////////////////////////////////////////////}
{/////			Start Execution				/////}
{////////////////////////////////////////////////////////////////////}
start:
	ar=0;
	dm(CDMODE)=ar;
	dm(REC_MODE)=ar;
	call CntlReg_inits;	{ set up SPORTS, TIMER, etc }
	ar=DM(Read68);
	ar=B#100010;
	DM(PIMASK)=ar;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Main_Loop:
PBSM:	nop;	{Call Hplay - Play/Record Block Completed}
SMSM:	nop;	{Call Hsmpte}

	ay1=DM(Status);ar=READOK;
	ar=ar and ay1;
	if EQ jump ML_Skip;
	call Command;	{READ DATA}
	jump Main_Loop;

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
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
JumpTable:
	rts;			{0x__01}
rts;{	jump SetFreq;}		{0xbF03}
	jump CntrlRegAdj;	{0x__05}
rts;{	jump SetTimer;}		{0x__07}
	jump DataRegAdj;	{0x__09}
rts;{	jump GainDown;}		{0x__0B}
rts;{	jump RecordSend;}	{0x__0D}
	rts;			{0x_C0f}

	rts;			{0x__11}
	rts;			{0x__13}
	jump PB_STATE;		{0x__15}
	jump SM_STATE;		{0x__17}
	jump MODE;		{0x__19}
	jump PAUSE;		{0x__1B} 
	jump ChanVol;		{0x__1D}
	jump HL_SEND;		{0x__1F}

	jump NewPlayPtr;	{0x__21}
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
	jump MonOn;		{0x__45}
	jump MonOff;		{0x__47}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
MonOn:
{	ar=B#0010000100101100;DM(CtrlOut)  =ar;
	ar=B#0010001000000000;DM(CtrlOut+1)=ar;
	ar=0x3fff;call Delay;
	call ModeOn;
	ar=0x3fff;call Delay;
	ar=B#1100000000000000;DM(DataOut)  =ar;
	ar=B#1100010111110101;DM(DataOut+1)=ar;}
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
MonOff:
{	ar=B#0010000100000100;DM(CtrlOut)  =ar;
	ar=B#0001001000000000;DM(CtrlOut+1)=ar;
	ar=0x3fff;call Delay;
	call ModeOn;
	ar=0x3fff;call Delay;
	call ModeOff;}
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ChanVol:
	ay0=^Vol_LEFT_Input;
	ar=ax1+ay0;
	i7=ar;

	Wait(READOK);
	ax0=DM(Read68);		{LEFT}
	PM(i7,m6)=ax0;

	Wait(READOK);
	ay0=DM(Read68);		{RIGHT}
	PM(i7,m6)=ay0;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
CntrlRegAdj:
	Wait(READOK);
	ar=DM(Read68);
	DM(CtrlOut)=ar;

	Wait(READOK);
	ar=DM(Read68);
	DM(CtrlOut+1)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
DataRegAdj:
	Wait(READOK);
	ar=DM(Read68);
	DM(DataOut)=ar;

	Wait(READOK);
	ar=DM(Read68);
	DM(DataOut+1)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
HLchanVAL:
	i6=^Val_LEFT_Input;
	i7=^HiLo_LEFT_Input;

	cntr=12;
	do HLop UNTIL CE;
		ar=DM(i6,m5);
		ar=ABS ar;
		ay0=ar;
		ar=dm(i7,m4);
		ar=ar-ay0;
		if GT jump Nxt;
		dm(i7,m4)=ay0;
Nxt:		Modify(i7,m5);
HLOP:		nop;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
HL_SEND:
{ннннннннннннннннннннннннннннн}
	ax0=0;
	i7=^Vol_LEFT_Input;
{ннннннннннннннннннннннннннннн}
{ннннннннннннннннннннннннннннн}
{ннннннннннннннннннннннннннннн}
	sr0=DM(HiLo_LEFT_Input);
	DM(HiLo_LEFT_Input)=ax0;		{ Zero it out }

	my0=PM(i7,m6);
	MR=sr0*my0 (SU);
	if mv sat mr;

	Wait(WRITEOK);
	ar=abs mr1;
	DM(Write68)=AR;	{ Write Left}
{-----------RIGHT--------------}
	sr0=DM(HiLo_RIGHT_Input);
	DM(HiLo_RIGHT_Input)=ax0;		{ Zero it out }

	my0=PM(i7,m7);
	MR=sr0*my0 (SU);
	if mv sat mr;

	Wait(WRITEOK);
	ar=abs mr1;
	DM(Write68)=AR;	
{ннннннннннннннннннннннннннннн}
{ннннннннннннннннннннннннннннн}
{ннннннннннннннннннннннннннннн}
	i6=^HiLo_Chan1;

	cntr=8;
	do HLoop UNTIL CE;
	sr0=DM(I6,m4);		{ Get HiLow Info }
	DM(I6,m5)=ax0;		{ Zero it out }

	my0=PM(i7,m6);
	MR=sr0*my0 (SU);
	if mv sat mr;

	Wait(WRITEOK);
	ar=abs mr1;
	DM(Write68)=AR;	{ Write Left}
{-----------RIGHT--------------}
	my0=PM(i7,m7);
	MR=sr0*my0 (SU);
	if mv sat mr;

	Wait(WRITEOK);
	ar=abs mr1;
HLoop:	DM(Write68)=AR;	{ Write Right}
{ннннннннннннннннннннннннннннн}
{ннннннннннннннннннннннннннннн}
{ннннннннннннннннннннннннннннн}
	sr0=DM(HiLo_LEFT_Output);
	DM(HiLo_LEFT_Output)=ax0;		{ Zero it out }

	my0=PM(i7,m6);
	MR=sr0*my0 (SU);
	if mv sat mr;

	Wait(WRITEOK);
	ar=abs mr1;
	DM(Write68)=AR;	{ Write Left}
{-----------RIGHT--------------}
	sr0=DM(HiLo_RIGHT_Output);
	DM(HiLo_RIGHT_Output)=ax0;		{ Zero it out }

	my0=PM(i7,m7);
	MR=sr0*my0 (SU);
	if mv sat mr;

	Wait(WRITEOK);
	ar=abs mr1;
	DM(Write68)=AR;	
{ннннннннннннннннннннннннннннн}
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
MODE:	ar=ax1; ar= PASS ar;
	if NE jump ModeOn;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ModeOff:
	ar=dm(CDMODE);
	ar=PASS ar;
	if EQ rts;

	ar=0;
{*****	dm(WCMODE)=ar;*******}
	dm(CDMODE)=ar;
{*************wtlp:
	ar=dm(WCMODE);
	ar=PASS ar;
	if EQ jump wtlp;***********}

	Call ControlMode;

	imask=b#000000;
	Wait(WRITEOK);
	DM(Write68)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ModeOn:
	ar=dm(CDMODE);
	ar=PASS ar;
	if NE rts;

	ar=1;
	dm(CDMODE)=ar;

	imask=b#000000;
	call DataMode;
	Wait(WRITEOK);
	DM(Write68)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
PAUSE:	ar=ax1; ar= PASS ar;
	if NE jump NotPause;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
PAUSEO:
	imask=b#100000;
	DM(PIMASK)=IMASK;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
NotPause:
	IMASK=B#100010;
	DM(PIMASK)=IMASK;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
NewCode:imask=b#000000;

	call ModeOff;
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
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Poke:	Wait(READOK);
	ar=DM(Read68);
	i7=ar;
	Wait(READOK);
	ar=DM(Read68);
	DM(i7,m4)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Peek:	Wait(READOK);
	ar=DM(Read68);
	i7=ar;
	Wait(WRITEOK);
	ar=DM(i7,m4);
	DM(Write68)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
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
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SM_STATE:
	ar=ax1;
	ar=pass ar;

	if NE jump ADD_SMPTE_INTERUPT;
	ar=0;
	DM(REMOVE_ME)=ar;
	rts;

ADD_SMPTE_INTERUPT:
	ar=0x4000;
	DM(REMOVE_ME)=ar;
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
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				SMPTE				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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


.var/dm/ram	SSAVEI7;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SmpteInterupt:
{*********************************}
	DM(SSAVEI7)=I7;
	I7=^SMPTEdata1;
	se=16;
{*********************************}
	ar=DM(SmpteData);			{ GET NEW SMPTE BIT }
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
	I7=DM(SSAVEI7);
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
	ar=DM(REC_MODE);
	ar=PASS ar;
	if NE jump SKKK;
	ax1=0x7FFF;
SKKK:
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
	ax1=0x7FFF;
	Wait(WRITEOK);
	DM(Write68)=ax1;
{--------------------------------}
	RemoveEvent(^SMSM);
	rts;
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
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecLeftOutInt:
	ar=14;	DM(Bank)=ar;
	DM(I3,M0)=ax0;
	jump RecIntCommon;
{ннннннннннннннннннннннннннннн}
RecRightOutInt:
	ar=15;	DM(Bank)=ar;
	DM(I3,M0)=ax1;
	jump RecIntCommon;
{ннннннннннннннннннннннннннннн}
RecLeftInInt:
	ar=14;	DM(Bank)=ar;
	ar=DM(Val_LEFT_Input);
	DM(I3,M0)=ar;
	jump RecIntCommon;
{ннннннннннннннннннннннннннннн}
RecRightInInt:
	ar=15;	DM(Bank)=ar;
	ar=DM(Val_RIGHT_Input);
	DM(I3,M0)=ar;
{	jump RecIntCommon;}
{ннннннннннннннннннннннннннннн}
RecIntCommon:
	ar=PASS sr0;
	if EQ jump SkipRecIncI;
	ay0=DM(RecCntr);ar=ay0+1;DM(RecCntr)=ar;
SkipRecIncI:
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SendRecordInfo:				{ This routine is called in main (Hplay)}
	ar=DM(Status);			{ polling loop. It Does the following}
	ay0=REC_FIFO_EMPTY;		{ It Puts A channel of rec info into}
	ar=ar and ay0;			{ FIFO the sends a 'RecordDataPresent'}
	if NE rts;			{ command, Decrements 'RecCntr' then}
					{ repeats for the next}
					{ record channel, if any}
SendRecordLeft:
	ChangeBank(14);
	ar=DM(RecCntr);
	ar=PASS ar;
	if LE jump SendRecordRight;

	ax1=MsgRecDataLeft;
	Call RecordSend;
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
SendRecordRight:
	ChangeBank(15);
	ar=DM(RecCntr);
	ar=PASS ar;
	if LE rts;

	ax1=MsgRecDataRight;
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
RecordSend:
	I4=DM(InPointer);
	CNTR=BlockSize;
	DO SendBsR UNTIL CE;
		ar=DM(I4,m5);
SendBsR:	DM(WriteFIFO)=ar;
	DM(InPointer)=i4;

	InteruptWrite(ax1);	{ REcordSendData[left or right]

	ax1=imask;
	imask=b#000000;		{ disable play interupt, so RecCntr does not change}
	ay0=DM(RecCntr);
	ar=ay0-1;
	DM(RecCntr)=ar;
	imask=ax1;
	ax1=1;
	DM(REC_MODE)=ax1;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOff:
	ay0=0;
	DM(REC_MODE)=ay0;
	ay0=1;
	ar=ax1 and ay0;
	if NE Call RecordOffLeft;

	ay0=2;
	ar=ax1 and ay0;
	if NE Call RecordOffRight;
	rts;
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
RecordOffRight:
	ar=0;px=0;
	i7=^RecR;		{ I HATE writting self modifying code }
	PM(i7,m4)=ar;

	ar=15;
	call CleanBank;
	rts;
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
RecordOffLeft:
	ar=0;px=0;
	i7=^RecL;		{ I HATE writting self modifying code }
	PM(i7,m4)=ar;

	ar=14;
	call CleanBank;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordInOn:
	si=i3;
TryRecInLeft:
	ay0=1;
	ar=ax1 and ay0;
	if EQ jump TryRecInRight;	{ Record Left On, NOT! }

	i7=^RecordLeftInJump;
	i6=^RecL;		{ I HATE writting self modifying code }
	ChangeBank(14);
	call RecChanOn;

TryRecInRight:
	ay0=2;
	ar=ax1 and ay0;
	if EQ rts;		{ Record Right On, NOT! }

	i7=^RecordRightInJump;
	i6=^RecR;		{ I HATE writting self modifying code }
	ChangeBank(15);
	jump RecChanOn;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOutOn:
	si=i3;
TryRecOutLeft:
	ay0=1;
	ar=ax1 and ay0;
	if EQ jump TryRecOutRight;	{ Record Left On, NOT! }

	i7=^RecordLeftOutJump;
	i6=^RecL;		{ I HATE writting self modifying code }
	ChangeBank(14);
	call RecChanOn;

TryRecOutRight:
	ay0=2;
	ar=ax1 and ay0;
	if EQ rts;		{ Record Right On, NOT! }

	i7=^RecordRightOutJump;
	i6=^RecR;		{ I HATE writting self modifying code }
	ChangeBank(15);
	jump RecChanOn;

{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecChanOn:
	DM(InPointer)=si;

	ar=PM(i7,m4);
	PM(i6,m4)=ar;

	ar=-1;
	DM(RecCntr)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordLeftInJump:	Call RecLeftInInt;
RecordRightInJump:	Call RecRightInInt;
RecordLeftOutJump:	Call RecLeftOutInt;
RecordRightOutJump:	Call RecRightOutInt;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				PLAY				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
PlayFifo:{ax0 = raw data }
	ay0=0x001F;
	ar=ax0 and ay0;
	sr=LSHIFT ar by -1 (lo);
	ChangeBank(sr0);

	ar=ax0;
	sr=LSHIFT ar by -5 (lo);

	i4=DM(InPointer);

	CNTR = sr0;
	DO PlayF3 UNTIL CE;
	ar=DM(ReadFIFO);
PlayF3:	DM(I4,m5)=ar;

	DM(InPointer)=i4;

	rts;
	POP PC;	jump Main_Loop; {jump instead of RTS}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
NewPlayPtr:
	ChangeBank(ax1);

	Wait(READOK);		{ Wait For INDEX }
	ar=DM(Read68);
	I4=ar;
	jump off_a_bridge;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
MovePlayPtr:
	ChangeBank(ax1);

	I4=I3;
	Wait(WRITEOK);
	DM(Write68)=i4;		{ send playcount num }

off_a_bridge:

	Wait(READOK);		{ Wait For Channel Data Cmd }
	ar=DM(Read68);

	sr=LSHIFT ar by -5 (lo);

	CNTR = sr0;
	DO PlayF8 UNTIL CE;
	ar=DM(ReadFIFO);
PlayF8:	DM(I4,m5)=ar;

	DM(InPointer)=i4;

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Hplay:				{NEEDED ???? }

{RI:	nop;}			{ Self Modifying code (which I hate)}
				{, Call to Block Trasfer Record Data}
	Call SendRecordInfo;

	ar=DM(PlayCntr);
	ar=PASS ar;
	if EQ rts;

	ax1=imask;
	imask=b#000000;		{ disable play interupt, so PlayCntr does not change}
	ay0=DM(PlayCntr);
	ar=ay0-1;
	DM(PlayCntr)=ar;
	imask=ax1;

	ax1=MsgPlayData;InteruptWrite(ax1);

	jump Hplay;
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
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	OldBit;
.var/dm/ram	SaveI7;
ConversInt:
{нннннннннннннннннннннннннннннннн}
	ENA SEC_REG;
	DM(DataOut+2)=ax0;{DAC_L} 
	DM(DataOut+3)=ax1;{DAC_R}
	ax0=DM(DataIn+2);{ADC_L}
	ax1=DM(DataIn+3);{ACC_R}
{нннннннннннннннннннннннннннннннн}
	DM(SaveI7)=I7;
{нннннннннннннннннннннннннннннннн}
	ar=I3;
	ay0=BLOCKSIZE;
	ar=ar and ay0;

	sr0=0;			{ FLAG used by record }
	ay1=DM(OldBit);
	af=ar xor ay1;
	if EQ jump Pskip;

	DM(OldBit)=ar;
	ay0=DM(PlayCntr);ar=ay0+1;DM(PlayCntr)=ar;
	sr0=1;			{ FLAG used by record }
{нннннннннннннннннннннннннннннннн}
Pskip:
{нннннннннннннннннннннннннннннннн}
	DM(Val_LEFT_Input)=ax0;
	DM(Val_RIGHT_Input)=ax1;
	i2=^Val_Chan1;
	i7=^Vol_LEFT_Input;
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{||                      LEFT                         ||}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
	ar=ax0;{Input LEFT}
	my0=PM(I7,m5);	{Vol_LEFT_Input}
	MR=ar*my0 (SU);
{нннннннннннннннннннннннннннннннн}
	cntr=CHANNELS;
	DO PlayLeft UNTIL CE;
		DM(Bank)=cntr;
		my0=PM(I7,m5) {GET VOL} , mx0=DM(I3,M0);{GET SAMP}
PlayLeft:	DM(i2,m1)=mx0 {4 HILO}  , MR=MR+mx0*my0 (SU);
{нннннннннннннннннннннннннннннннн}
	if mv sat mr;
	DM(Val_LEFT_Output)=mr1;
	my0=PM(I7,m5);{(Vol_LEFT_Output)}
	MR=mr1*my0 (SU);
	if mv sat mr;
	ax0=MR1;	{DAC_L}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{||                      RIGHT                        ||}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
	ar=ax1;	{Input RIGHT}
	my0=PM(I7,m5);	{(Vol_RIGHT_Input)}{Monitor}
	MR=ar*my0 (SU);
{нннннннннннннннннннннннннннннннн}
	i2=^Val_Chan1;		{ Left buss read the buffer and placed the data here }
	ay0=0;			{ Do Not NonZero this register till modify}
	cntr=CHANNELS;
	DO PlayRight UNTIL CE;
		DM(Bank)=cntr;
		my0=PM(I7,m5) {GET VOL}   , mx0=DM(I2,M1);{GET SAMP}
PlayRight:	DM(i3,m0)=ay0 {ZERO SAMP} , MR=MR+mx0*my0 (SU);
{нннннннннннннннннннннннннннннннн}
	if mv sat mr;
	DM(Val_RIGHT_Output)=mr1;
	my0=PM(I7,m5);{(Vol_RIGHT_Output)}
	MR=mr1*my0 (SU);
	if mv sat mr;
	ax1=MR1;	{DAC_R}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{||                      RECORD                       ||}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecL:	nop;			{SMC}
RecR:	nop;			{SMC}
{нннннннннннннннннннннннннннннннн}
	Modify(i3,m1);
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{||                      OTHER                        ||}
{ннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннн}
	ay1=DM(SAMPLCNTR);
	ar=ay1+1;
	DM(SAMPLCNTR)=ar;
{нннннннннннннннннннннннннннннннн}
{******	ar=DM(Status);
	ay0=0x0080;
	ar=ar and ay0;
	dm(WCMODE)=ar;*****}		{ USED DURRING DATA/CNTRL TRANS }
{******	if EQ call SmpteInterupt;*******}
{нннннннннннннннннннннннннннннннн}
	I7=DM(SaveI7);
	ar=DM(BANKsave);
	DM(Bank)=ar;
	DIS SEC_REG;
{нннннннннннннннннннннннннннннннн}
MODESI:	nop;
{нннннннннннннннннннннннннннннннн}
	rti;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX			Control/Data Mode			 XX  }
{ /  \			Setup					/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ControlMode:
	I7=0x3FFF;
	DM(I7,M4)=0x0418;	{SerialOff}

	ax0=0;
	DM(ModeSel)=ax0;
	ar=0x01FF;Call Delay;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	DMcntr;
DataMode:
	I7=0x3FFF;
	DM(I7,M4)=0x0418;	{SerialOff}

	ar=0x01FF;Call Delay;

	I0=^CtrlIn;
	I1=^CtrlOut;

	ax0=DM(CtrlOut);
	ay1=B#1111101111111111;
	ar=ax0 and ay1;
	DM(CtrlOut)=ar;
	DM(SerTest)=ar;

	ax0=1;
	DM(SUCmode)=ax0;

	I7=0x3fef;
	l7=0;
{3fef}	DM(I7,M5)=B#0000001010000111; {0x0283}	{aport1 autobuffer cntrl reg}
{3ff0}	DM(I7,M5)=383;{383}		{rfsdiv0}
{3ff1}	DM(I7,M5)=SCLK;	{849;}		{sclkdiv0}
{3ff2}	DM(I7,M5)=B#0100000100011111; {sport1 cntr reg: internal sclk&rfs} {normal framing mode,} {frame sync not inverted} {16 bit word len}

	ICNTL=B#00000;
	IMASK=B#100100;		{only sport transmit int enabled}
	ax0=DM(I1,M1);		{send first 16 bits of ctrl word}
	TX1=ax0;

	I7=0x3fff;
{3fff}	DM(I7,M4)=0x0C18;	{system cntrl reg: sport 1 enabled}
{++++++++++++++++}
{								ar=0x7F00; DM(DMcntr)=ar;}

wait1:
{								ay0=DM(DMcntr);
								ar=ay0-1;
								if EQ jump T?r?a?nsitionError;
								DM(DMcntr)=ar;}

	idle;
	ar=DM(SUCmode);
	ar=PASS ar;
	if NE jump wait1;
{++++++++++++++++}
GO_DMODE:
	ar=0x01FF;Call Delay;
	I0=^DataIn+2;
	I1=^DataOut+2;

	ax0=DM(I1,M1);TX1=ax0;
	ar=0xc18;DM(0x3FFF)=ar;

	ENA SEC_REG;
	ax0=0;
	ax1=0;		{ Clear to prevent POPS }
	DIS SEC_REG;

	IFC=B#000000111111;
	IMASK=DM(PIMASK);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SETUPCONTROL:
	ay1=^SUCJumpTable;
	ar=DM(SUCmode);
	ar=ar+ay1;
	I7=ar;
	jump (I7);
{ннннннннннннннннннннннннннннннннн}
SUCJumpTable:
	rti;			{0}
	jump SUCFirst;		{1}
	jump SUCWaitDCB0;	{2}
	jump SUCWaitDCB1;	{3}
{ннннннннннннннннннннннннннннннннн}
SUCFirst:
	ax0=2;
	DM(SUCmode)=ax0;
	rti;
{ннннннннннннннннннннннннннннннннн}
SUCWaitDCB0:
	ax0=DM(CtrlIn);			{DCB flag has not been set yet}
	ay1=DM(SerTest);
	ar=ax0 XOR ay1;			{same as we sent out?}
	if NE rti;			{set flag if DCB was 0}

	ax0=DM(CtrlOut);
	ay1=B#0000010000000000;		{set DCB back to 1 (TRANSFERS DATA!)}
	ar=ax0 or ay1;
	DM(CtrlOut)=ar;

	ax0=3;
	DM(SUCmode)=ax0;
	rti;
{ннннннннннннннннннннннннннннннннн}
SUCWaitDCB1:
	ax0=DM(CtrlIn);			{DCBFlag was set}
	ay1=B#0000010000000000;
	ar=ax0 AND ay1;			{only check for DCB bit}
	if EQ rti;			{if DCB=1 ready for datamode}

	IMASK=B#100000;
	ax0=0x0418;			{disable sport1}
	DM(0x3FFF)=ax0;

	ax0=0x001f;
	DM(0x3ff2)=ax0;			{sport 1 cntrl: internal tfs, external sclk & rfs}
	ax0=1;
	DM(ModeSel)=ax0;		{set D/C high}
	ar=0x01FF;Call Delay;

	ax0=0;
	DM(SUCmode)=ax0;
	rti;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
CntlReg_inits:
{++++++++++++++++}
	I7=0x3FFF;
	DM(I7,M4)=0x0418;	{SerialOff}
	ax0=0;
	DM(ModeSel)=ax0;
	ar=0x01FF;Call Delay;

	ax0=0x120C;		{ 3.6K samp rate w/ 1.8432MHZ clk }
	DM(REMOVE_ME)=ax0;		{ CLKIN=900*Rate @1.8432MHZ}
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
{3ffe}	DM(I7,M5)=0x1049;	{DM WAIT STATES one for each except 3000-3400}

	I3=^buffer;

	M0=0;
	M1=1;
	M2=2;		{ CAN BE CHANGED}
	M3=-1;
	M4=0;
	M5=1;
	M6=10;
	M7=-9;

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
	DM(I1,M1)=B#0010001000000000; {MCK=2,FSEL=0,MS=1,TXDIS=0,ENL=0,ADL=0}
	DM(I1,M1)=B#0000000000000000; {pio=0}
	DM(I1,M1)=B#0000000000000000; {REVID=NC}

	I1=^DataOut;
	DM(I1,M1)=B#1100000000000000;	{OM0=1,OM1=1,L0=10,SM=0,R0=10}
	DM(I1,M1)=B#1100010111110101;	{PI0=11,OVR=0,IS=0,LG=5,MA=15,RG=5}
	DM(I1,M1)=0x0000;
	DM(I1,M1)=0x0000;

	ax1=0;
	ChangeBank(14);
	DM(RecCntr)=ax1;
	ChangeBank(15);
	DM(RecCntr)=ax1;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				MISC				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Delay:
	cntr=ar;
	DO DelayLp UNTIL CE;
		nop;
DelayLp:	nop;
		rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
CleanBank:
{	DM(BANKsave)=ar;
	DM(Bank)=ar;
	ay1=0;
	I7=^buffer;
	cntr=BUFFERSIZE;
		do ClearRecR until CE;
ClearRecR:	DM(i7,m5)=ay1;}
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.endmod;
