{**********************************************************************
Copyright (C) 1992 SunRize Industries
	Written by Todd Modjeski
**********************************************************************}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{/////			Demo DSP Code				/////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.module/ram/abs=0/boot=0		Demo;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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
.var/pm/ram/abs=0x03f0 Sware;
.var/pm/ram/abs=0x03f1 EprmRev;
.var/pm/ram/abs=0x03f2 BoardRev;
.var/pm/ram/abs=0x03f3 Flags;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.init Sware: 0x010014;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		READOK=0x0001;
.const		WRITEOK=0x0002;
.const		IntStat=0x0004;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram/abs=0x3800 SampIn;
.var/dm/ram/abs=0x3801 SampOut;
.var/dm/ram/abs=0x3802 Check;
.var/dm/ram	VFsave;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/     Macros     \/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
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
{/\/\/\/\/\/\/\/\/\/\/\/Interupt Vectors\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Begin:	nop;		nop;		nop;		jump start;	{Reset Vector}
	ENA SEC_REG;	call SampInt;	DIS SEC_REG;	rti;	{irq2 A2D}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/Sample Interupt/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SampInt:
	si=DM(ADC);
	sr=LSHIFT si by 4 (lo);
	DM(SampIn)=sr0;
	
	si=DM(SampOut);
	sr=ASHIFT si by -4 (hi);
	DM(DAC)=sr1;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/Start Execution/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
start:	call CntlReg_inits;	{ set up SPORTS, TIMER, etc }
	ar=DM(Read68);

	ar=0x091F;
	DM(VolFreq)=ar;		{ Set Volume and Cutoff Frequency }
	DM(VFsave)=ar;		{ Set Volume and Cutoff Frequency }

	ar=0x1234;
	DM(Check)=ar;

	ax1=100;call GainDown;
	ax1=50;	call GainUp;	{ Set Gain to 50% }

{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{////////////////////////////////////////////////////////////////////}
{/////			     Main Loop				/////}
{////////////////////////////////////////////////////////////////////}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
lop:
	Wait(READOK);
	ar=DM(Read68);

	ay0=0x0043; af=ar-ay0; if NE jump Next1;
	Call Poke; jump lop;
Next1:
	ay0=0x0041; af=ar-ay0; if NE jump Next2;
	Call Peek; jump lop;
Next2:
	ay0=0x0109; af=ar-ay0; if NE jump Next3;
	ax1=1; Call GainUp; jump lop;
Next3:
	ay0=0x010b; af=ar-ay0; if NE jump Next4;
	ax1=1; Call GainDown; jump lop;
Next4:
	ay0=0x003d; af=ar-ay0; if NE jump Next5;
	Call SendRev; jump lop;
Next5:
	ay0=0x003f; af=ar-ay0; if EQ jump NewCode;
	jump lop;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
CntlReg_inits:
	   L0=0;
	   M0=1;
	   I0=0x3FEF;		{point to last DM-mapped control registers }
 { 0x3FEF } DM(I0,M0)=0x0000;	{SPORT1 AUTOBUFF DISABLED}
 { 0x3FF0 } DM(I0,M0)=0x0000;	{SPORT1 RFSDIV NOT USED}
 { 0x3FF1 } DM(I0,M0)=55;	{SPORT1 SCLKDIV   (SAMPLING RATE)}
 { 0x3FF2 } DM(I0,M0)=0x4000;	{SPORT1 Control Register}
	   I0=0x3FFB;
 { 0x3FFB } DM(I0,M0)=0x0040;	{TIMER TSCALE Scaling Register}
 { 0x3FFC } DM(I0,M0)=0x0001;	{TIMER TCOUNT Counting Register}
 { 0x3FFD } DM(I0,M0)=0x0C00;	{TIMER TPERIOD Period Register}
 { 0x3FFE } DM(I0,M0)=0x1249;	{DM WAIT STATES one for each}
 { 0x3FFF } DM(I0,M0)=0x1018;	{SPORT0 ENABLED, BOOT PAGE 0}
	icntl=b#00111;
	imask=b#100000;		{ Start ADC + DAC (Turn On Interupt)}
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
NewCode:ar=0;
	DM(Check)=ar;

	imask=b#000000;
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
SendRev:
	ay1=WRITEOK;
	cntr=4;
	i7=0x03f0;

	do SPLoop UNTIL CE;
		ar=PM(I7,m5);

		Wait(WRITEOK);
		DM(Write68)=ar;

		Wait(WRITEOK);
SPLoop:		DM(Write68)=px;

	rts;
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
Poke:	Wait(READOK);
	ar=DM(Read68);
	i7=ar;
	Wait(READOK);
	ar=DM(Read68);
	DM(i7,m7)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Peek:	Wait(READOK);
	ar=DM(Read68);
	i7=ar;
	Wait(WRITEOK);
	ar=DM(i7,m7);
	DM(Write68)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.endmod;
