{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				MONITOR	15			 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
.module/ram/abs=0/boot=0		Monitor;
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
.const		Srate=100;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		EVhilo	=	0x0004;
.const		EVsmpt	=	0x0002;
.const		EVnewcode=	0x0008;

.const		EVplay	=	0x0010;
.const		EVmon	=	0x0080;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		SampRate=0x3FF1;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		WRITEOK=0x0002;
.const		READOK=0x0001;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		MsgCh0=0x0021;
.const		MsgCh1=0x0031;
.const		MsgCh2=0x0041;
.const		MsgCh3=0x0051;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		MsgHiLo=0x0001;
.const		MsgHiOUT=0x0011;
.const		MsgLoOUT=0x0017;
.const		MsgLoIN=0x0007;
.const		MsgRecData=0x0009;
.const		MsgPlayData=0x000F;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		BLOCKSIZE=0x0200;
.const		BUFFERSIZE=0x0800;
.const		NUMBUFFERS=0x0004;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram/circ buffer[0x2000];
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	VFsave;
.var/dm/ram	Vol;
.var/dm/ram	Vol0;
.var/dm/ram	Vol1;
.var/dm/ram	Vol2;
.var/dm/ram	Vol3;
.var/dm/ram	VolM;
.var/dm/ram	VolMon;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	ChanHL0;
.var/dm/ram	ChanHL1;
.var/dm/ram	ChanHL2;
.var/dm/ram	ChanHL3;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	VAR0;
.var/dm/ram	VAR1;
.var/dm/ram	VAR2;
.var/dm/ram	VAR3;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	s0;
.var/dm/ram	s1;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	LoOUT;
.var/dm/ram	HiOUT;
.var/dm/ram	LoIN;
.var/dm/ram	HiIN;
.var/dm/ram	LoCHAN1;
.var/dm/ram	HiCHAN1;
.var/dm/ram	LoCHAN2;
.var/dm/ram	HiCHAN2;
.var/dm/ram	LoCHAN3;
.var/dm/ram	HiCHAN3;
.var/dm/ram	LoCHAN4;
.var/dm/ram	HiCHAN4;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	BuffCntr;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	FLAG;
.var/dm/ram	Mode;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	Output;
.var/dm/ram	Input;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	Save1;
.var/dm/ram	Save2;
.var/dm/ram	Save3;
.var/dm/ram	Save4;
.var/dm/ram	Save5;
.var/dm/ram	Save6;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	Channel;
.var/dm/ram	EVflag;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	OldBit;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	BuffersNeeded;
.var/dm/ram	ReceiveNeeded;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		SMPTEsyncword=0xBFFC;	{0011 1111 1111 1101}
.var/dm/ram	SMPTEmode;	{bak}	{1011 1111 1111 1100}
.var/dm/ram	SMPTEdata;
.var/dm/ram	SMPTEctr;
.var/dm/ram	SMPTEsecframe;
.var/dm/ram	SMPTEhourmin;
.var/dm/ram	OLDhourmin;
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX			INTERUPT VECTORS			 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
Begin:	jump start;	rti;		rti;		rti;	{Reset Vector}
	ENA SEC_REG;	call HandleInt;	DIS SEC_REG;	rti;	{irq2 A2D}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	ENA SEC_REG;	call Command; 	DIS SEC_REG;	rti;	{irq1 I68000}
	ENA SEC_REG;	call Smpte;	DIS SEC_REG;	rti;	{irq0 SMPTE}
	ENA SEC_REG;	call HiLow;	DIS SEC_REG;	rti;	{timer}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX			Start Execution				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
start:	call CntlReg_inits;	{ set up SPORTS, TIMER, etc }

	ar=0x0F22;
	DM(VolFreq)=ar;		{ Set Volume and Cutoff Frequency }
	DM(VFsave)=ar;		{ Set Volume and Cutoff Frequency }
	ax1=100;
	call GainToggle;

	ar=1;
	DM(SMPTEmode)=ar;
	DM(SMPTEctr)=ar;

	ar=DM(Read68);

	Call ModeOff;

	ar=0;
	DM(BuffCntr)=ar;
	DM(FLAG)=ar;

	DM(VolM)=ar;
	ar=0x7FFF;
	DM(Vol0)=ar;
	DM(Vol1)=ar;
	DM(Vol2)=ar;
	DM(Vol3)=ar;
	DM(VolMon)=ar;
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}

