{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{/////				MONITOR				/////}
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
.const		SampRate=0x3FF1;

.const		WRITEOK=0x0002;
.const		READOK=0x0001;

.const		BLOCKSIZE=0x0100;
.const		BUFFERSIZE=0x1000;
.const		NUMBUFFERS=0x0010;

.const		IntOffMode=0x0000;
.const		IntOnMode=0x0001;
.const		RecMode=0x0002;
.const		NullMode=0x0003;

.const		MsgHi=0x0001;
.const		MsgLow=0x0007;
.const		MsgRecData=0x0009;
.const		MsgPlayData=0x000F;

.var/dm/ram	I0Save;
.var/dm/ram	I1Save;
.var/dm/ram	I2Save;
.var/dm/ram	I3Save;

.var/dm/ram/circ buffer[0x2000];
.var/dm/ram	VAR0;
.var/dm/ram	VAR1;
.var/dm/ram	VAR2;
.var/dm/ram	VAR3;
.var/dm/ram	Lowest;
.var/dm/ram	Highest;
.var/dm/ram	FLAG;
.var/dm/ram	MODEsave;
.var/dm/ram	VFsave;

.var/dm/ram	OldBit;
.var/dm/ram	Events;

.var/dm/ram	IntEvents;
.var/dm/ram	Output;
.var/dm/ram	Input;

.var/dm/ram	Save1;
.var/dm/ram	Save2;
.var/dm/ram	Save3;
.var/dm/ram	Save4;
.var/dm/ram	Save5;
.var/dm/ram	Save6;


.var/dm/ram	Vol0;
.var/dm/ram	Vol1;
.var/dm/ram	Vol2;
.var/dm/ram	Vol3;
.var/dm/ram	VolMon;


.var/dm/ram	BuffersNeeded;
.var/dm/ram	ReceiveNeeded;

.const		SMPTEsyncword=0xBFFC;	{0011 1111 1111 1101}
.var/dm/ram	SMPTEmode;	{bak}	{1011 1111 1111 1100}
.var/dm/ram	SMPTEdata;
.var/dm/ram	SMPTEctr;
.var/dm/ram	SMPTEsecframe;
.var/dm/ram	SMPTEhourmin;
.var/dm/ram	OLDhourmin;
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

	si=1;
	DM(SMPTEmode)=si;

	si=0x00e2;
	DM(VolFreq)=si;		{ Set Volume and Cutoff Frequency }
	DM(VFsave)=si;		{ Set Volume and Cutoff Frequency }

	Call ModeOff;
	si=DM(Read68);

	si=0;
	DM(Events)=si;
	DM(IntEvents)=si;

	si=0;
	DM(FLAG)=si;
	ar=0x7000;
	DM(VAR1)=ar;
	DM(VAR2)=ar;
	DM(VAR3)=ar;

	si=0xFFFF;
	DM(Vol0)=si;
	DM(Vol1)=si;
	DM(Vol2)=si;
	DM(Vol3)=si;
	DM(VolMon)=si;

	ENA TIMER;
	IFC=b#000000111111;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
	imask=b#100111;

lop:	ax1=M7{MODE};
	ay1=0x0003;
	ar=ax1 and ay1;

	ay1=^JumpMain;
	ar=ar+ay1;
	I5=ar;
	Call (I5);
	jump	lop;

JumpMain:
	rts;
	jump	HandleEvents;
	jump	Hrecord;
	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
HandleEvents:
	Call Hplay;
	ax0=DM(Events);
	ay0=0x0002;
	ar=ax0 and ay0;
	if ne Call Hsmpte;
	ax0=DM(Events);
	ay0=0x0004;
	ar=ax0 and ay0;
	if ne Call HRsmp;
	ax0=DM(Events);
	ay0=0x0001;
	ar=ax0 and ay0;
	if ne Call Hhilo;
	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
Hplay:	ay0=0x0010;
	ax0=DM(FLAG);
	ar=ax0 and ay0;
	if EQ rts;

	ar=ax0 xor ay0;
	DM(FLAG)=ar;

	ar=MsgPlayData;
	call SendW;
	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
HRsmp:	ay0=0x0004;
	ax0=DM(FLAG);
	ar=ax0 and ay0;
	if EQ rts;

	ar=ax0 xor ay0;
	DM(FLAG)=ar;

	ay0=0x0004;
	call bitOFF;
	jump DoSmpte;
{////////////////////////////////////////////////////////////////////}
Hsmpte:	ay0=0x0002;
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
Hhilo:	ay0=0x0001;
	ax0=DM(FLAG);
	ar=ax0 and ay0;
	if EQ rts;

	ar=ax0 xor ay0;
	DM(FLAG)=ar;

	ay1=MsgHi;
	si=DM(Highest);
	sr=LSHIFT si by 8 (LO);
	ar=SR0 or ay1;
	call SendW;

	ay1=MsgLow;
	si=DM(Lowest);
	sr=LSHIFT si by 8 (LO);
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
	si=ax0;
	ar=ax0 and ay0;
	if EQ jump PlayData;
{---------------------------------------------------------------------}
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
	I4=ar;
	jump (I4);

JumpTable:
	jump SetVol;		{0xVV01}
	jump SetFreq;		{0xbF03}
	jump SetSamplingRate;	{0x__05  Rate=VAR0}
	jump ModeOn;		{0x__07}
	jump SetTimer;		{0x__09  Period=VAR0  Scale=VAR1}
	jump RecordOn;		{0x__0B}
	jump RecordOff;		{0x__0D}
	jump PlaySample;	{0x__0F}
	jump EndSample;		{0x__11}
	jump StopPlay;		{0x__13}
	jump MonitorOn;		{0x__15}
	jump DelayOn;		{0x__17}
	jump DelayBuff;		{0x__19  BufferLen=VAR0}
	jump MonitorOff;	{0x__1B} 
	jump DspReset;		{0x__1D}
	jump ModeOff;		{0x__1F}

	jump VAR0lo;		{0xdd21}
	jump VAR0hi;		{0xdd23}
	jump VAR1lo;		{0xdd25}
	jump VAR1hi;		{0xdd27}
	jump VAR2lo;		{0xdd29}
	jump VAR2hi;		{0xdd2B}
	jump VAR3lo;		{0xdd2D}
	jump VAR3hi;		{0xdd2F}
	jump DelayOff;		{0x__31}
	rts;			{0x__33}
	rts;			{0x__35}
	jump HILOoff;		{0x__37}
	jump HILOon;		{0x__39}
	jump SMPTEreq;		{0x__3B}
	jump SMPTEoff;		{0x__3D}
	jump SMPTEon;		{0x__3F}
	jump SetVolMon;		{0x__41}
	jump SetVol0;		{0x__43}
	jump SetVol1;		{0x__45}
	jump SetVol2;		{0x__47}
	jump SetVol3;		{0x__49}
{---------------------------------------------------------------------}
SetVolMon:
	ar=DM(VAR0);
	DM(VolMon)=ar;
	rts;
{---------------------------------------------------------------------}
SetVol0:
	ar=DM(VAR0);
	DM(Vol0)=ar;
	rts;
{---------------------------------------------------------------------}
SetVol1:
	ar=DM(VAR0);
	DM(Vol1)=ar;
	rts;
{---------------------------------------------------------------------}
SetVol2:
	ar=DM(VAR0);
	DM(Vol2)=ar;
	rts;
{---------------------------------------------------------------------}
SetVol3:
	ar=DM(VAR0);
	DM(Vol3)=ar;
	rts;
{---------------------------------------------------------------------}
HILOoff:ay0=0x0001;
	jump bitOFF;
{---------------------------------------------------------------------}
HILOon:	ay0=0x0001;
	jump bitON;
{---------------------------------------------------------------------}
SMPTEoff:ay0=0x0002;
	jump bitOFF;
{---------------------------------------------------------------------}
SMPTEon:ay0=0x0002;
	jump bitON;
{---------------------------------------------------------------------}
SMPTEreq:
	ay0=0xFFFF;
	DM(OLDhourmin)=ay0;	{ make shure hours/mins updated}

	ay0=0xFFF9;
	ax0=DM(FLAG);
	ar=ax0 and ay0;		{clear smpte flags}
	DM(FLAG)=ar;

	ay0=0x0004;
	jump bitON;
{---------------------------------------------------------------------}
bitON:	ax0=DM(Events);
	ar=ax0 or ay0;
	DM(Events)=ar;
	rts;

bitOFF:	ax0=0xFFFF;
	af=ax0 xor ay0;
	ax0=DM(Events);
	ar=ax0 and af;
	DM(Events)=ar;
	rts;
{-------------------------- 0x8??? CMDS ------------------------------}
{-------------------------- 0x8??? CMDS ------------------------------}
{-------------------------- 0x8??? CMDS ------------------------------}
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
ModeOff:
	ar=0;
	DM(IntEvents)=ar;
	M7{MODE}=NullMode;
	rts;
{---------------------------------------------------------------------}
ModeOn:
	M7{MODE}=IntOnMode;
	rts;
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
VAR0lo:	I3=^VAR0;
	jump SetLo;
VAR0hi:	I3=^VAR0;
	jump SetHi;
VAR1lo:	I3=^VAR1;
	jump SetLo;
VAR1hi:	I3=^VAR1;
	jump SetHi;
VAR2lo:	I3=^VAR2;
	jump SetLo;
VAR2hi:	I3=^VAR2;
	jump SetHi;
VAR3lo:	I3=^VAR3;
	jump SetLo;
VAR3hi:	I3=^VAR3;
	jump SetHi;
{---------------------------------------------------------------------}
SetLo:	m3=0;
	L3=0;
	ar=DM(I3,m3);
	ay1=0xFF00;
	af=ar and ay1;

	ar=ax1 or af;
	DM(I3,m3)=ar;
	rts;
{---------------------------------------------------------------------}
SetHi:	m3=0;
	ar=DM(I3,m3);

	si=ax1;
	sr= LSHIFT si by 8 (LO);

	ay1=0x00FF;
	af=ar and ay1;

	ar=sr0 or af;
	DM(I3,m3)=ar;
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
 { 0x3FFD } DM(I0,M0)=0x0600;	{TIMER TPERIOD Period Register}
 { 0x3FFE } DM(I0,M0)=0x1249;	{DM WAIT STATES two for each}
 { 0x3FFF } DM(I0,M0)=0x1018;	{SPORT0 ENABLED, BOOT PAGE 0}

	m0=1;
	L0=0x1000;
	I0=^buffer;
	Call SwapI0;
	I0=^buffer;

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
WLoop:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump WLoop;

	ar=DM(Save6);
	DM(Write68)=ar;			{ xxxx xxxx  xxxx xxxx }

	DM(Interupt68)=ar;

	ay0=DM(Save5);
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
InitBuffer:
	I0=^buffer;
	call SwapI0;
	I0=^buffer;
	
	m0=1;
	L0=0x1000;
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
DspReset:
	ax0=0x1218;
	dm(0x3FFF)=ax0;
lp:	jump lp;

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////			NULL Interupt				/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
NullInt:	rts;
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
	DM(Output)=mr1;
	rts;
{////////////////////////////////////////////////////////////////////}
MonitorOn:
	ar=0x0001;
	jump IntBitOn;
{////////////////////////////////////////////////////////////////////}
MonitorOff:
	ar=0x0001;
	jump IntBitOff;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////				Play				/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
PlayInt:ax0=i0;
	ay0=BLOCKSIZE;
	ar=ax0 and ay0;
	ax1=ar;

	ay1=DM(OldBit);
	ar=ax1 xor ay1;		{Wait for bit to toggle}
	if EQ jump Pskip;

	DM(OldBit)=ax1;
	ar=DM(FLAG);
	ay0=0x0010;		{Bit Toggled.. Time for another block}
	ar=ar or ay0;
	DM(FLAG)=ar;

Pskip:	ay0=0xFFFF;
	ax0=DM(I0,m0);		{ ar = b XXXXssssssssssss }
	ar=ax0-ay0;
	if EQ jump StopPlay;

	ar=ax0;
	my0=DM(Vol0);
	MR=ar*my0 (SU);
	ax0=mr1;

	ay0=DM(Output);
	ENA AR_SAT;
	ar=ax0+ay0;
	DIS AR_SAT;
	DM(Output)=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
PlaySample:
	I0=^buffer;
	m0=1;
	L0=0x1000;

	DM(I0,m0)=0x0000;
	DM(I0,m0)=0x0000;

	ax0=i0;
	ay0=BLOCKSIZE;
	ar=ax0 and ay0;
	DM(OldBit)=ar;

	ar=0x0002;
	jump IntBitOn;
{////////////////////////////////////////////////////////////////////}
EndSample:
	ax0=0xFFFF;
	Call SwapI0;
	DM(I0,m0)=ax0;
	Call SwapI0;
	rts;
{////////////////////////////////////////////////////////////////////}
StopPlay:
	I1=^buffer;
	m1=1;
	L1=0x1000;

	ar=0x0002;
	jump IntBitOff;
{////////////////////////////////////////////////////////////////////}
PlayData:
	DM(I1,m1)=si;
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
	ay0=0x0001;
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

HL:	si=DM(Output);
	sr=ASHIFT si by -8 (HI);	{ b SSSSSSSSssssssss}
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
Smpte:	si=DM(SMPTEdata);
	sr=LSHIFT si by -1 (lo);
	ay0=0x7fff;
	ar=sr0 and ay0;
	IF NOT FLAG_IN JUMP SMP0;
	ay0=0x8000;
	ar=ar+ay0;
SMP0:	DM(SMPTEdata)=ar;
	ax0=ar;

	ay0=DM(SMPTEmode);
	af=PASS ay0;
	if eq jump Sdata;
	jump Ssearch;

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
	ay0=ar;

	ax1=0x000F;
	ar=ax1 and ay0;		{ Quicken routine }
	if ne rts;

{	ax1=0x0070;
	ar=ax1 and ay0;
	ay1=^SMPl;
	ar=ar+ay1;
	I5=ar;
	jump (I5);

SMPl:	rts;
	jump Frames;
	jump Secs;
	jump Mins;
	jump Hours;}

Frames:
	ax1=16;
	ar=ax1-ay0;
	if ne jump secs; 
	ay0=0x0300;
	ar=ax0 and ay0;
	sr=LSHIFT ar by -4 (lo);
	ay0=0x000F;
	af=ax0 and ay0;
	ar=sr0 + af;
	DM(SMPTEsecframe)=ar;
Secs:
	ax1=32;
	ar=ax1-ay0;
	if ne jump Mins; 
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

Mins:	ax1=48;
	ar=ax1-ay0;
	if ne jump hours; 
	ay0=0x0700;
	ar=ax0 and ay0;
	sr=LSHIFT ar by -4 (lo);
	ay0=0x000F;
	af=ax0 and ay0;
	ar=sr0 + af;
	DM(SMPTEhourmin)=ar;
Hours:
	ax1=64;
	ar=ax1-ay0;
	if ne rts; 
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
	ay0=0x0002;
	ar=ar or ay0;
	DM(FLAG)=ar;

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
	ay0=0x0fff;

	ar=ar and ay0;
	DM(I0,m0)=ar;			{ ar = b 0000ssssssssssss }

	ax0=i0;
	ay0=BLOCKSIZE;
	ar=ax0 and ay0;
	ax1=ar;

	ay1=DM(OldBit);
	ar=ax1 xor ay1;			{Wait for bit to toggle}
	if EQ rts;

	DM(OldBit)=ax1;
	ay0=DM(BuffersNeeded);
	ar=ay0+1;
	DM(BuffersNeeded)=ar;

	rts;
{////////////////////////////////////////////////////////////////////}
Hrecord:ay0=DM(BuffersNeeded);
	ar=pass ay0;
	if EQ rts;

	ar=ay0-1;
	DM(BuffersNeeded)=ar;

	ar=MsgRecData;
	call SendW;

	ay0=WRITEOK;
	CNTR=BlockSize;
	DO SendB UNTIL CE;

RWLop:	ax0=DM(Status);
	ar=ax0 and ay0;
	if EQ jump RWLop;

	ar=DM(I1,m1);
	sr=ASHIFT ar by 4 (LO);
SendB:	DM(Write68)=sr0;

	rts;
{////////////////////////////////////////////////////////////////////}
RecordOn:
	call InitBuffer;

	DM(I0,m0)=0x0000;
	DM(I0,m0)=0x0000;

	ax0=i0;
	ay0=BLOCKSIZE;
	ar=ax0 and ay0;
	DM(OldBit)=ar;

	ar=0x0000;
	DM(BuffersNeeded)=ar;

	M7{MODE}=RecMode;

	ar=0x0004;
	jump IntBitOn;
{////////////////////////////////////////////////////////////////////}
RecordOff:
	M7{MODE}=IntOnMode;
	ar=0x0004;
	jump IntBitOff;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////				Delay				/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{	          +---------------+                       }
{	          |        +------+                       }
{	        |VOL3|     |    |VOL2|                    }
{	          +--------+      V                       }
{	Input-----+---------------+-----|VOL1|-> Output   }


DelayInt:
	ar=DM(Input);
	sR=LSHIFT ar by 4 (HI);
	ay0=sr1;
	MY0=DM(VAR1);		{VOL1}
	MR=SR1 * MY0 (SU);
 
	m0=0;
	ar=DM(i0,m0);
	m0=1;
	MY0=DM(VAR2);		{VOL2}
	MR=MR+AR*MY0 (SU);
	if mv sat mr;

	si=mr1;

	ay1=DM(Output);
	ENA AR_SAT;
	ar=sr1+ay1;
	DIS AR_SAT;
	DM(Output)=ar;

	sR=ASHIFT ar by -1 (HI);

	MY0=DM(VAR3);		{VOL1}
	MR=AR * MY0 (SU);

	ENA AR_SAT;
	ar=mr1+ay0;
	DIS AR_SAT;

	DM(i0,m0)=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
DelayOn:
	call InitBuffer;

	L0=0x2000;

	ar=0x0008;
	jump IntBitOn;
{////////////////////////////////////////////////////////////////////}
DelayOff:
	call InitBuffer;

	L0=0x2000;

	ar=0x0008;
	jump IntBitOn;
{////////////////////////////////////////////////////////////////////}
DelayBuff:
	ar=DM(VAR0);
	l0=ar;
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
HandleInt:
	ar=DM(ADC);
	sr=LSHIFT ar by 4 (lo);
	DM(Input)=sr0;

	ax0=DM(IntEvents);
	ay0=0x0001;
	ar=ax0 and ay0;
	if ne Call MonInt;

	ax0=DM(IntEvents);
	ay0=0x0002;
	ar=ax0 and ay0;
	if ne Call PlayInt;

	ax0=DM(IntEvents);
	ay0=0x0004;
	ar=ax0 and ay0;
	if ne Call RecInt;

	ax0=DM(IntEvents);
	ay0=0x0008;
	ar=ax0 and ay0;
	if ne Call DelayInt;

	call HiLoSave;

	ar=DM(Output);
	sr=LSHIFT ar by -4 (lo);
	DM(DAC)=sr0;
	ar=0;
	DM(Output)=ar;
	rts;

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}

IntbitON:
	ay0=DM(IntEvents);
	ar=ar or ay0;
	DM(IntEvents)=ar;
	rts;

IntbitOFF:
	ay0=0xFFFF;
	af=ar xor ay0;
	ax0=DM(IntEvents);
	ar=ax0 and af;
	DM(IntEvents)=ar;
	rts;

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
SwapI0:
	ar=DM(I0save);
	DM(I0save)=I0;
	I0=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
SwapI1:
	ar=DM(I1save);
	DM(I1save)=I1;
	I1=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
SwapI2:
	ar=DM(I2save);
	DM(I2save)=I2;
	I2=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
SwapI3:
	ar=DM(I3save);
	DM(I3save)=I3;
	I3=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
.endmod;
