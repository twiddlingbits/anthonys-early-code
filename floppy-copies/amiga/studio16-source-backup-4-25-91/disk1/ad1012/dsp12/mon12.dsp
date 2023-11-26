{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{/////				MONITOR	12			/////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
.module/ram/abs=0/boot=0		Monitor;
.const		Srate=100;
.port		ADC;
.port		DAC;
.port		VolFreq;
.port		Read68;
.port		Write68;
.port		Status;
.port		Interupt68;

.const		EVhilo=0x0001;
.const		EVsmpt=0x0002;
.const		EVsmpR=0x0000;

.const		EVmon=0x0010;
.const		EVrec=0x0080;
.const		EVrecSt=0x0040;

.const		EVplay=0x0020;

.const		SampRate=0x3FF1;

.const		WRITEOK=0x0002;
.const		READOK=0x0001;


.const		MsgHi=0x0001;
.const		MsgLow=0x0007;
.const		MsgRecData=0x0009;
.const		MsgPlayData=0x000F;

.const		BLOCKSIZE=0x0200;
.const		BUFFERSIZE=0x0800;
.const		NUMBUFFERS=0x0004;
.var/dm/ram/circ buffer[0x2000];

.var/dm/ram	VFsave;

.var/dm/ram	VAR0;
.var/dm/ram	VAR1;
.var/dm/ram	VAR2;
.var/dm/ram	VAR3;

.var/dm/ram	Lowest;
.var/dm/ram	Highest;

.var/dm/ram	BuffCntr;

.var/dm/ram	FLAG;
.var/dm/ram	Mode;

.var/dm/ram	Output;
.var/dm/ram	Input;

.var/dm/ram	Save1;
.var/dm/ram	Save2;
.var/dm/ram	Save3;
.var/dm/ram	Save4;
.var/dm/ram	Save5;
.var/dm/ram	Save6;
{||||||||||||||||||||||||||||||}
.var/dm/ram	Channel;
.var/dm/ram	EVflag;

.var/dm/ram	OldBit;

.var/dm/ram	Vol;
.var/dm/ram	Vol0;
.var/dm/ram	Vol1;
.var/dm/ram	Vol2;
.var/dm/ram	Vol3;
.var/dm/ram	VolMon;
{||||||||||||||||||||||||||||||}
.var/dm/ram	BuffersNeeded;
.var/dm/ram	ReceiveNeeded;
{||||||||||||||||||||||||||||||}
.const		SMPTEsyncword=0xBFFC;	{0011 1111 1111 1101}
.var/dm/ram	SMPTEmode;	{bak}	{1011 1111 1111 1100}
.var/dm/ram	SMPTEdata;
.var/dm/ram	SMPTEctr;
.var/dm/ram	SMPTEsecframe;
.var/dm/ram	SMPTEhourmin;
.var/dm/ram	OLDhourmin;
{||||||||||||||||||||||||||||||}
{////////////////////////////////////////////////////////////////////}
{/////			Interupt Vectors			/////}
{////////////////////////////////////////////////////////////////////}
	jump start;	rti;		rti;		rti;	{Reset Vector}
	ENA SEC_REG;	call HandleInt;	DIS SEC_REG;	rti;	{irq2 A2D}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	ENA SEC_REG;	call Command; 	DIS SEC_REG;	rti;	{irq1 I68000}
	ENA SEC_REG;	call Smpte;	DIS SEC_REG;	rti;	{irq0 SMPTE}
	ENA SEC_REG;	call HiLow;	DIS SEC_REG;	rti;	{timer}
{////////////////////////////////////////////////////////////////////}
{/////			Start Execution				/////}
{////////////////////////////////////////////////////////////////////}
start:	call CntlReg_inits;	{ set up SPORTS, TIMER, etc }

	ar=1;
	DM(SMPTEmode)=ar;
	DM(SMPTEctr)=ar;

	ar=0x00e2;
	DM(VolFreq)=ar;		{ Set Volume and Cutoff Frequency }
	DM(VFsave)=ar;		{ Set Volume and Cutoff Frequency }

	ar=DM(Read68);

{	Call ModeOff;}

	ar=0;
	DM(BuffCntr)=ar;
	DM(FLAG)=ar;

	ar=0x7FFF;
	DM(Vol0)=ar;
	DM(Vol1)=ar;
	DM(Vol2)=ar;
	DM(Vol3)=ar;
	DM(VolMon)=ar;

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
	IFC=b#000000111111;
	imask=b#100111;
	ENA TIMER;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}

lop:	ar=DM(Mode);
	ar=PASS ar;
	if NE jump MainEvents;
	DIS TIMER;
	IDLE;
	ENA TIMER;
	jump lop;

{////////////////////////////////////////////////////////////////////}

{////////////////////////////////////////////////////////////////////}
MainEventsTable:
	Call Hhilo;
	Call Hsmpte;
	nop;
	nop;

	nop;
	Call Hplay;
	Call HrecordStart;
	Call Hrecord;
MainEvents:
	nop;
	nop;
	nop;
	nop;

	nop;
	nop;
	nop;
	nop;
	jump lop;

{////////////////////////////////////////////////////////////////////}
Hplay:	si=0;

	ar=DM(BuffCntr);
	ar=PASS ar;
	if EQ rts;

	ax1=0;
	imask=b#000111;		{ disable play interupt }
	si=DM(BuffCntr);
	DM(BuffCntr)=ax1;
	imask=b#100111;

	sr=LSHIFT si by 4 (lo);

	ay0=MsgPlayData;
	ar=sr0+ay0;

	jump SendW;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
HRsmp:	ay0=EVsmpR;
	ax0=DM(FLAG);
	ar=ax0 and ay0;
	if EQ rts;

	ar=ax0 xor ay0;
	DM(FLAG)=ar;

	ay0=0x0004;
	call EbitOFF;
	jump DoSmpte;
{////////////////////////////////////////////////////////////////////}
Hsmpte:	ay0=EVsmpt;
	ax0=DM(FLAG);
	ar=ax0 and ay0;
	if eq rts;

	ar=ax0 xor ay0;
	DM(FLAG)=ar;

DoSmpte:
	ay0=DM(SMPTEhourmin);
	ar=DM(SMPTEsecframe);
	call SendW;

	ax0=DM(OLDhourmin);
	ar=ax0-ay0;
	if eq rts;
	DM(OLDhourmin)=ay0;
	ar=ay0;
	call SendW;

	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
Hhilo:	ay0=EVhilo;
	ax0=DM(FLAG);
	ar=ax0 and ay0;
	if EQ rts;

	ar=ax0 xor ay0;
	DM(FLAG)=ar;

	ay1=MsgHi;
	ar=DM(Highest);
	sr=LSHIFT ar by 8 (LO);
	ar=SR0 or ay1;
	call SendW;

	ay1=MsgLow;
	ar=DM(Lowest);
	sr=LSHIFT ar by 8 (LO);
	ar=SR0 or ay1;
	call SendW;

	ar=0x0000;
	DM(Lowest)=ar;
	ar=0x0000;
	DM(Highest)=ar;

	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}


{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////			Get Command from 68000			/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
Command:ay0=0x0001;
	ax0=DM(Read68);
	ar=ax0 and ay0;
	if EQ jump PlayData;
{---------------------------------------------------------------------}
	si=ax0;
	sr=ASHIFT si by 8 (LO);
	ay1=0x00FF;
	ar=sr1 and ay1;			{ ax1 = Data }
	ax1=ar;

	sr=LSHIFT si by -1 (LO);
	ay1=0x007F;
	ar=sr0 and ay1;			{ ar = command}

	ay1=0x0040;		{Check if bit 7 (6<<1) set}
	af=ar and ay1;
	if NE rts;		{YES,Bad Command}

	ay1=^JumpTable;
	ar=ar+ay1;
	I7=ar;
	jump (I7);

JumpTable:
	jump SetVol;		{0xVV01}
	jump SetFreq;		{0xbF03}
	jump SetSamplingRate;	{0x__05  Rate=VAR0}
	jump SetTimer;		{0x__07}
	rts;			{0x__09  Period=VAR0  Scale=VAR1}
	rts;			{0x__0B}
	rts;			{0x__0D}
	jump SetCVol;		{0x_C0f  value = var0} 

	jump ModeOn;		{0x__11}
	jump ModeOff;		{0x__13}
	jump EbitOFF;		{0xEV15}
	jump EbitON;		{0xEV17}
	rts;			{0x__19}
	rts;			{0x__1B} 
	rts;			{0x__1D}
	jump DspReset;		{0x__1F}

	jump VAR0lo;		{0xdd21}
	jump VAR0hi;		{0xdd23}
	jump VAR1lo;		{0xdd25}
	jump VAR1hi;		{0xdd27}
	jump VAR2lo;		{0xdd29}
	jump VAR2hi;		{0xdd2B}
	jump VAR3lo;		{0xdd2D}
	jump VAR3hi;		{0xdd2F}

	jump MovePlayPtr;	{0x__31}
	jump RecordOn;		{0x__33}
	rts;			{0x__35}
	rts;			{0x__37}
	rts;			{0x__39}
	jump SMPTEreq;		{0x__3B}
	rts;			{0x__3D}
	rts;			{0x__3F}
{---------------------------------------------------------------------}
SetCVol:
	ay1=^JumpTableVm;
	si=ax1;
	ax1=DM(VAR0);

	sr=LSHIFT si by 1 (lo);
	ar=sr0+ay1;
	I7=ar;
	jump (I7);

JumpTableVm:
	DM(Vol0)=ax1;
	rts;
	DM(Vol1)=ax1;
	rts;
	DM(Vol2)=ax1;
	rts;
	DM(Vol3)=ax1;
	rts;
	DM(VolMon)=ax1;
	rts;
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
SMPTEreq:
	ay0=0xFFFF;
	DM(OLDhourmin)=ay0;	{ make shure hours/mins updated}

	ay0=0xFFFd;
	ax0=DM(FLAG);
	ar=ax0 and ay0;		{clear smpte flags}
	DM(FLAG)=ar;
	rts;
{---------------------------------------------------------------------}
SetVol:			{ data = f0 to 00 }
	ax0=DM(VFsave);
	ay1=0xFF0F;
	af=ax0 and ay1;

	ay1=0x00F0;
	ar=ax1 and ay1;
	ar=ar+af;

	DM(VFsave)=ar;
	DM(VolFreq)=ar;
	rts;
{---------------------------------------------------------------------}
SetFreq:
	ay1=0x0010;
	ar=ax1 and ay1;
	if eq	RESET FLAG_OUT;
	if ne	SET FLAG_OUT;

	ax0=DM(VFsave);
	ay1=0xFFF0;
	ar=ax0 and ay1;
	ay0=ar;

	ay1=0x000F;
	ar=ax1 and ay1;
	ar=ar+ay0;

	DM(VFsave)=ar;
	DM(VolFreq)=ar;
	rts;
{---------------------------------------------------------------------}
SetSamplingRate:

	ar=DM(VAR0);
	ay0=30;
	ar=ar-ay0;		{Boundry test}
	if LT rts;

	ar=DM(VAR0);
	DM(SampRate)=ar;

	rts;
{---------------------------------------------------------------------}
SetTimer:
	ar=DM(VAR0);
	DM(0x3ffd)=ar;
	ar=DM(VAR1);
	DM(0x3ffb)=ar;
	rts;
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
ModeOff:ar=0x00FF;
	Call EventOFF;
	ar=0;
	DM(Mode)=ar;
	rts;
{---------------------------------------------------------------------}
ModeOn:	ar=1;
	DM(Mode)=ar;
	rts;
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
VAR0lo:	I7=^VAR0;
	jump SetLo;
VAR0hi:	I7=^VAR0;
	jump SetHi;
VAR1lo:	I7=^VAR1;
	jump SetLo;
VAR1hi:	I7=^VAR1;
	jump SetHi;
VAR2lo:	I7=^VAR2;
	jump SetLo;
VAR2hi:	I7=^VAR2;
	jump SetHi;
VAR3lo:	I7=^VAR3;
	jump SetLo;
VAR3hi:	I7=^VAR3;
	jump SetHi;
{---------------------------------------------------------------------}
SetLo:	m7=0;
	ar=DM(I7,m7);
	ay1=0xFF00;
	af=ar and ay1;

	ar=ax1 or af;
	DM(I7,m7)=ar;
	rts;
{---------------------------------------------------------------------}
SetHi:	m7=0;
	ar=DM(I7,m7);

	si=ax1;
	sr= LSHIFT si by 8 (LO);

	ay1=0x00FF;
	af=ar and ay1;

	ar=sr0 or af;
	DM(I7,m7)=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////				HiLow				/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
HiLow:
	ar=DM(FLAG);
	ay0=EVhilo;
	ar=ar or ay0;
	DM(FLAG)=ar;

	rts;
{////////////////////////////////////////////////////////////////////}
HiLoSave:				{ ar = value}
{ Save Highest/lowest byte }
	DM(Save1)=ay0;
	DM(Save2)=ax0;
	DM(Save3)=sr0;
	DM(Save4)=sr1;

	Call HL;

	ay0=DM(Save1);
	ax0=DM(Save2);
	sr0=DM(Save3);
	sr1=DM(Save4);
	rts;

HL:	sr=ASHIFT mr1 by -8 (HI);	{ b SSSSSSSSssssssss}
	ay0=sr1;

	ax0=0x8000;
	ar=ax0 and ay0;
	if NE jump Negitive;

	ax0=DM(Highest);
	ar=ax0-ay0;
	if GT rts;
	DM(Highest)=ay0;
	rts;

Negitive:
	ax0=DM(Lowest);
	ar=ax0-ay0;
	if LT rts;
	DM(Lowest)=ay0;

	rts;

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////				SMPTE				/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
Smpte:	ar=DM(SMPTEdata);
	sr=LSHIFT ar by -1 (lo);	{0}
	IF NOT FLAG_IN JUMP SMP0;
	ay0=0x8000;
	ar=sr0+ay0;
	sr0=ar;
SMP0:	DM(SMPTEdata)=sr0;
	ax0=sr0;

	ar=DM(SMPTEmode);
	ar=PASS ar;
	if eq jump Sdata;

Ssearch:ay0=SMPTEsyncword;
	ar=ax0-ay0;
	if NE rts;
	si=0;
	DM(SMPTEmode)=si;
	DM(SMPTEctr)=si;
	rts;

Sdata:	ay0=DM(SMPTEctr);
	ar=ay0+1;
	DM(SMPTEctr)=ar;

	ay0=0x000F;
	ar=ar and ay0;		{ Quicken routine }
	if ne rts;

	si=DM(SMPTEctr);
	sr=LSHIFT si by -4 (lo);
	ay1=^SMPl;
	ar=sr0+ay1;
	I5=ar;
	jump (I5);

SMPl:	rts;
	jump Frames;
	jump Secs;
	jump Mins;
	jump Hours;

Frames:
	ay0=0x0300;
	ar=ax0 and ay0;
	sr=LSHIFT ar by -4 (lo);
	ay0=0x000F;
	af=ax0 and ay0;
	ar=sr0 + af;
	DM(SMPTEsecframe)=ar;
	rts;
Secs:
	ay0=0x0700;
	ar=ax0 and ay0;
	sr=LSHIFT ar by -4 (lo);
	ay0=0x000F;
	af=ax0 and ay0;
	ar=sr0 + af;
	sr=LSHIFT ar by 6 (lo);
	ay0=DM(SMPTEsecframe);
	ar=sr0+ay0;
	sr=LSHIFT ar by 3 (lo);
	ay0=0x0003;
	ar=sr0 + ay0;
	DM(SMPTEsecframe)=ar;
	rts;

Mins:
	ay0=0x0700;
	ar=ax0 and ay0;
	sr=LSHIFT ar by -4 (lo);
	ay0=0x000F;
	af=ax0 and ay0;
	ar=sr0 + af;
	DM(SMPTEhourmin)=ar;
	rts;
Hours:
	ay0=0x0300;
	ar=ax0 and ay0;
	sr=LSHIFT ar by -4 (lo);
	ay0=0x000F;
	af=ax0 and ay0;
	ar=sr0 + af;
	sr=LSHIFT ar by 7 (lo);
	ay0=DM(SMPTEhourmin);
	ar=sr0+ay0;
	sr=LSHIFT ar by 3 (lo);
	ay0=0x0005;
	ar=sr0 + ay0;
	DM(SMPTEhourmin)=ar;
EndFrame:
	si=1;
	DM(SMPTEmode)=si;
	DM(SMPTEctr)=si;

	ar=DM(FLAG);
	ay0=EVsmpt;
	ar=ar or ay0;
	DM(FLAG)=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////				Misc				/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
CntlReg_inits:
	    L0=0;
	    M0=1;
	    I0=0x3FEF;		{point to last DM-mapped control registers }
 { 0x3FEF } DM(I0,M0)=0x0000;	{SPORT1 AUTOBUFF DISABLED}
 { 0x3FF0 } DM(I0,M0)=0x0000;	{SPORT1 RFSDIV NOT USED}
 { 0x3FF1 } DM(I0,M0)= Srate;	{SPORT1 SCLKDIV   (SAMPLING RATE)}
 { 0x3FF2 } DM(I0,M0)=0x4000;	{SPORT1 Control Register}
 { 0x3FF3 } DM(I0,M0)=0x0000;	{NA}
 { 0x3FF4 } DM(I0,M0)=0x0000;	{NA}
 { 0x3FF5 } DM(I0,M0)=0x0000;	{NA}
 { 0x3FF6 } DM(I0,M0)=0x0000;	{NA}
 { 0x3FF7 } DM(I0,M0)=0x0000;	{NA}
 { 0x3FF8 } DM(I0,M0)=0x0000;	{NA}
 { 0x3FF9 } DM(I0,M0)=0x0000;	{NA}
 { 0x3FFA } DM(I0,M0)=0x0000;	{NA}
 { 0x3FFB } DM(I0,M0)=0x0040;	{TIMER TSCALE Scaling Register}
 { 0x3FFC } DM(I0,M0)=0x0001;	{TIMER TCOUNT Counting Register}
 { 0x3FFD } DM(I0,M0)=0x0C00;	{TIMER TPERIOD Period Register}
 { 0x3FFE } DM(I0,M0)=0x1249;	{DM WAIT STATES one for each}
 { 0x3FFF } DM(I0,M0)=0x1018;	{SPORT0 ENABLED, BOOT PAGE 0}

	l0=BUFFERSIZE;
	l1=BUFFERSIZE;
	l2=BUFFERSIZE;
	l3=BUFFERSIZE;
	ar=^buffer;
	I0=ar;
	ay0=BUFFERSIZE;
	ar=ar+ay0;I1=ar;
	ar=ar+ay0;I2=ar;
	ar=ar+ay0;I3=ar;

	m1=1;
	m0=0;

	l7=0;

	m4=0;
	m5=1;
	m6=BUFFERSIZE;
	m7=8;

	L4=BUFFERSIZE;
	I4=^buffer;

	L5=000;

	icntl=b#00111;		{ disable IRQ nesting, all IRQs edge-sensitive }

	rts;

{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
WaitWrite:
	ay0=WRITEOK;
WLop:	ax0=DM(Status);
	ar=ax0 and ay0;
	if EQ jump WLop;

	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
SendW:
	DM(Save5)=ay0;
	DM(Save6)=ar;

	ay0=WRITEOK;
WLoo1p:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump WLoo1p;

	ar=DM(Save6);

	DM(Write68)=ar;			{ xxxx xxxx  xxxx xxxx }

	DM(Interupt68)=ar;

	ay0=WRITEOK;
WLoop:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump WLoop;

	ay0=DM(Save5);
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
DspReset:
	ax0=0x1218;
	dm(0x3FFF)=ax0;
lp:	jump lp;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////				Monitor				/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
MonInt:	ar=DM(Input);
	my0=DM(VolMon);
	MR=ar*my0 (SU);
	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////				Record				/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
RecInt:	ar=DM(Input);
	DM(I5,M6)=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
HrecordStart:
	ay0=si;
	ar=pass ay0;
	if EQ rts;

	ar=ay0-1;
	si=ar;

	ar=EVrecSt;
	jump EventOff;
{////////////////////////////////////////////////////////////////////}
Hrecord:ay0=si;
	ar=pass ay0;
	if EQ rts;

	my0=BlockSize;
	MR= ar * my0 (uu);
	CNTR=mr0;

	sr=LSHIFT si by 4 (lo);
	ay0=MsgRecData;
	ar=sr0+ay0;
	Call SendW;

	ay1=0;
	ay0=WRITEOK;

	DO SendBs UNTIL CE;

	ar=DM(I3,m0);
	DM(Write68)=ar;

RWLop:	ax0=DM(Status);
	ar=ax0 and ay0;
	if EQ jump RWLop;

SendBs:DM(I3,m1)=ay1;

	rts;
{////////////////////////////////////////////////////////////////////}
RecordOn:
	ar=I4;				{ ADD PRE DELAY FOR SAMPLE }
	ay0=BUFFERSIZE*3;
	ar=ar+ay0;
	I3=ar;

	ar=EVrec;
	Call EventOn;

	ar=EVrecSt;
	jump EventOn;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////				Play				/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
PlayInt:
	ar=I4;
	ay0=BLOCKSIZE;
	ar=ar and ay0;

	ay1=DM(OldBit);
	af=ar xor ay1;
	if EQ jump Pskip;

	DM(OldBit)=ar;
	ay0=DM(BuffCntr);ar=ay0+1;DM(BuffCntr)=ar;

Pskip:	i7=^Vol0;
	ay0=0;

	I5=I4;
	CNTR=3;	{Channels}

	DO PlyLp UNTIL CE;
	ar=DM(I5,M4);
	my0=DM(i7,m5);
PlyLP:	DM(I5,M6)=ay0		,	MR=MR+ar*my0 (SU);

	Modify(I4,M5);

	rts;
{////////////////////////////////////////////////////////////////////}
MovePlayPtr:
	ar=ax1;
	sr=LSHIFT ar by 2 (lo);
	ay0=^MovePlayPtrJumpTable;
	ar=sr0+ay0;
	I7=ar;
	ar=I4;				{ ADD PRE DELAY FOR SAMPLE }
	jump (I7);

MovePlayPtrJumpTable:
	nop;
	nop;
	I0=ar;
	rts;

	ay0=BUFFERSIZE;
	ar=ar+ay0;
	I1=ar;
	rts;

	ay0=BUFFERSIZE*2;
	ar=ar+ay0;
	I2=ar;
	rts;

	ay0=BUFFERSIZE*3;
	ar=ar+ay0;
	I3=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
PlayData:{ax0 = raw data }
	ay0=0xFFF0;		{ DATA }
	ar=ax0 and ay0;
	ax1=ar;

	ay0=0x0006;		{ 4 Chanels 0-3 }
	ar=ax0 and ay0;

	ay0=^DataInJumpTable;
	ar=ar+ay0;
	I7=ar;
	jump (I7);

DataInJumpTable:
	DM(I0,m1)=ax1;
	rts;
	DM(I1,m1)=ax1;
	rts;
	DM(I2,m1)=ax1;
	rts;
	DM(I3,m1)=ax1;
	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
IntEventsTable:
	nop; {Hhilo}
	nop; {Hsmpte}
	nop;
	nop;

	Call MonInt;
	Call PlayInt;
	nop;
	Call RecInt;

IntEvents:
	nop;
	nop;
	nop;
	nop;

IntEn:	Call MonInt;		{ << Powerup Monitoring}
	nop;
	nop;
	nop;

	rts;

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
HandleInt:
	mr=0;			{ Clear Mult (output) }

	ar=DM(ADC);
	sr=LSHIFT ar by 4 (lo);	{ Get Byte from Sampler }
	DM(Input)=sr0;

	call IntEn;
	if mv sat mr;

	call HiLoSave;

	sr=ASHIFT mr1 by -4 (hi);
	DM(DAC)=sr1;
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
EbitON:	ar=ax1;
EventON:
	sr1=ar;
	af=PASS 0;
	CNTR = 8;

	DO LoadIn UNTIL CE;
	sr=LSHIFT sr1 by -1 (hi);
	ar=PASS sr0;
	if NE call MainOn;
LoadIn:	af=af+1;
	rts;

MainOn:	{ af=offset }	{ MODIFIED : AR I4}
	m7=8;

	ar=^MainEventsTable;
	ar=ar+af;
	i7=ar;
	ar=PM(i7,m7);
	PM(i7,m7)=ar;

	ar=^IntEventsTable;
	ar=ar+af;
	i7=ar;
	ar=PM(i7,m7);
	PM(i7,m7)=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
EbitOFF:	ar=ax1;

EventOFF:
	sr1=ar;

	af=PASS 0;
	CNTR = 8;
	DO LoadOut UNTIL CE;
	sr=LSHIFT sr1 by -1 (hi);
	ar=PASS sr0;
	if ne call MainOff;
LoadOut:af=af+1;
	rts;

MainOff:{ af=offset }	{ MODIFIED : AR I7 PX}
	m7=8;

	ar=^MainEvents;
	ar=ar+af;
	i7=ar;
	ar=0;
	px=0;
	PM(i7,m7)=ar;

	ar=^IntEvents;
	ar=ar+af;
	i7=ar;
	ar=0;
	px=0;
	PM(i7,m7)=ar;
	rts;
{////////////////////////////////////////////////////////////////////}

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
.endmod;

