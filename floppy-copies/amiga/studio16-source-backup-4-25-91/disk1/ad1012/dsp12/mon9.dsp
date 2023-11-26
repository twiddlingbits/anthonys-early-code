{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{/////				MONITOR	9			/////}
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

.const		EVmon=0x0004;
.const		EVrec=0x0008;

.const		EVplay0=0x0010;
.const		EVplay1=0x0020;
.const		EVplay2=0x0040;
.const		EVplay3=0x0080;

.const		SampRate=0x3FF1;

.const		WRITEOK=0x0002;
.const		READOK=0x0001;

.const		BLOCKSIZE=0x0100;
.const		BUFFERSIZE=0x0400;
.const		NUMBUFFERS=0x0004;

.const		MsgHi=0x0001;
.const		MsgLow=0x0007;
.const		MsgRecData=0x0009;
.const		MsgPlayData=0x000F;

.var/dm/ram/circ buffer0[0x0400];
.var/dm/ram/circ buffer1[0x0400];
.var/dm/ram/circ buffer2[0x0400];
.var/dm/ram/circ buffer3[0x0400];
.var/dm/ram/circ bufferDelay[0x1000];
.var/dm/ram	VFsave;

.var/dm/ram	VAR0;
.var/dm/ram	VAR1;
.var/dm/ram	VAR2;
.var/dm/ram	VAR3;
.var/dm/ram	Lowest;
.var/dm/ram	Highest;

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

.var/dm/ram	DataOut0;
.var/dm/ram	DataOut1;
.var/dm/ram	DataOut2;
.var/dm/ram	DataOut3;

.var/dm/ram	DataIn0;
.var/dm/ram	DataIn1;
.var/dm/ram	DataIn2;
.var/dm/ram	DataIn3;

.var/dm/ram	OldBit;
.var/dm/ram	OldBit0;
.var/dm/ram	OldBit1;
.var/dm/ram	OldBit2;
.var/dm/ram	OldBit3;

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

	si=1;
	DM(SMPTEmode)=si;
	DM(SMPTEctr)=si;

	si=0x00e2;
	DM(VolFreq)=si;		{ Set Volume and Cutoff Frequency }
	DM(VFsave)=si;		{ Set Volume and Cutoff Frequency }

	si=DM(Read68);

{	Call ModeOff;}

	si=0;
	DM(FLAG)=si;

	si=0x7FFF;
	DM(Vol0)=si;
	DM(Vol1)=si;
	DM(Vol2)=si;
	DM(Vol3)=si;
	DM(VolMon)=si;

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
	nop; {Monitor}
	nop; {Record}

	nop;
	nop;
	nop;
	nop;
MainEvents:
	nop;
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
Hplay:	ay0=EVplay0;
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
HRsmp:	ay0=EVsmpR;
	ax0=DM(FLAG);
	ar=ax0 and ay0;
	if EQ rts;

	ar=ax0 xor ay0;
	DM(FLAG)=ar;

	ay0=0x0004;
	call bitOFF;
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
	I6=ar;
	jump (I6);

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
	rts;			{0x__15}
	jump DelayOn;		{0x__17}
	jump DelayBuff;		{0x__19  BufferLen=VAR0}
	rts;			{0x__1B} 
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
	jump SetCVol;		{0x_D33}
	rts;			{0x__35}
	rts;			{0x__37}
	rts;			{0x__39}
	jump SMPTEreq;		{0x__3B}
	jump bitOFF;		{0xDD3D}
	jump bitON;		{0xDD3F}
{---------------------------------------------------------------------}
SetCVol:
	ay1=^JumpTableVm;
	si=ax1;
	ax1=DM(VAR0);

	sr=LSHIFT si by 1 (lo);
	ar=sr0+ay1;
	I6=ar;
	jump (I6);

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
VAR0lo:	I6=^VAR0;
	jump SetLo;
VAR0hi:	I6=^VAR0;
	jump SetHi;
VAR1lo:	I6=^VAR1;
	jump SetLo;
VAR1hi:	I6=^VAR1;
	jump SetHi;
VAR2lo:	I6=^VAR2;
	jump SetLo;
VAR2hi:	I6=^VAR2;
	jump SetHi;
VAR3lo:	I6=^VAR3;
	jump SetLo;
VAR3hi:	I6=^VAR3;
	jump SetHi;
{---------------------------------------------------------------------}
SetLo:	m6=0;
	L6=0;
	ar=DM(I6,m6);
	ay1=0xFF00;
	af=ar and ay1;

	ar=ax1 or af;
	DM(I6,m6)=ar;
	rts;
{---------------------------------------------------------------------}
SetHi:	m6=0;
	L6=0;
	ar=DM(I6,m6);

	si=ax1;
	sr= LSHIFT si by 8 (LO);

	ay1=0x00FF;
	af=ar and ay1;

	ar=sr0 or af;
	DM(I6,m6)=ar;
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

	l4=0;		{ for selfmodifying code }
	m4=8;

	m1=1;
	m0=0;

	L1=%buffer0;

	L0=%buffer0;

	I0=^buffer0;
	DM(DataIn0)=I0;

	I0=^buffer1;
	DM(DataIn1)=I0;

	I0=^buffer2;
	DM(DataIn2)=I0;

	I0=^buffer3;
	DM(DataIn3)=I0;

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
	DM(I0,m0)=0x0000;
	DM(I0,m0)=0x0000;

	ax0=i0;
	ay0=BLOCKSIZE;
	ar=ax0 and ay0;
	DM(OldBit)=ar;

	ar=0x0000;
	DM(BuffersNeeded)=ar;

	ar=EVrec;
	jump EventOn;
{////////////////////////////////////////////////////////////////////}
RecordOff:
	ar=EVrec;
	jump EventOff;
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
 
	ar=DM(i0,m0);
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

	DM(i0,m1)=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
DelayOn:
	L0=0x2000;

	ar=0x0008;
	jump EventOn;
{////////////////////////////////////////////////////////////////////}
DelayOff:
	L0=0x2000;

	ar=0x0008;
	jump EventOn;
{////////////////////////////////////////////////////////////////////}
DelayBuff:
	ar=DM(VAR0);
	l0=ar;
	rts;

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
{/////				Play				/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{ I7 = pointer to buff }
{ ay1= Old Bit }
{ m6 = }
{ l6 = }
{ I6 = }
{////////////////////////////////////////////////////////////////////}
Play0Int:
	my0=DM(Vol0);
	ay1=DM(OldBit0);
	I0=DM(DataOut0);
	sr1=EVplay0;
	call PlInt;
	DM(DataOut0)=I0;
	DM(OldBit0)=ay1;
	rts;
Play1Int:
	my0=DM(Vol1);
	ay1=DM(OldBit1);
	I0=DM(DataOut1);
	sr1=EVplay1;
	call PlInt;
	DM(DataOut1)=I0;
	DM(OldBit1)=ay1;
	rts;
Play2Int:
	my0=DM(Vol2);
	ay1=DM(OldBit2);
	I0=DM(DataOut2);
	sr1=EVplay2;
	call PlInt;
	DM(DataOut2)=I0;
	DM(OldBit2)=ay1;
	rts;
Play3Int:
	my0=DM(Vol3);
	ay1=DM(OldBit3);
	I0=DM(DataOut3);
	sr1=EVplay3;
	call PlInt;
	DM(DataOut3)=I0;
	DM(OldBit3)=ay1;
	rts;

PlInt:	sr0=I0;
	ay0=BLOCKSIZE;
	ar=sr0 and ay0;

	af=ar xor ay1;		{Wait for bit to toggle}
	if EQ jump Pskip;

	ay1=ar;
	ay0=DM(FLAG);
	ar=sr1 or ay0;
	DM(FLAG)=ar;

Pskip:	ay0=0xFFFF;
	ar=DM(I0,M1);		{ ar = b XXXXssssssssssss }
	af=ar-ay0;
	if EQ jump StopChannel;	{ STOP PLAY }

	MR=MR+ar*my0 (SU);  { my0 = volume     ar = Sample }
	rts;
{////////////////////////////////////////////////////////////////////}
PlaySample:
	ar=ax1;
	sr=LSHIFT ar by 2 (lo);
	ay0=^PlaySampJumpTable;
	ar=sr0+ay0;
	I6=ar;
	jump (I6);

PlaySampJumpTable:

	ax0=DM(DataOut0);
	ay0=BLOCKSIZE;
	ar=ax0 and ay0;
	DM(OldBit0)=ar;
	ar=EVplay0;
	jump EventOn;
	rts;
	rts;

	ax0=DM(DataOut1);
	ay0=BLOCKSIZE;
	ar=ax0 and ay0;
	DM(OldBit1)=ar;
	ar=EVplay1;
	jump EventOn;
	rts;
	rts;

	ax0=DM(DataOut2);
	ay0=BLOCKSIZE;
	ar=ax0 and ay0;
	DM(OldBit2)=ar;
	ar=EVplay2;
	jump EventOn;
	rts;
	rts;

	ax0=DM(DataOut3);
	ay0=BLOCKSIZE;
	ar=ax0 and ay0;
	DM(OldBit3)=ar;
	ar=EVplay3;
	jump EventOn;
	rts;
	rts;

{////////////////////////////////////////////////////////////////////}
StopPlay:
	DM(Channel)=ax1;
StopChannel:
	ar=DM(Channel);
	sr=LSHIFT ar by 2 (lo);

	ay0=^StopPlayJumpTable;
	ar=sr0+ay0;
	I6=ar;
	jump (I6);

StopPlayJumpTable:
	I0=^buffer0;
	DM(DataIn0)=I0;
	ar=EVplay0;
	jump EventOff;

	I0=^buffer1;
	DM(DataIn1)=I0;
	ar=EVplay1;
	jump EventOff;

	I0=^buffer2;
	DM(DataIn2)=I0;
	ar=EVplay2;
	jump EventOff;

	I0=^buffer3;
	DM(DataIn3)=I0;
	ar=EVplay3;
	jump EventOff;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
EndSample:
	ar=ax1;
	sr=LSHIFT ar by 2 (lo);
	ax1=0xFFFF;
	jump DataIn;
{////////////////////////////////////////////////////////////////////}
PlayData:{ax0 = raw data }
	ay0=0x0006;		{ 4 Chanels 0-3 }
	ar=ax0 and ay0;
	sr=LSHIFT ar by 1 (lo);

	ay0=0xFFF0;		{ DATA }
	ar=ax0 and ay0;
	ax1=ar;

{////////////////////////////////////////////////////////////////////}
DataIn:	ay0=^DataInJumpTable;
	ar=sr0+ay0;
	I6=ar;
	jump (I6);

DataInJumpTable:
	I0=DM(DataIn0);
	DM(I0,m1)=ax1;
	DM(DataIn0)=I0;
	rts;
	I0=DM(DataIn1);
	DM(I0,m1)=ax1;
	DM(DataIn1)=I0;
	rts;
	I0=DM(DataIn2);
	DM(I0,m1)=ax1;
	DM(DataIn2)=I0;
	rts;
	I0=DM(DataIn3);
	DM(I0,m1)=ax1;
	DM(DataIn3)=I0;
	rts;
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{	Call Play0Int;
	Call Play1Int;
	Call Play2Int;
	Call Play3Int;}
{////////////////////////////////////////////////////////////////////}
IntEventsTable:
	nop; {Hhilo}
	nop; {Hsmpte}
	Call MonInt;
	nop;{Call RecInt}

	Call Play0Int;
	nop;
	nop;
	nop;
IntEvents:
	nop;
	nop;
IntEn:	Call MonInt;		{ << Powerup Monitoring}
	nop;

	nop;
	nop;
	nop;
	nop;
	rts;

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
HandleInt:
	ar=DM(ADC);
	sr=LSHIFT ar by 4 (lo);	{ Get Byte from Sampler }
	DM(Input)=sr0;

	mr=0;			{ Clear Mult (output) }

	call IntEn;
	if mv sat mr;

	call HiLoSave;

	sr=ASHIFT mr1 by -4 (hi);
	DM(DAC)=sr1;
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
bitON:	ar=ax1;
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
	ar=^MainEventsTable;
	ar=ar+af;
	i4=ar;
	ar=PM(i4,m4);
	PM(i4,m4)=ar;

	ar=^IntEventsTable;
	ar=ar+af;
	i4=ar;
	ar=PM(i4,m4);
	PM(i4,m4)=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
bitOFF:	ar=ax1;
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

MainOff:{ af=offset }	{ MODIFIED : AR I4 PX}
	ar=^MainEvents;
	ar=ar+af;
	i4=ar;
	ar=0;
	px=0;
	PM(i4,m4)=ar;

	ar=^IntEvents;
	ar=ar+af;
	i4=ar;
	ar=0;
	px=0;
	PM(i4,m4)=ar;
	rts;
{////////////////////////////////////////////////////////////////////}

{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
.endmod;

Play0Int:
	I0=DM(DataOut0);
	my0=DM(Vol0);
	ay1=DM(OldBit0);
	sr1=EVplay0;
	call PlInt;
	DM(DataOut0)=I0;
	DM(OldBit0)=ay1;
	rts;

PlayInt:sr0=I1;
	ay0=BLOCKSIZE;
	ar=sr0 and ay0;

	af=ar xor ay1;
	if EQ jump Pskip;

	ay1=ar;
	ay0=DM(FLAG);
	ar=sr1 or ay0;
	DM(FLAG)=ar;

Pskip:	I0=I1;
	m2=BUFFERSIZE;
	CNTR=4;
	ax1=0;
	i2=^Vol0;

	DO PlyLp UNTIL CE;
	ar=DM(I0,M0);
	my0=dM(i2,m1);
PlyLP:	MR=MR+ar*my0 (SU)	,	DM(I0,M2)=ax1;


	Modify(I1,M1);

	rts;
{////////////////////////////////////////////////////////////////////}
