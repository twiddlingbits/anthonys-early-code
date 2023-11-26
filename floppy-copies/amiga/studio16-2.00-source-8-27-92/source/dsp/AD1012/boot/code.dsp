{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{/////				Boot Epprom			/////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
.module/ram/abs=0/boot=0		Monitor;
.const		Srate=60;
.port		ADC;
.port		DAC;
.port		VolFreq;
.port		Read68;
.port		Write68;
.port		Status;
.port		Interupt68;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.global		Status	,Read68;
.external	LoadCode,WaitRead;
.entry		Begin;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram/circ buffer[0x2000];
.var/pm/ram/abs=0x03f0 Sware;
.var/pm/ram/abs=0x03f1 EprmRev;
.var/pm/ram/abs=0x03f2 BoardRev;
.var/pm/ram/abs=0x03f3 Flags;
.var/dm/ram	VFsave;
.var/dm/ram	InRig;
.var/dm/ram	ErrorTst;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.init EprmRev: 0x000501;
{++++++++++++++++++++++++}
{++++++++++++++++++++++++}
.init BoardRev: 0x050002;
{ Board Revision 0xbb.bbcd   b=board revision c=AutoConfigpal(U15) rev  d=pal1(U24) rev}
{++++++++++++++++++++++++}
.init Flags: 0x000200;
{memory     bits  0-7       1=8k  4=32k  F=128k}
{filter     bits  8-11      0000=Bessel LTC1064-3  0001=Butterworth LTC1064-2V4.2   0002=Butterworth LTC1064-2V5.0}
{           bits 12-23     unused}
{++++++++++++++++++++++++}
.init Sware: 0xFFFFFF;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		WRITEOK=0x0002;
.const		READOK=0x0001;
.const		IntStat=0x0004;
.const		InTestRig=0x0008;
.const		SampRate=0x3FF1;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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
{////////////////////////////////////////////////////////////////////}
{/////			Interupt Vectors			/////}
{////////////////////////////////////////////////////////////////////}
Begin:	nop;		nop;		nop;		jump start;	{Reset Vector}
	si=DM(ADC); 
SMC:	call Beep;	DM(DAC)=si;	rti;			{irq2 A2D}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}

{////////////////////////////////////////////////////////////////////}
{/////			Start Execution				/////}
{////////////////////////////////////////////////////////////////////}
start:	call CntlReg_inits;	{ set up SPORTS, TIMER, etc }

	ar=0x091F;
	DM(VolFreq)=ar;		{ Set Volume and Cutoff Frequency }
	DM(VFsave)=ar;		{ Set Volume and Cutoff Frequency }

	ax1=100;call GainDown;
	ax1=50;	call GainUp;

	ar=DM(Read68);
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
	ar=0x0001;dm(InRig)=ar;
	ay0=InTestRig;
	ar=DM(Status);
	ar=ar and ay0;
	if NE jump TestCard;

	ar=0x0000;dm(InRig)=ar;

	Call MemoryTest;

	call CardPassedSound;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
lop:	ay0=READOK;
BSLoop:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump BSLoop;

	ar=DM(Read68);
	ay0=0x003d;
	af=ar-ay0;
	if EQ jump SendRev;
	ay0=0x003f;
	af=ar-ay0;
	if NE jump lop;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
NewCode:imask=b#000000;
	m7=1;l7=0;
	Wait(READOK);
	ar=DM(Read68);
	i7=ar;
	Wait(READOK);
	ar=DM(Read68);
	CNTR=ar;
	jump LoadCode;
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
	jump lop;
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
 { 0x3FFD } DM(I0,M0)=0x0C00;	{TIMER TPERIOD Period Register}
 { 0x3FFE } DM(I0,M0)=0x1249;	{DM WAIT STATES one for each}
 { 0x3FFF } DM(I0,M0)=0x1018;	{SPORT0 ENABLED, BOOT PAGE 0}
	icntl=b#00111;
	IFC=b#000000111111;
	rts;

{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
{++++++++++++++++++++++++}
{ Determine Memory Size  }
{++++++++++++++++++++++++}
FindMem:
	I7=^buffer;
	l7=%buffer;
	m4=0;

	ar=0x011F;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x011E;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x011D;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x011C;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x011B;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x011A;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x0119;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x0118;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x0117;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x0116;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x0115;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x0114;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x0113;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x0112;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x0111;DM(VolFreq)=ar;DM(I7,m4)=ar;
	ar=0x0110;DM(VolFreq)=ar;DM(I7,m4)=ar;

	ar=0x011F;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x011E;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x011D;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x011C;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x011B;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x011A;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x0119;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x0118;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x0117;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x0116;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x0115;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x0114;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x0113;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x0112;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x0111;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ar=0x0110;DM(VolFreq)=ar;ay1=DM(I7,m4);ar=ar-ay1;if EQ rts;
	ay0=4; jump ErrorTone;

MemoryTest:

	Call FindMem;
	ax1=0x000F;
	ar=ax1 and ay1;
	ay1=ar;
	ax1=ay1;
	ar=3;ar=ar-ay1; if EQ jump GOOOO;
	ar=15;ar=ar-ay1; if EQ jump GOOOO;
	ay0=5; jump ErrorTone;


GOOOO:	I7=^Flags;
	ar=pm(I7,m4);
	px=ax1;
	pm(I7,m4)=ar;

{++++++++++++++++++++++++}
	ay0=0x0000;
{++++++++++++++++++++++++}
	cntr=ax1;
	DO RT_wl2 UNTIL CE;
	ar=cntr;
	af=PASS ar;
	ar=0x0110;
	ar=ar+af;
	DM(VolFreq)=ar;

	I7=^buffer;
	l7=%buffer;
	m5=1;
	cntr=0x1FFF;				{ Write Ram Test }

	ar=0x835A;
	ar=ar xor ay0;
	ay0=ar;

	DO RT_wl UNTIL CE;
	ar=ay0+1;
	ay0=ar;
RT_wl:	DM(I7,m5)=ar;
RT_wl2:	nop;
{++++++++++++++++++++++++}
	ay0=0x0000;
	cntr=ax1;
	DO RT_rl2 UNTIL CE;
	ar=cntr;
	af=PASS ar;
	ar=0x0110;
	ar=ar+af;
	DM(VolFreq)=ar;

	I7=^buffer;
	cntr=0x1FFF;				{ Read Ram Test }

	ar=0x835A;
	ar=ar xor ay0;
	ay0=ar;

	DO RT_rl UNTIL CE;
	ar=ay0+1;
	ay0=ar;
	ar=DM(I7,m5);
	ar=ar-ay0;
	if NE jump memerror;
RT_rl:	nop;
RT_rl2:	nop;
{++++++++++++++++++++++++}
{++++++++++++++++++++++++}
	cntr=0x0F;
	DO RT_cl2 UNTIL CE;
	ar=cntr;
	af=PASS ar;
	ar=0x0110;
	ar=ar+af;
	DM(VolFreq)=ar;

	I7=^buffer;
	l7=%buffer;
	cntr=0x1FFF;				{ Clear Ram Test }

	DO RT_cl UNTIL CE;
	ar=0x0000;
	DM(I7,m5)=ar;
RT_cl:	nop;
RT_cl2:	nop;

	l7=8;	{Restore old value}
	rts;

memerror:
	call CntlReg_inits;
	ay0=2; jump ErrorTone;
{////////////////////////////////////////////////////////////////////}
{/////			Sound Interupt				/////}
{////////////////////////////////////////////////////////////////////}
Beep:	ENA SEC_REG;
	ar=ay1+1;
	ay1=ar;
	ar=ay1+1;
	ay1=0x003F;
	ar=ar and ay1;
	ay1=ar;
	i5=ar;
	DIS SEC_REG;
	si=i5;
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
LatchTest:

	ay0=DM(Read68);

BITlp1:	ay0=0;

	ar=ay0+1;
	Wait(WRITEOK);
	DM(Write68)=ar;
	Wait(READOK);
	ay0=DM(Read68);

	ax0=1; ar=ax0-ay0; if EQ jump Inc;
	ax0=2; ar=ax0-ay0; if EQ jump DontInc;

	jump BITlp1;

DontInc:

	Wait(WRITEOK);
	DM(Write68)=ay0;

	ar=ay0+1; ax0=ar;
DI1:
	Wait(READOK);
	ay0=DM(Read68);

	ar=ax0-ay0; if NE jump LatchError;
	ax0=0xFFFF; ar=ax0-ay0; if EQ rts; {EndBITTest;}

	Wait(WRITEOK);
	DM(Write68)=ay0;

	ar=ay0+1; ax0=ar;

	jump DI1;

Inc:
	ax0=2;DM(Write68)=ax0;

IQ1:
	Wait(READOK);
	ay0=DM(Read68);

	ar=ax0-ay0; if NE jump LatchError;

	ax0=0xFFFF; ar=ax0-ay0; if EQ rts; {EndBITTest;}

	ar=ay0+1; ay0=ar;
	Wait(WRITEOK);
	DM(Write68)=ay0;
	ax0=ay0;

	jump IQ1;

LatchError:
	ay0=3; jump ErrorTone;

{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
DcOffsetTest:
	ar=60;  DM(SampRate)=ar;
	ar=0x0110; DM(VolFreq)=ar;

	Call InteruptMonitor;

	imask=b#100000;
	cntr=0x0400;
	DO D1 UNTIL CE;
	cntr=0x1FFF;
	DO D2 UNTIL CE;

	ar=si;
	sr = LSHIFT ar by 4 (HI); ar=sr1 ; sr= ASHIFT ar by -5 (HI);
	af=abs sr1;

	ax0=0x0007;ar=ax0 and af;ar=ar-af; if NE jump MAX;

	ar=abs sr1;
	se=ar;

	ar=1;
	sr = LSHIFT ar (LO);
	ar=sr0;
	jump overit;

MAX:	ar=0x0080;

overit:	nop;
	dm(Write68)=ar;
	dm(Interupt68)=ar;

D2:	nop;
D1:	nop;
	imask=b#000000;

	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
FilterTest:
	Call InteruptMonitor;

	ar=200; DM(SampRate)=ar;

	imask=b#100000;
	cntr=0x1000;
	DO D4 UNTIL CE;
	cntr=0x1FFF;
	DO D3 UNTIL CE;
	nop;
D3:	nop;

	ar=cntr; ay0=0x003F; ar=ar and ay0;

	if ne jump skip1;
		ar=cntr;
		sr=LSHIFT ar by -2 (lo);

		ay0=0x00F0; ar=sr0 and ay0;
		DM(VolFreq)=ar;

		sr=LSHIFT ar by -5 (lo);
		se=sr0;
		ar=1; sr=LSHIFT ar (lo);		{ VALID?}
		dm(Write68)=sr0;
		dm(Interupt68)=sr0;
skip1:	nop;
D4:	nop;
	imask=b#000000;

	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
PotTest:
	Call InteruptMonitor;

	ar=100; DM(SampRate)=ar;
	ax1=100; call GainDown;

	imask=b#100000;
	cntr=0x1000;
	DO D6 UNTIL CE;
	cntr=0x1FFF;
	DO D5 UNTIL CE;
	nop;
D5:	nop;

	ar=cntr; ay0=0x000F; ar=ar and ay0;

	if ne jump skip2;
		ar=cntr;
		ay0=0x0100; ar=ar and ay0;
		if EQ jump GD;
		ax1=1;call GainUp;
		jump D7;
GD:		ax1=1;call GainDown;

d7:
		ar=cntr;
		sr=LSHIFT ar by -4 (lo);
		ay0=0x00FF; ar=sr0 and ay0;
		dm(Write68)=ar;
		dm(Interupt68)=ar;
skip2:	nop;
D6:	nop;
	imask=b#000000;

	ax1=100; call GainDown;
	ax1=50;call GainUp;
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
TestCard:
	Call CardPassedSound;
	ar=0xFF; call UpdateLeds;
	Call PotTest;
	ar=0xFF; call UpdateLeds;
	Call FilterTest;
	ar=0xFF; call UpdateLeds;
	Call DcOffsetTest;
	ar=0xFF; call UpdateLeds;
	Call MemoryTest;
	ar=0xFF; call UpdateLeds;
{	Call LatchTest;}
	jump TestCard;

{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}

CardPassedSound:
	ar=400;DM(SampRate)=ar;
	Call InteruptBeep;
	imask=b#100000;
	ay0=562;

	cntr=0x200;
	DO WeetLp2 UNTIL CE;

	ar=ay0-1;
	ay0=ar;
	DM(SampRate)=ar;

	cntr=0x0800;
	DO WeetLp UNTIL CE;
	nop;
WeetLp:	nop;
WeetLp2:nop;

Weel:	ay0=600;
	cntr=0x100;
	DO WeelLp2 UNTIL CE;

	ar=ay0-1;
	ay0=ar;
	DM(SampRate)=ar;

	cntr=0x0800;
	DO WeelLp UNTIL CE;
	nop;
WeelLp:	nop;
WeelLp2:nop;

	cntr=0x100;
	DO WeelLp4 UNTIL CE;

	ar=ay0+1;
	ay0=ar;
	DM(SampRate)=ar;

	cntr=0x0800;
	DO WeelLp3 UNTIL CE;
	nop;
WeelLp3:nop;
WeelLp4:nop;

	imask=b#000000;
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
{ay0 = Error #}
{	Error #2 = Memory Error						}
{	Error #3 = Latch Error						}
{	Error #4 = No Memory Error					}

ErrorTone:
{++++++++++++++++++++++++}
	Call InteruptBeep;

KKK1:	ar=200; DM(SampRate)=ar;

	imask=b#000000;
	cntr=0x0080;
	DO lp3 UNTIL CE;
	sr0=cntr;
	ay1=b#100000; ax0=imask; ar=ax0 XOR ay1; imask=ar;
	cntr=sr0;
	DO lp2 UNTIL CE;
	cntr=0x0800;
	DO lp1 UNTIL CE;
	nop;
lp1:	nop;
	call ErrorInt;
lp2:	nop;
lp3:	nop;
	imask=b#000000;
{++++++++++++++++++++++++}
	ar=400;  DM(SampRate)=ar;

	ar=ay0;
	sr=LSHIFT ar by 1 (lo);
	cntr=sr0;
	DO lp4 UNTIL CE;

	cntr=0x0080;
	DO lp5 UNTIL CE;
	cntr=0x1FFF;
	DO lp6 UNTIL CE;
	nop;
lp6:	nop;
	call ErrorInt;
lp5:	nop;

	ay1=b#100000; ax0=imask; ar=ax0 XOR ay1; imask=ar;

lp4:	nop;
{++++++++++++++++++++++++}

	cntr=0x0200;
	DO lp8 UNTIL CE;
	cntr=0x1FFF;
	DO lp7 UNTIL CE;
	nop;
lp7:	nop;
	call ErrorInt;
lp8:	nop;
{++++++++++++++++++++++++}

	jump KKK1;

ErrorInt:
	rts;
	ar=dm(InRig);
	ar=PASS ar;
	if EQ rts;

	ax0=0xFFFF;
	ar = ax0 xor ay0;
	dm(Write68)=ar;
	dm(Interupt68)=ar;
	rts; 
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}

{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
GainUp:
	ax0=DM(VFsave);
	ay1=0x01F0;
	af=ax0 and ay1;

	ar=0x0600;
	ar=ar+af;

	DM(VolFreq)=ar;
	jump GainToggle;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
GainDown:
	ax0=DM(VFsave);
	ay1=0x01F0;
	af=ax0 and ay1;

	ar=0x0200;
	ar=ar+af;

	DM(VolFreq)=ar;
	jump GainToggle;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
GainToggle:
	cntr=ax1;
	ay1=0x0200;

	do Togg until CE;
		ar=ar-ay1;
		DM(VolFreq)=ar;
		cntr=7;
		do Gwait1 until CE;
Gwait1:		nop;

		ar=ar+ay1;
		DM(VolFreq)=ar;
		cntr=27;
		do Gwait2 until CE;
Gwait2:		nop;

Togg:		nop;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
UpdateLeds:
	dm(Write68)=ar;
	dm(Interupt68)=ar;

	rts;

	ay0=IntStat;
ll1:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump ll1;

	ay0=WRITEOK;
ll2:	ar=DM(Status);
	ar=ar and ay0;
	if NE jump ll2;

	rts;

{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
writeAY0:
	ay1=WRITEOK;
BLp2:	ar=DM(Status);
	ar=ar and ay1;
	if EQ jump BLp2;

	DM(Write68)=ay0;
	rts;
readAY0:
	ay0=READOK;
BLp:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump BLp;

	ay0=DM(Read68);
	rts;


IM:	NOP;
InteruptMonitor:
	i7=^IM;
	ar=pm(i7,m4);
	i7=^SMC;		{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	rts;

IB:	call Beep;
InteruptBeep:
	i7=^IB;
	ar=pm(i7,m4);
	i7=^SMC;		{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	rts;

.endmod;