{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
lop:	ar=DM(Mode);
	ar=PASS ar;
	if NE jump MainEvents;
	imask=b#000000;
{ннннннннннннннннннннннннннннннн}
lopOFF:	ay0=READOK;
BSLoop:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump BSLoop;

	ar=DM(Read68);
	ay0=0x003f;
	af=ar-ay0;
	if EQ jump NewCode;
	af=af+1;
	if NE jump lopOFF;
{ннннннннннннннннннннннннннннннн}
ModeOn:	ar=1;
	DM(Mode)=ar;
	ar=0;
	DM(BuffCntr)=ar;
	IFC=b#000000111111;
	imask=b#100111;
	ENA TIMER;
	jump lop;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
NewCode:i7=0;m7=1;l7=0;
	CNTR=0x03F3;
	jump LoadCode;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Hplay:	ay1=0;
	ar=DM(BuffCntr);
	ar=PASS ar;
	if EQ rts;

	imask=b#000111;		{ disable play interupt, so buffCntr does not change}
	ay0=DM(BuffCntr);
	ar=ay0-1;
	DM(BuffCntr)=ar;
	imask=b#100111;

RI:	nop;

	ar=MsgPlayData;
	jump SendW;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{HRsmp:	ay0=EVsmpR;
	ax0=DM(FLAG);
	ar=ax0 and ay0;
	if EQ rts;

	ar=ax0 xor ay0;
	DM(FLAG)=ar;

	ay0=0x0004;
	call EbitOFF;
	jump DoSmpte;}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Hhilo:	call HLchanVAL;

	ay0=EVhilo;
	ax0=DM(FLAG);
	ar=ax0 and ay0;
	if EQ rts;

	ar=ax0 xor ay0;
	DM(FLAG)=ar;

	ar=MsgHiLo;
	call SendW;

	ay0=0xFF00;

	i6=^LoOUT;
	l6=0;
	ax0=0;
	cntr=6;
	ay1=WRITEOK;

	do HLoop UNTIL CE;
	ar=DM(I6,m4);	{ Low Info }
	DM(I6,m5)=ax0;
	sr=LSHIFT ar by -8 (LO);
	ar=DM(I6,m4);	{High Info}
	DM(I6,m5)=ax0	,	af=ar and ay0;
	ar=sr0 or af;


HLo1p:	ax1=DM(Status);
	af=ax1 and ay1;
	if EQ jump HLo1p;

	nop;
	nop;

	DM(Write68)=ar;

{HLo2p:	ax1=DM(Status);
	af=ax1 and ay1;
	if EQ jump HLo2p;}

HLoop:	NOP;

	rts;
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
HLchanVAL:
	I6=I4;
	l6=m6;
	ar=DM(I6,m7);
	l6=0;

	ay0=DM(I6,M6);
	ax1=DM(HiCHAN1);
	ax0=DM(LoCHAN1);
	call	HIDLOWS;
	DM(LoCHAN1)=ax0;
	DM(HiCHAN1)=ax1;

	ay0=DM(I6,M6);
	ax1=DM(HiCHAN2);
	ax0=DM(LoCHAN2);
	call	HIDLOWS;
	DM(LoCHAN2)=ax0;
	DM(HiCHAN2)=ax1;

	ay0=DM(I6,M6);
	ax1=DM(HiCHAN3);
	ax0=DM(LoCHAN3);
	call	HIDLOWS;
	DM(LoCHAN3)=ax0;
	DM(HiCHAN3)=ax1;

	ay0=DM(I6,M6);
	ax1=DM(HiCHAN4);
	ax0=DM(LoCHAN4);
	call	HIDLOWS;
	DM(LoCHAN4)=ax0;
	DM(HiCHAN4)=ax1;
	rts;
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
HIDLOWS:
	ar=0x8000;
	ar=ar and ay0;
	if NE jump Negi;

	ar=ax1-ay0;
	if GT rts;
	ax1=ay0;
	rts;

Negi:	ar=ax0-ay0;
	if LT rts;
	ax0=ay0;
	rts;
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}


{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/			Get Command from 68000			/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
Command:ay0=0x0001;
	ax0=DM(Read68);
	ar=ax0 and ay0;
	if EQ jump PlayData;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
	si=ax0;
	sr=ASHIFT si by 8 (LO);
	ay1=0x00FF;		{UNNESSECARY?}
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
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}

