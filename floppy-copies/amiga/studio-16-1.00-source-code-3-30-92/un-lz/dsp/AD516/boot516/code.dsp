{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{/////				Boot Epprom			/////}
{/////				for AD0516			/////}
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
.module/ram/abs=0/boot=0		Monitor;
.const		Srate=60;
.port		VolFreq;
.port		Read68;
.port		Write68;
.port		Status;
.port		Interupt68;
.port		ModeSel;
.port 		Rate;
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
{           bits 12-22     unused}
{           bits 23         Divide Ratio 0=(NO plus one in denominator)}
{++++++++++++++++++++++++}
.init Sware: 0xEEEEEE;
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
	nop;	 	nop;		nop;		rti;	{irq2 A2D}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}

{////////////////////////////////////////////////////////////////////}
{/////			Start Execution				/////}
{////////////////////////////////////////////////////////////////////}
start:	call CntlReg_inits;	{ set up SPORTS, TIMER, etc }
	ar=DM(Read68);
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
lop:	{ar=0;
	DM(Rate)=ar;
	DM(Write68)=ar;
	nop;

	ar=0xFFFF;
	DM(Rate)=ar;
	DM(Write68)=ar;
	jump lop;}

	ay0=READOK;
BSLoop:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump BSLoop;

	ar=DM(Read68);

	ay0=0x0043;
	af=ar-ay0;
	if EQ jump Poke;

	ay0=0x0041;
	af=ar-ay0;
	if EQ jump Peek;

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
Poke:	Wait(READOK);
	ar=DM(Read68);
	i7=ar;
	Wait(READOK);
	ar=DM(Read68);
	DM(i7,m7)=ar;
	jump lop;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Peek:	Wait(READOK);
	ar=DM(Read68);
	i7=ar;
	Wait(WRITEOK);
	ar=DM(i7,m7);
	DM(Write68)=ar;
	jump lop;
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
	L7=0;
	M7=0;
	rts;
{////////////////////////////////////////////////////////////////////}
{/////								/////}
{////////////////////////////////////////////////////////////////////}
.endmod;
