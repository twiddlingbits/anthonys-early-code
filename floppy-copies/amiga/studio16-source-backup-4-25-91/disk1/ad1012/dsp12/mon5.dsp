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

.const		NullMode=0x0000;
.const		MonMode=0x0001;
.const		RecMode=0x0002;
.const		PlayMode=0x0003;
.const		DelayMode=0x0003;
.const		NoMode=0x0007;

.const		MsgHi=	0x0001;
.const		MsgLow=	0x0011;
.const		MsgRecData=	0x0003;
.const		MsgPlayData=	0x0005;

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
.var/dm/ram	BuffersNeeded;
.var/dm/ram	ReceiveNeeded;
.var/dm/ram	Save1;
.var/dm/ram	Save2;
.var/dm/ram	Save3;

.const		SMPTEsyncword=0x2FFD;
.var/dm/ram	SMPTEmode;
.var/dm/ram	SMPTEctr;
.var/dm/ram	SMPTEsecframe;
.var/dm/ram	SMPTEhourmin;
{////////////////////////////////////////////////////////////////////}
{/////			Interupt Vectors			/////}
{////////////////////////////////////////////////////////////////////}
	jump start;	rti;		rti;		rti;	{Reset Vector}
	ENA SEC_REG;	call (I7);	DIS SEC_REG;	rti;	{irq2 A2D}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	ENA SEC_REG;	call Command; 	DIS SEC_REG;	rti;	{irq1 I68000}
	ENA SEC_REG;	call Smpte;	DIS SEC_REG;	rti;	{irq0 SMPTE}
	ENA SEC_REG;	call HiLow;	DIS SEC_REG;	rti;	{timer}
{////////////////////////////////////////////////////////////////////}
{/////			Start Execution				/////}
{////////////////////////////////////////////////////////////////////}
start:  call CntlReg_inits;	{ set up SPORTS, TIMER, etc }
	icntl=b#00111;		{ disable IRQ nesting, all IRQs edge-sensitive }

	si=0;
	DM(SMPTEmode)=si;

	si=0x00e2;
	DM(VolFreq)=si;		{ Set Volume and Cutoff Frequency }
	DM(VFsave)=si;		{ Set Volume and Cutoff Frequency }

	Call Exit;

	si=DM(Read68);

	ar=0x7000;
	DM(VAR1)=ar;
	DM(VAR2)=ar;
	DM(VAR3)=ar;

	ENA TIMER;
	IFC=b#000000111111;
{|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||}
lop:	imask=b#100101;

	ax1=M7{MODE};		{M7 = MODE}
	ay1=0x0007;
	ar=ax1 and ay1;

	ay1=^JumpMain;
	ar=ar+ay1;
	I5=ar;
	jump (I5);

JumpMain:
	jump	Off;
	jump	MonitorS;
	jump	RecordS;
	jump	PlayS;  { PLAY_SND }
	jump	lop;
	jump	lop;
	jump	lop;
	jump	lop;
{|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||}
Off:
	ay0=0x0001;
	ax0=DM(FLAG);
	ar=ax0 and ay0;
	if EQ jump lop;

	ar=MsgHi;
	call SendW;

	ar=MsgLow;
	call SendW;

	ax0=0;
	DM(FLAG)=ax0;

	jump lop;
{|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||}
MonitorS:
	ay0=0x0001;
	ax0=DM(FLAG);
	ar=ax0 and ay0;
	if EQ jump lop;

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

	ax0=0;
	DM(FLAG)=ax0;

	jump lop;
{|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||}
RecordS:ay0=DM(BuffersNeeded);
	ar=pass ay0;
	if EQ jump lop;

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

	jump lop;
{|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||}
PlayS:	imask=b#000000;

	ar=DM(ReceiveNeeded);
	ar=pass ar;
	if NE jump lop;

	ay0=DM(BuffersNeeded);
	ar=pass ay0;
	if EQ jump lop;

	ar=ay0-1;
	DM(BuffersNeeded)=ar;

	ar=BLOCKSIZE;
	DM(ReceiveNeeded)=ar;

	imask=b#100101;

	ar=MsgPlayData;
	call SendW;

	jump lop;
{////////////////////////////////////////////////////////////////////}
{/////			NULL Interupt				/////}
{////////////////////////////////////////////////////////////////////}
NullInt:	rts;
{////////////////////////////////////////////////////////////////////}
{/////			Play Interupt				/////}
{////////////////////////////////////////////////////////////////////}
PlayInt:ax0=i0;
	ay0=BLOCKSIZE;
	ar=ax0 and ay0;
	ax1=ar;

	ay1=DM(OldBit);
	ar=ax1 xor ay1;			{Wait for bit to toggle}
	if EQ jump Pskip;

	DM(OldBit)=ax1;
	ay0=DM(BuffersNeeded);
	ar=ay0+1;
	DM(BuffersNeeded)=ar;

Pskip:	ay0=0xFFFF;
	ax0=DM(I0,m0);			{ ar = b XXXXssssssssssss }
	ar=ax0-ay0;
	if EQ jump Pstop;

	DM(DAC)=ax0;
	rts;

Pstop:
	M7{MODE}=DM(MODEsave);
	I7{InteruptVector}=L7{SavedIntVector};			{Restore old interupt vector}
	rts;
{////////////////////////////////////////////////////////////////////}
{/////			Record Interupt				/////}
{////////////////////////////////////////////////////////////////////}
RecInt:	ar=DM(ADC);
	DM(DAC)=ar;

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
{/////			Monitor Interupt			/////}
{////////////////////////////////////////////////////////////////////}
MonInt:	ar=DM(ADC);
	DM(DAC)=ar;

{ Save Highest/lowest byte }
	sr=LSHIFT ar by 4 (HI);
	sr=ASHIFT sr1 by -8 (HI);	{ b SSSSSSSSssssssss}
	ay0=sr1;

	ay1=0x8000;
	ar=sr1 and ay1;
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
{/////			Delay Interupt				/////}
{////////////////////////////////////////////////////////////////////}
{	          +---------------+                        }
{	          |        +------+                   }
{	        |VOL3|     |    |VOL2|                    }
{	          +--------+      V                       }
{	Input-----+---------------+-----|VOL1|-> Output   }


DelayInt:
	ar=DM(ADC);
	sR=LSHIFT ar by 4 (HI);
	ay0=sr1;
	MY0=DM(VAR1);		{VOL1}
	MR=SR1 * MY0 (SU);

	ar=DM(i0,m0);
	MY0=DM(VAR2);		{VOL2}
	MR=MR+AR*MY0 (SU);
	if mv sat mr;

	si=mr1;
	sR=ASHIFT si by -4 (HI);
	DM(DAC)=sr1;

	sR=ASHIFT ar by -1 (HI);


	MY0=DM(VAR3);		{VOL1}
	MR=AR * MY0 (SU);

	ENA AR_SAT;
	ar=mr1+ay0;
	DIS AR_SAT;

	DM(i0,m0)=ar;
	rts;

{////////////////////////////////////////////////////////////////////}
{/////			Get Command from 68000			/////}
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
	jump MonitorOn;		{0x__05}
	jump MonitorOff;	{0x__07}
	jump SetTimer;		{0x__09  Period=VAR0  Scale=VAR1}
	jump RecordOn;		{0x__0B}
	jump RecordOff;		{0x__0D}
	jump PlaySample;	{0x__0F}
	jump EndSample;		{0x__11}
	jump StopPlay;		{0x__13}
	jump SetSamplingRate;	{0x__15  Rate=VAR0}
	jump DelayOn;		{0x__17}
	jump DelayOff;		{0x__19} 
	jump DelayBuff;		{0x__1B  BufferLen=VAR0}
	rts;			{0x__1D}
	jump Exit;		{0x__1F}
	jump VAR0lo;		{0xdd21}
	jump VAR0hi;		{0xdd23}
	jump VAR1lo;		{0xdd25}
	jump VAR1hi;		{0xdd27}
	jump VAR2lo;		{0xdd29}
	jump VAR2hi;		{0xdd2B}
	jump VAR3lo;		{0xdd2D}
	jump VAR3hi;		{0xdd2F}
	rts;			{0x__31}
	rts;			{0x__33}
	rts;			{0x__35}
	rts;			{0x__37}
	rts;			{0x__39} 
	rts;			{0x__3B}
	rts;			{0x__3D}
	rts;			{0x__3F}

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
MonitorOn:
	I7{InteruptVector}=^MonInt;
	M7{MODE}=MonMode;
	rts;
{---------------------------------------------------------------------}
MonitorOff:
	I7{InteruptVector}=^NullInt;
	M7{MODE}=NullMode;
	rts;
{---------------------------------------------------------------------}
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

	DM(MODEsave)=M7{MODE};
	M7{MODE}=RecMode;

	L7{SavedIntVector}=I7{InteruptVector};			{ Save old Interupt Vector }
	I7{InteruptVector}=^RecInt;

	rts;
{---------------------------------------------------------------------}
RecordOff:
	I7{InteruptVector}=L7{SavedIntVector};			{ Restore old Interupt Vector }
	M7{MODE}=DM(MODEsave);
	rts;
{---------------------------------------------------------------------}
PlaySample:

	call InitBuffer;

	DM(I0,m0)=0x0000;
	DM(I0,m0)=0x0000;

	ar=NUMBUFFERS;
	DM(BuffersNeeded)=ar;
	ar=0x0000;
	DM(ReceiveNeeded)=ar;

	DM(MODEsave)=M7{MODE};
	M7{MODE}=PlayMode;

	ax0=i0;
	ay0=BLOCKSIZE;
	ar=ax0 and ay0;
	DM(OldBit)=ar;

	L7{SavedIntVector}=I7{InteruptVector};			{ Save old Interupt Vector }
	I7{InteruptVector}=^PlayInt;

	rts;


{---------------------------------------------------------------------}
EndSample:
	ax0=0xFFFF;
	DM(I1,m1)=ax0;
	rts;
{---------------------------------------------------------------------}
StopPlay:
	I7{InteruptVector}=L7{SavedIntVector};			{ Restore old Interupt Vector }
	M7{MODE}=DM(MODEsave);
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
DelayOn:
	call InitBuffer;

	L0=0x1FFF;

	DM(MODEsave)=M7{MODE};
	M7{MODE}=DelayMode;

	L7{SavedIntVector}=I7{InteruptVector};			{ Save old Interupt Vector }
	I7{InteruptVector}=^DelayInt;
	rts;
{---------------------------------------------------------------------}
DelayOff:
	I7{InteruptVector}=L7{SavedIntVector};			{ Restore old Interupt Vector }
	M7{MODE}=DM(MODEsave);
	rts;
{---------------------------------------------------------------------}
DelayBuff:
	ar=DM(VAR0);
	l0=ar;
	rts;
{---------------------------------------------------------------------}
Exit:	L7{SavedIntVector}=^MonInt;
	I7{InteruptVector}=^MonInt;
	M7{MODE}=NoMode;
	rts;
{-------------------------- 0x9??? CMDS ------------------------------}
{-------------------------- 0x9??? CMDS ------------------------------}
{-------------------------- 0x9??? CMDS ------------------------------}
{-------------------------- 0x9??? CMDS ------------------------------}
{-------------------------- 0x9??? CMDS ------------------------------}
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
{---------------------------------------------------------------------}

{------------------------ -- Play Data -- ----------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
{---------------------------------------------------------------------}
PlayData:sr=ASHIFT si by -4 (HI);
	DM(I1,m1)=sr1;
	ay0=DM(ReceiveNeeded);
	ar=ay0-1;
	DM(ReceiveNeeded)=ar;
	rts;
{////////////////////////////////////////////////////////////////////}
{/////			HighLow Send				/////}
{////////////////////////////////////////////////////////////////////}
HiLow:	DM(Save1)=ar;

	ar=0x0001;
	DM(FLAG)=ar;

	ar=DM(Save1);
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
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
 { 0x3FFE } DM(I0,M0)=0x2492;	{DM WAIT STATES two for each}
 { 0x3FFF } DM(I0,M0)=0x1018;	{SPORT0 ENABLED, BOOT PAGE 0}
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
	DM(Save1)=ay0;
	DM(Save2)=ar;

	ay0=WRITEOK;
WLoop:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump WLoop;

	ar=DM(Save2);
	DM(Write68)=ar;			{ xxxx xxxx  xxxx xxxx }

	DM(Interupt68)=ar;

	ar=DM(Save2);
	ay0=DM(Save1);
	rts;

{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}

InitBuffer:
	I0=^buffer;
	I1=^buffer;
	m0=1;
	m1=1;
	L0=0x1000;
	L1=0x1000;

	CNTR=%buffer;
	DO ClearBuff UNTIL CE;
	DM(I0,m0)=0x0000;
ClearBuff:DM(I1,m1)=0x0000;
	rts;

{////////////////////////////////////////////////////////////////////}
{/////				SMPTE				/////}
{////////////////////////////////////////////////////////////////////}
Smpte:	si=DM(SMPTEdata);
	sr=LSHIFT si by 1 (lo);
	IF NOT FLAG_IN JUMP SMP0;
	ay0=1;
	ar=sr0+ay0;
	sr0=ar;
SMP0:	DM(SMPTEdata)=sr0;
	ax0=sr0;

	ay0=DM(SMPTEmode);
	af=PASS ay0;
	if eq jump Sdata;
	jump Ssearch;

Ssearch:ay0=SMPTEsyncword;
	ar=ax0-ay0;
	if EQ rts;
	si=0;
	DM(SMPTEmode)=si;
	DM(SMPTEctr)=si;
	rts;

Sdata:	ay0=DM(SMPTEctr);
	ar=ay+1;
	DM(SMPTEctr)=ar;
	ay0=ar;

	ax1=64;
	ar=ax1-ay0;
	if eq jump EndFrame;
	ax1=28;
	ar=ax1-ay0;
	if eq jump DM(SMPTEsecframe)=ax0;
	ax1=44;
	ar=ax1-ay0;
	if eq jump DM(SMPTEhourmin)=ax0;
	rts;

EndFrame:
	si=1;
	DM(SMPTEmode)=si;
	DM(SMPTEctr)=si;
	rts;

.endmod;