JumpTable:
	rts;			{0xVV01}
	jump SetFreq;		{0xbF03}
	jump SetSamplingRate;	{0x__05  Rate=VAR0}
	jump SetTimer;		{0x__07}
	jump GainUp;		{0x__09  Period=VAR0  Scale=VAR1}
	jump GainDown;		{0x__0B}
	rts;			{0x__0D}
	jump SetCVol;		{0x_C0f  value = var0} 

	jump DelayOn;			{0x__11}
	jump DelayOff;			{0x__13}
	jump EbitOFF;		{0xEV15}
	jump EbitON;		{0xEV17}
	jump DelayBuff;			{0x__19}
	rts;			{0x__1B} 
	jump MonitorOff;	{0x__1D}
	jump MonitorOn;		{0x__1F}

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
	jump RecordOff;		{0x__35}
	rts;			{0x__37}
	rts;			{0x__39}
	jump SMPTEreq;		{0x__3B}
	jump ModeOff;		{0x__3D}
	rts;			{0x__3F}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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
	DM(VolM)=ax1;
	DM(VolMon)=ax1;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SMPTEreq:
	ay0=0xFFFF;
	DM(OLDhourmin)=ay0;	{ make shure hours/mins updated}

	ay0=0xFFFd;
	ax0=DM(FLAG);
	ar=ax0 and ay0;		{clear smpte flags}
	DM(FLAG)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
GainUp:
	ax0=DM(VFsave);
	ay1=0xFF0F;
	af=ax0 and ay1;

	ar=0x0060;
	ar=ar+af;

	DM(VolFreq)=ar;
	jump GainToggle;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
GainDown:
	ax0=DM(VFsave);
	ay1=0xFF0F;
	af=ax0 and ay1;

	ar=0x0020;
	ar=ar+af;

	DM(VolFreq)=ar;
	jump GainToggle;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
GainToggle:
	cntr=ax1;
	ay1=0x0020;

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
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SetSamplingRate:

	ar=DM(VAR0);
	ay0=30;
	ar=ar-ay0;		{Boundry test}
	if LT rts;

	ar=DM(VAR0);
	DM(SampRate)=ar;

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SetTimer:
	ar=DM(VAR0);
	DM(0x3ffd)=ar;
	ar=DM(VAR1);
	DM(0x3ffb)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ModeOff:ar=0;
	DM(Mode)=ar;
	jump RecordOff;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SetLo:	ar=DM(I7,m4);
	ay1=0xFF00;
	af=ar and ay1;

	ar=ax1 or af;
	DM(I7,m4)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SetHi:	ar=DM(I7,m4);

	si=ax1;
	sr= LSHIFT si by 8 (LO);

	ay1=0x00FF;
	af=ar and ay1;

	ar=sr0 or af;
	DM(I7,m4)=ar;
	rts;
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				HILOW				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
HiLow:
	ar=DM(FLAG);
	ay0=EVhilo;
	ar=ar or ay0;
	DM(FLAG)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
HiLoSave:				{ ar = value}
{ Save Highest/lowest byte }

	ay0=mr1;

	ax0=0x8000;
	ar=ax0 and ay0;
	if NE jump Negitive;

	ax0=DM(HiOUT);
	ar=ax0-ay0;
	if GT rts;
	DM(HiOUT)=ay0;
	rts;

Negitive:
	ax0=DM(LoOUT);
	ar=ax0-ay0;
	if LT rts;
	DM(LoOUT)=ay0;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
HLin:				{ ar = value}
{ Save Highest/lowest byte }
	ay0=sr0;

	ax0=0x8000;
	ar=ax0 and ay0;
	if NE jump Negie;

	ax0=DM(HiIN);
	ar=ax0-ay0;
	if GT rts;
	DM(HiIN)=ay0;
	rts;

Negie:
	ax0=DM(LoIN);
	ar=ax0-ay0;
	if LT rts;
	DM(LoIN)=ay0;
	rts;
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				SMPTE				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
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
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/				Misc				/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
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

