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

.var/circ	CtrlIn[4];
.var/circ	CtrlOut[4];
.var/circ	DataIn[4];
.var/circ	DataOut[8];
.var		FirstFlg;
.var		DCBFlg;
.var		DmodeFlg;
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
	jump setupcontrol;rti;		rti;		rti;	{NA}
	jump newdata;	rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}

{////////////////////////////////////////////////////////////////////}
{/////			Start Execution				/////}
{////////////////////////////////////////////////////////////////////}
start:	call CntlReg_inits;	{ set up SPORTS, TIMER, etc }
xxx:	jump xxx;
	ar=DM(Read68);
{////////////////////////////////////////////////////////////////////}
{////////////////////////////////////////////////////////////////////}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
lop:	ay0=READOK;
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
{first init 16 bit codac...}
	ax0=0x1249;		{one wait state}
	DM(0x3ffe)=ax0;
	ax0=1024;		{ 44.1K samp rate w/ 1.8432MHZ clk }
	DM(Rate)=ax0;		{ CLKIN=900*Rate @1.8432MHZ}
	ax0=0;
	DM(ModeSel)=ax0;
	L0=%CtrlIn;
	M0=1;
	I0=^CtrlIn;
	L1=%CtrlOut;
	M1=1;
	I1=^CtrlOut;
	ax0=1;
	DM(FirstFlg)=ax0;
	DM(DCBFlg)=ax0;
	ax0=0;
	DM(DmodeFlg)=ax0;
	ay0=B#0010000100011100;  {DCB=0,AC=0,DFR=3,ST=1,DF=001}
{Intialize control mode output buffer}
	DM(I1,M1)=ay0;
	DM(I1,M1)=B#0010001000000000; {MCK=2,FSEL=0,MS=1,TXDIS=0,ENL=0,ADL=0}
	DM(I1,M1)=B#0000000000000000; {pio=0}
	DM(I1,M1)=B#0000000000000000; {REVID=NC}
	L2=0;
	I2=0x3fef;
{3fef}	DM(I2,M1)=0x0283;	{aport1 autobuffer cntrl reg}
{3ff0}	DM(I2,M1)=383;		{rfsdiv0}
{3ff1}	DM(I2,M1)=849;		{sclkdiv0}
{3ff2}	DM(I2,M1)=B#0100000100011111; {sport1 cntr reg: internal sclk&rfs}
					{normal framing mode, 	}
					{frame sync not inverted}
					{16 bit word len}
	I2=0x3ffb;
{3ffb}	DM(I2,M1)=0x0040;	{TIMER TSCALE Scaling Registe
{3ffc}	DM(I2,M1)=0x0001;	{TIMER TCOUNT Counting Regist
{3ffd}	DM(I2,M1)=0x0C00;	{TIMER TPERIOD Period Registe
{3ffe}	DM(I2,M1)=0x1249;	{DM WAIT STATES one for each}{REDUNDANT!}
	ICNTL=0;
	IMASK=B#000100;		{only sport transmit int enabled}		
	ax0=DM(I1,M1);		{send first 16 bits of ctrl word}
	TX1=ax0;
{3fff}	DM(I2,M1)=0x0c18;	{system cntrl reg: sport 1 enabled}

 {old 0x3FFF }{ DM(I0,M0)=0x1018;}{SPORT0 ENABLED, BOOT PAGE 0}

{	icntl=b#00111;
	IFC=b#000000111111;}

wait1:	ax1=DM(DmodeFlg);
	ar=PASS ax1;
	if GT jump GO_DMODE;
	jump wait1;
GO_DMODE:
	ar=DM(Status); {for debugging}
	L0=%DataIn;
	I0=^DataIn;
	L1=%DataOut;
	I1=^DataOut;	
	M2=3;
	L3=L1;
	ax0=DM(I1,M1);	{send first 16 bits of data}
	TX1=ax0;
	ax0=0xc18;	{turn on sport 1}
	DM(0x3FFF)=ax0;
	IFC=B#000000111111;	{clear all pending ints}
	IMASK=B#000010;		{rx interrupts on}

{ system should now be passing A/D to D/A in stereo }

	L7=0;
	M7=0;
	rts;
{////////////////////////////////////////////////////////////////////}
{ sport1 tx interrupt routine 					     }
{////////////////////////////////////////////////////////////////////}
SETUPCONTROL:
	ax0=DM(FirstFlg);
	af=PASS ax0;
	if NE jump DECR_FIRST;
	ax0=DM(DCBFlg);
	ar=PASS ax0;
	if EQ Jump DCBFLG_SET;
	ax0=DM(CtrlIn);			{DCB flag has not been set yet}
	ar=ax0 XOR ay0;			{same as we sent out?}
	if EQ jump SET_DCB;		{set flag if DCB was 0}
	rti;
DCBFLG_SET:
	ax0=DM(CtrlIn);			{DCBFlag was set}
	ar=ax0 AND ay0;			{only check for DCB bit}
	if NE jump SETMODE;		{if DCB=1 ready for datamode}
	rti;
SET_DCB:
	ax0=0;
	DM(DCBFlg)=ax0;
	ax0=B#0010010100011100;
	DM(CtrlOut)=ax0;
	ay0=B#0000010000000000;
	rti;
DECR_FIRST:
	ax0=0;
	DM(FirstFlg)=ax0;
	rti;
SETMODE:
	IMASK=0;
	ax0=0x0418;			{disable sport1}
	DM(0x3FFF)=ax0;

	I1=^DataOut;
	L1=0;
	DM(I1,M1)=0x0000;		{reset output & input data buffs}
	DM(I1,M1)=0x0000;
	DM(I1,M1)=B#0100000000000000;	{OM0=1,OM1=1,L0=0,SM=0,R0=0}
{	DM(I1,M1)=B#1100000000000000;}	{PI0=11,OVR=0,IS=0,LG=0,MA=15,RG=0}
	DM(I1,M1)=B#1100000011110000;	{PI0=11,OVR=0,IS=0,LG=0,MA=15,RG=0}

	DM(I1,M1)=0x0000;		{copy of above}
	DM(I1,M1)=0x0000;
	DM(I1,M1)=B#0100000000000000;	{OM0=1,OM1=1,L0=0,SM=0,R0=0}
{	DM(I1,M1)=B#1100000000000000;}	{PI0=11,OVR=0,IS=0,LG=0,MA=15,RG=0}
	DM(I1,M1)=B#1100000011110000;	{PI0=11,OVR=0,IS=0,LG=0,MA=15,RG=0}

	ax0=0x001f;
	DM(0x3ff2)=ax0;	{sport 1 cntrl: internal tfs, external sclk & rfs}
	ax0=1;
	DM(ModeSel)=ax0;	{set D/C high}
	DM(DmodeFlg)=ax0;
	rti;
{////////////////////////////////////////////////////////////////////}
{rx int handler}
{////////////////////////////////////////////////////////////////////}
NEWDATA:



	I3=I1;			{copy datain to data out}
	modify(I3,M2);
	ax0=DM(DataIn);
	DM(I3,M1)=ax0;
	ax0=DM(DataIn+1);
	DM(I3,M1)=ax0;
	
	modify(I3,M1);		{skip DataOut+2}
	ax0=DM(DataIn+3);	{if PIO1 is low, pulse status}
	ay0=0x8000;		{and copy to PIO0}
	ar=ax0 AND ay0;
	if NE jump zzz;
	ar=DM(Status); {for debugging}
	ay0=0xBFFF;
	ar=ax0 AND ay0;{clr bit}
	ay0=0x8000;
	ar=ar OR ay0;{keep PIO1 high (tri-state) bit}
	DM(I3,M1)=ar;
	rti;

zzz:
	ay0=0x4000;
	ar=ax0 OR ay0;{set bit}
	DM(I3,M1)=ar;
	rti;

{/////								/////}
{////////////////////////////////////////////////////////////////////}
.endmod;