{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SendW:
	DM(Save5)=ay0;
	DM(Save6)=ar;

	ay0=WRITEOK;
WLoo1p:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump WLoo1p;

	ar=DM(Save6);
	nop;
	nop;
	DM(Write68)=ar;			{ xxxx xxxx  xxxx xxxx }

	DM(Interupt68)=ar;

{	ay0=WRITEOK;
WLoop:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump WLoop;
}
	ay0=DM(Save5);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				MONITOR				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
MonInt:	ar=DM(Input);
	my0=DM(VolM);
	MR=MR+ar*my0 (SU);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
MonitorOn:
	ar=DM(VolMon);
	DM(VolM)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
MonitorOff:
	ar=DM(VolM);
	DM(VolMon)=ar;
	ar=0;			{ FIX ME }
	DM(VolM)=ar;
	rts;
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				RECORD				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
RecInt:	ar=DM(Input);
	DM(I5,M6)=ar;
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
	ar=MsgRecData;
	Call SendW;

	ay1=0;
	ay0=WRITEOK;

	ar=BlockSize;
	CNTR=ar;

	DO SendBs UNTIL CE;

	ax1=DM(I3,m0);			{ Get word }

RWLop:	ax0=DM(Status);
	ar=ax0 and ay0;			{ Wait for flag }
	if EQ jump RWLop;
	nop;
	nop;

	DM(Write68)=ax1;

{RW1Lop:	ax0=DM(Status);
	ar=ax0 and ay0;	
	if EQ jump RW1Lop;
}
SendBs:DM(I3,m1)=ay1;			{ Zero word  , add 1}

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOff:
	ar=0;px=0;
	i7=^PlayNOP;		{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	i7=^RI;		{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOn:
	ar=I4;			{ ADD PRE DELAY FOR SAMPLE }
	ay0=BUFFERSIZE*3;
	ar=ar+ay0;
	I3=ar;

	i7=^RecordJump;
	ar=PM(i7,m4);
	i7=^PlayNOP;		{ I HATE writting self modifying code }
	PM(i7,m4)=ar;

	i7=^HRScall;
	ar=PM(i7,m4);
	i7=^RI;			{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordJump:jump RecInt;
HRScall:   Call HrecordStart;
HRcall:    Call Hrecord;

{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				PLAY				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
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

PlayNOP:nop;	{ or jump RecInt }
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Play4Int:
	ar=DM(I5,M4);
	my0=DM(i7,m5);
	DM(I5,M6)=ay0		,	MR=MR+ar*my0 (SU);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				Events				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
EbitON:	ar=ax1;
EventON:sr1=ar;
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
	i7=ar;
	ar=PM(i7,m7);
	PM(i7,m7)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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

	ar=^MainEvents;
	ar=ar+af;
	i7=ar;
	ar=0;
	px=0;
	PM(i7,m7)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
MainEventsTable:
	nop;
	Call Hsmpte;
	Call Hhilo;
	JUMP NewCode;

	Call Hplay;
	nop;
	nop;
	nop;	{MONITOR}
MainEvents:
	nop;
	nop;
	nop;
	nop;

	nop;	{ Play }		{Must be in this order}
	nop;
	nop;
	nop;
	jump lop;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
HandleInt:
	mr=0;			{ Clear Mult (output) }

	ar=DM(ADC);
	sr=LSHIFT ar by 4 (lo);		{ Get Byte from Sampler }

DI:	nop;			{delay patch}

	DM(Input)=sr0;
	Call	HLin;

	Call PlayInt;
	Call MonInt;

	if mv sat mr;
	call HiLoSave;

	sr=ASHIFT mr1 by -4 (hi);
	DM(DAC)=sr1;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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

	ar=DM(ADC);
	sr=LSHIFT ar by 4 (lo);
DelayInt:
	Call	HLin;
	si=sr0;

	MY0=DM(VAR1);		{VOL1  Input}
	MR=SR0 * MY0 (SU);
 
	ar=DM(i0,m0);
	MY0=DM(VAR2);		{VOL2  Old Delay}
	MR=MR+AR*MY0 (SU);
	if mv sat mr;

	call HiLoSave;

	sr=ASHIFT mr1 by -4 (hi);
	DM(DAC)=sr1;

	ay0=si;

	ar=DM(i0,m0);
	sR=ASHIFT ar by -1 (HI);
	MY0=DM(VAR3);		{VOL3 Feedback}
	MR=AR * MY0 (SU);

	ENA AR_SAT;
	ar=mr1+ay0;
	DIS AR_SAT;

	DM(i0,m1)=ar;

	if mv sat mr;

	rts;
{////////////////////////////////////////////////////////////////////}
DelayOn:
	i7=^DEcall;
	ar=PM(i7,m4);
	i7=^DI;			{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	dm(s0)=i0;
	dm(s1)=l0;
	rts;
DelayOff:
	ar=0;px=0;
	i7=^DI;			{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	i0=dm(s0);
	l0=dm(s1);
	rts;
DelayBuff:
	l0=DM(VAR0);
rts;

DEcall: Jump DelayInt;

.endmod;
