{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				MONITOR	V1.04			 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
.module/ram/abs=0/boot=0		Monitor;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/pm/ram/abs=0x03f0 Sware;
.init Sware: 0x010104;		{RRRRTT     R=Software Rev   T=SoftWare Type}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.port		ADC;
.port		DAC;
.port		VolFreq;
.port		Read68;
.port		Write68;
.port		Status;
.port		Interupt68;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		WRITEOK=0x0002;
.const		READOK=0x0001;
.const		INTSTAT=0x0004;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.global		Status	,Read68;
.external	LoadCode,WaitRead;
.entry		Begin;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		Srate=100;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		SampRate=0x3FF1;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		MsgRecData=0x0009;
.const		MsgPlayData=0x000F;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.const		BLOCKSIZE=0x0400;
.const		BUFFERSIZE=0x1800;
.const		NUMBUFFERS=0x0006;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram/circ/seg=extern_dm buffer[BUFFERSIZE];
.var/dm/ram/circ/seg=extern_dm InPointer;
.var/dm/ram/circ/seg=extern_dm DelayTime;
.var/dm/ram/circ/seg=extern_dm DelayFeedback;
.var/dm/ram/circ/seg=extern_dm Volume;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram/circ Stack[100];
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	HL_TEST;
.const		HILO_AMIGA_MSG=0x0001;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	HiLo_Input;
.var/dm/ram	HiLo_Chan1;
.var/dm/ram	HiLo_Chan2;
.var/dm/ram	HiLo_Chan3;
.var/dm/ram	HiLo_Chan4;
.var/dm/ram	HiLo_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	Vol_Input;
.var/dm/ram	Vol_Chan1;
.var/dm/ram	Vol_Chan2;
.var/dm/ram	Vol_Chan3;
.var/dm/ram	Vol_Chan4;
.var/dm/ram	Vol_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	Val_Input;
.var/dm/ram	Val_Chan1;
.var/dm/ram	Val_Chan2;
.var/dm/ram	Val_Chan3;
.var/dm/ram	Val_Chan4;
.var/dm/ram	Val_Output;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	BuffCntr;
.var/dm/ram	FLAG;
.var/dm/ram	OldBit;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	SM_TEST;
.const		SMPTEsyncword=0xBFFC;	{0011 1111 1111 1101}
.var/dm/ram	SMPTEmode;	{bak}	{1011 1111 1111 1100}
.var/dm/ram	SMPTEdata;

.var/dm/ram	SMPTEframe;
.var/dm/ram	SMPTEsec;
.var/dm/ram	SMPTEmin;

.var/dm/ram	SAVEframe;
.var/dm/ram	SAVEsec;
.var/dm/ram	SAVEmin;
.var/dm/ram	SAVEhour;

.var/dm/ram	SMPTEctr;

.var/dm/ram	SMPTEsecframe;
.var/dm/ram	SMPTEhourmin;
.var/dm/ram	OLDhourmin;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	SAMPLCNTR;

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
.var/dm/ram BANKsave;
.MACRO ChangeBank(%0);
	ar=%0;
	af=PASS ar;
	ar=0x000f; ar=ar and af;DM(BANKsave)=ar;af=PASS ar;
	ar=DM(FREQsave);
	af=ar or af;
	ar=0x0200;
	ar=ar or af;
	DM(VolFreq)=ar;
.ENDMACRO;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram FREQsave;
.MACRO ChangeFreq(%0);
	ar=%0;
	af=PASS ar;
	ar=0x01f0; ar=ar and af;DM(FREQsave)=ar;af=PASS ar;
	ar=DM(BANKsave);
	af=ar or af;
	ar=0x0200;
	ar=ar or af;
	DM(VolFreq)=ar;
.ENDMACRO;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.MACRO ReturnBF(%0);
	ar=DM(FREQsave);
	af=PASS ar;
	ar=DM(BANKsave);
	ar=ar or af;
	%0=ar;
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
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX			INTERUPT VECTORS			 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Begin:	DIS SEC_REG;	ENA AR_SAT;	jump start;	rti;	{Reset Vector}
	ENA SEC_REG;	call HandleInt;	DIS SEC_REG;	rti;	{irq2 A2D}
	rti;		rti;		rti;		rti;	{NA}
	rti;		rti;		rti;		rti;	{NA}
	rti;		nop;		nop;		rti;	{irq1 I68000}
SMSMPTE:nop;		call Smpte;	rti;		rti;	{irq0 SMPTE}
	rti;		rti;		rti;		rti;	{timer}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX			Start Execution				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
start:	call CntlReg_inits;	{ set up SPORTS, TIMER, etc }
	DIS TIMER;

	ChangeFreq(0x0110);
	ChangeBank(0);

{	ax1=100;call GainDown;}

	ar=1;
	DM(SMPTEmode)=ar;
	DM(SMPTEctr)=ar;

	ar=DM(Read68);

	Call ModeOff;

	ar=0;
	DM(BuffCntr)=ar;
	DM(FLAG)=ar;

	DM(Vol_Input)=ar;
	ar=0x8000;
	DM(Vol_Chan1)=ar;
	DM(Vol_Chan2)=ar;
	DM(Vol_Chan3)=ar;
	DM(Vol_Chan4)=ar;
	DM(Vol_Output)=ar;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
	IFC=b#000000111111;
	imask=b#000000;
Main_Loop:

SMSM:	nop;	{Call Hsmpte}
PBSM:	nop;	{Call Hplay - Play block completed}

	ay0=DM(Status);ar=READOK;
	ar=ar and ay0;
	if EQ jump Skip22;
	call Command;	{READ DATA}
	jump Main_Loop;

Skip22:	call HLchanVAL;
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
Command:ay0=0x0001;
	ax0=DM(Read68);
	ar=ax0 and ay0;
	if EQ jump PlayData;
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

JumpTable:
	rts;			{0x__01}
	jump SetFreq;		{0xbF03}
	jump SetSamplingRate;	{0x__05}
	jump SetTimer;		{0x__07}
	jump GainUp;		{0x__09}
	jump GainDown;		{0x__0B}
	jump RecordSend;	{0x__0D}
	rts;			{0x_C0f}

	rts;			{0x__11}
	jump SC_STATE;		{0x__13}
	jump PB_STATE;		{0x__15}
	jump SM_STATE;		{0x__17}
	jump MODE;		{0x__19}
	rts;			{0x__1B} 
	jump ChanVol;		{0x__1D}
	jump HL_SEND;		{0x__1F}

	rts;			{0xdd21}
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
	jump SMPTEreq;		{0x__39}
	rts;			{0x__3B} 	{ MUST REMAIN UNUSED 4 error resyncing}
	jump SendRev;		{0x__3D}
	jump NewCode;		{0x--3f loadcode}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ChanVol:
	ay0=^Vol_Input;
	ar=ax1+ay0;
	i7=ar;
	Wait(READOK);
	ax0=DM(Read68);
	DM(i7,m4)=ax0;

	ar=ax1;
	ay0=5;
	ar=ar-ay0;
	if eq rts;

	ar=ax1;
	ar=PASS ar;
	if eq rts;

	ay0=1;
	ar=ax1-ay0;
	ChangeBank(ar);
	DM(Volume)=ax0;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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
GainUp:	si=imask;
	imask=b#000000;

	ReturnBF(ay1);
	ar=0x0600;
	ar=ar or ay1;
	DM(VolFreq)=ar;

	call GainToggle;

	ReturnBF(ay1);
	ar=0x0200;
	ar=ar or ay1;
	DM(VolFreq)=ar;

	imask=si;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
GainDown:
	si=imask;
	imask=b#000000;

	ReturnBF(ay1);
	ar=0x0200;
	ar=ar or ay1;
	DM(VolFreq)=ar;

	call GainToggle;

	ReturnBF(ay1);
	ar=0x0200;
	ar=ar or ay1;
	DM(VolFreq)=ar;

	imask=si;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
GainToggle:
	cntr=30;
	do Gwait3 until CE;
	nop;
Gwait3:	nop;

	ay1=0x0200;

	cntr=ax1;
	do Togg until CE;
		ar=ar XOR ay1;
		DM(VolFreq)=ar;
		cntr=10;
		do Gwait1 until CE;
		nop;
Gwait1:		nop;

		ar=ar XOR ay1;
		DM(VolFreq)=ar;
		cntr=30;
		do Gwait2 until CE;
		nop;
Gwait2:		nop;

Togg:		nop;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SetFreq:
	ay1=0x001F;
	ar=ax1 and ay1;
	sr=LSHIFT ar by 4 (lo);

	ChangeFreq(sr0);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SetSamplingRate:

	Wait(READOK);
	ar=DM(Read68);
	ay0=30;
	af=ar-ay0;
	if LT rts;

	DM(SampRate)=ar;

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SetTimer:
	Wait(READOK);
	ar=DM(Read68);
	DM(0x3ffd)=ar;
	Wait(READOK);
	ar=DM(Read68);
	DM(0x3ffb)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
MODE:	ar=ax1; ar= PASS ar;
	if NE jump ModeOn;
	jump ModeOff;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ModeOn:	ar=0;
	DM(BuffCntr)=ar;
	imask=b#100011;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
ModeOff:
	imask=b#000000;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
NewCode:imask=b#000000;
	m7=1;l7=0;
	Wait(READOK);
	ar=DM(Read68);
	i7=ar;
	Wait(READOK);
	ar=DM(Read68);
	CNTR=ar;
	POP PC;
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
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				SMPTE				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	SMSi7;
.var/dm/ram	SMSar;
.var/dm/ram	SMSay0;
.var/dm/ram	SMSsr0;
.var/dm/ram	SMSsr1;
.var/dm/ram	SMSpx;

Smpte:	DM(SMSar)=ar;
	DM(SMSay0)=ay0;
	DM(SMSsr0)=sr0;
	DM(SMSsr1)=sr1;
	DM(SMSpx)=px;
	DM(SMSi7)=I7;
{*********************************}
	ar=DM(SMPTEdata);
	sr=LSHIFT ar by -1 (lo);
	IF NOT FLAG_IN JUMP SMP0;
	ay0=0x8000;
	ar=sr0 or ay0;
	sr0=ar;
SMP0:	DM(SMPTEdata)=sr0;
	ay0=sr0;
{*********************************}
{*********************************}
	ar=DM(SMPTEmode);
	ar=PASS ar;
	if eq jump Sdata;
{*********************************}
Ssearch:ar=SMPTEsyncword;
	ar=ar-ay0;
	if NE jump SMxit;
	ar=0;
	DM(SMPTEmode)=ar;
	DM(SMPTEctr)=ar;
	jump SMxit;
{*********************************}
Sdata:
	ay0=DM(SMPTEctr);ar=ay0+1;DM(SMPTEctr)=ar;

	ay0=0x000F;
	ar=ar and ay0;		{ Quicken routine }
	if eq jump SAVE_SMP;

SMxit:
	I7=DM(SMSi7);
	ay0=DM(SMSay0);
	sr0=DM(SMSsr0);
	sr1=DM(SMSsr1);
	px=DM(SMSpx);
	ar=DM(SMSar);
	rts;

SAVE_SMP:ar=DM(SMPTEctr);
	sr=LSHIFT ar by -3 (lo);
	ay0=^SMPl;
	ar=sr0+ay0;
	I7=ar;

	ar=DM(SMPTEdata);
	jump (I7);

SMPl:	jump SMxit;
	nop;
	DM(SMPTEframe)=ar;
	jump SMxit;
	DM(SMPTEsec)=ar;
	jump SMxit;
	DM(SMPTEmin)=ar;
	jump SMxit;
	DM(SAVEhour)=ar;

	ar=DM(SMPTEmin);DM(SAVEmin)=ar;
	ar=DM(SMPTEsec);DM(SAVEsec)=ar;
	ar=DM(SMPTEframe);DM(SAVEframe)=ar;

	ar=1;DM(SMPTEmode)=ar;

	AddEvent(^SMSM,^SM_SMS);
	jump SMxit;

SM_SMS:	call Hsmpte;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Hsmpte:	ax0=DM(SAVEframe);
Frames:	ay0=0x0300;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;
	DM(SMPTEsecframe)=ar;

	ax0=DM(SAVEsec);
Secs:	ay0=0x0700;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;

	sr=LSHIFT ar by 6 (lo);
	ay0=DM(SMPTEsecframe);
	ar=sr0+ay0;
	sr=LSHIFT ar by 4 (lo);
	ay0=0x0003;
	ar=sr0 + ay0;
	DM(SMPTEsecframe)=ar;

	ax0=DM(SAVEmin);
Mins:	ay0=0x0700;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;
	DM(SMPTEhourmin)=ar;

	ax0=DM(SAVEhour);
Hours:	ay0=0x0300;
	ar=ax0 and ay0;
	my0=0x0500;
	MR=ar*my0 (SS);	{mr1=ar*10}

	ay0=0x000F;
	af=ax0 and ay0;
	ar=mr1 + af;

	sr=LSHIFT ar by 6 (lo);
	ay0=DM(SMPTEhourmin);
	ar=sr0+ay0;
	sr=LSHIFT ar by 4 (lo);
	ay0=0x0005;
	ar=sr0 + ay0;
	DM(SMPTEhourmin)=ar;

{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
DoSmpte:
	ax1=DM(SMPTEhourmin);
	ax0=DM(SMPTEsecframe);

	ay0=DM(OLDhourmin);
	ar=ax1-ay0;
	if eq jump FrSe;
	DM(OLDhourmin)=ay0;
	InteruptWrite(ax1);

Frse:	InteruptWrite(ax0);
	ar=0;
	si=imask;
	imask=0;
	ax1=DM(SAMPLCNTR);
	DM(SAMPLCNTR)=ar;
	imask=si;
	Wait(WRITEOK);
	DM(Write68)=ax1;

	RemoveEvent(^SMSM);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SM_STATE:
	DM(SM_TEST)=ax1;
	ar=ax1;
	ar=pass ar;

	if NE jump ADD_SMPTE_INTERUPT;
	AddEvent(^SMSMPTE,^SMSMPTE_SMS);
	rts;
ADD_SMPTE_INTERUPT:
	RemoveEvent(^SMSMPTE);
	rts;
SMSMPTE_SMS: RTI;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
SC_STATE:
	ar=ax1;
	ar=pass ar;
	if NE jump ADD_CONVESION_INTERUPT;
	ay0=b#011111;
	ar=imask;
	ar=ar and ay0;
	imask=ar;

	rts;
ADD_CONVESION_INTERUPT:
	ay0=b#100000;
	ar=imask;
	ar=ar or ay0;
	imask=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/				Misc				/\/\/}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
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


	m1=1;
	m2=-1;
	i1=^Stack;
	l1=100;

	m0=0;

	l7=0;

	m4=0;
	m5=1;
	m6=2;
	m7=8;

	I4=^buffer;
	I0=^buffer;
	I3=^buffer;

	l2=0;
	L0=BUFFERSIZE;
	L3=BUFFERSIZE;
	L4=BUFFERSIZE;
	L5=BUFFERSIZE;

{-- HL Regs --}
	l6=0;
	l5=0;
{-------------}
	icntl=b#10111;		{ enable IRQ nesting, all IRQs edge-sensitive }

	rts;

{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
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
.var/dm/ram	MR1save;
RecOutInt:
	if mv sat mr;
	DM(MR1save)=mr1;
	my0=DM(Vol_Output);
	MR=mr1*my0 (SU);
	if mv sat mr;

	DM(I4,M4)=mr1;
	DM(i2,m1)=mr1;
	MR1=DM(MR1save);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecInInt:
	ar=DM(Val_Input);
	DM(I4,M4)=ar;

	DM(i2,m1)=ar;
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
	ax1=MsgRecData;
	InteruptWrite(ax1);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordSend:
	ChangeBank(3);
	ay1=0;
	ay0=WRITEOK;

	CNTR=BlockSize;

	DO SendBs UNTIL CE;

	ax1=DM(I3,m0);			{ Get word }

RWLop:	ax0=DM(Status);
	ar=ax0 and ay0;			{ Wait for flag }
	if EQ jump RWLop;

	DM(Write68)=ax1;

SendBs:DM(I3,m1)=ay1;			{ Zero word  , add 1}

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOff:
	si=imask;
	imask=b#100000;

	i7=^PlayJump;
	ar=PM(i7,m4);
	i7=^PChan3;		{ I HATE writting self modifying code }
	PM(i7,m4)=ar;

	ar=0;px=0;
	i7=^RI;			{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	imask=si;

	ChangeBank(3);
	ay1=0;
	I0=^buffer;
	L0=BUFFERSIZE;
	cntr=BUFFERSIZE;
	do ClearRec until CE;
	DM(i0,m1)=ay1;
ClearRec:nop;

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordInOn:
	i7=^RecordInJump;
	jump RecordOn;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOutOn:
	i7=^RecordOutJump;
	jump RecordOn;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordOn:
	I3=I4;			{ ADD PRE DELAY FOR SAMPLE }

	ar=PM(i7,m4);
	i7=^PChan3;		{ I HATE writting self modifying code }
	PM(i7,m4)=ar;

	i7=^HRScall;
	ar=PM(i7,m4);
	i7=^RI;			{ I HATE writting self modifying code }
	PM(i7,m4)=ar;
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
RecordInJump:	Call RecInInt;
RecordOutJump:	Call RecOutInt;
PlayJump:	Call StandardPlay;
HRScall:	Call HrecordStart;
HRcall:		Call Hrecord;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				PLAY				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
StandardPlay:
	ar=DM(I4,M4);
	my0=DM(Volume);
	DM(i2,m1)=ar;
	DM(I4,M4)=ay0		,	MR=MR+ar*my0 (SU);
	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
MovePlayPtr:
	ChangeBank(ax1);
	Wait(READOK);
	ar=DM(Read68);

	sr=LSHIFT ar by -4 (lo);
	ay0=1; ar=sr0-ay0;sr0=ar;

	ay0=READOK;

	ay1=DM(I0,m0);
FDWt:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump FDWt;
	ar=DM(Read68);
	ar=ar+ay1;
	i0=i4;
	DM(I0,m1)=ar;

	CNTR = sr0;

	DO PlayF UNTIL CE;
	ay1=DM(I0,m0);
FDWat:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump FDWat;
	ar=DM(Read68);
	ar=ar+ay1;
	DM(I0,m1)=ar;
PlayF:	nop;

	DM(InPointer)=i0;

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
PlayData:{ax0 = raw data }
	ay0=0x000F;
	ar=ax0 and ay0;
	sr=LSHIFT ar by -1 (lo);
	ChangeBank(sr0);

	i0=DM(InPointer);

	ar=ax0;
	sr=LSHIFT ar by -4 (lo);

	ay0=READOK;
	CNTR = sr0;

	DO PlayD UNTIL CE;
	ay1=DM(I0,m0);
PDWat:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump PDWat;
	ar=DM(Read68);
	ar=ar+ay1;
PlayD:	DM(I0,m1)=ar;

	DM(InPointer)=i0;

	rts;

	POP PC;	jump Main_Loop; {jump instead of RTS}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
Hplay:	{ay1=0;}			{NEEDED ???? }
	ar=DM(BuffCntr);
	ar=PASS ar;
	if EQ rts;

	si=imask;
	imask=b#000000;		{ disable play interupt, so buffCntr does not change}
	ay0=DM(BuffCntr);
	ar=ay0-1;
	DM(BuffCntr)=ar;
	imask=si;

RI:	nop;			{ Self Modifying code (which I hate)}
				{, Call to Block Trasfer Record Data}

	ax1=MsgPlayData;
	InteruptWrite(ax1);

	jump Hplay;
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
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				Events				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.var/dm/ram	SaveI7;
.var/dm/ram	VFBsv;
HandleInt:
{нннннннннннннннннннннннннннннннн}
	ar=DM(BANKsave);
	DM(VFBsv)=ar;
	DM(SaveI7)=I7;
{нннннннннннннннннннннннннннннннн}
	mr=0;				{ Clear Mult (output) }
	i2=^Val_Input;
	ar=DM(ADC);
	sr=LSHIFT ar by 4 (lo);		{ Get Byte from Sampler }
{нннннннннннннннннннннннннннннннн}
	DM(i2,m1)=sr0;{Val_Input}
	my0=DM(Vol_Input);		{Monitor}
	MR=MR+sr0*my0 (SU);		{}
{нннннннннннннннннннннннннннннннн}
	ar=I4;
	ay0=BLOCKSIZE;
	ar=ar and ay0;

	ay1=DM(OldBit);
	af=ar xor ay1;
	if EQ jump Pskip;

	DM(OldBit)=ar;
	ay0=DM(BuffCntr);ar=ay0+1;DM(BuffCntr)=ar;
{нннннннннннннннннннннннннннннннн}
Pskip:	ay1=DM(FREQsave);
	ar=0x0200;
	ar=ar or ay1;
	ay1=ar;
{нннннннннннннннннннннннннннннннн}
	ay0=0;{ Do Not NonZero this register till modify}
	DM(VolFreq)=ay1;
PChan0:	Call StandardPlay;	{SMC}

	ar=1; ar=ar+ay1; DM(VolFreq)=ar;
PChan1:	Call StandardPlay;	{SMC}

	ar=2; ar=ar+ay1; DM(VolFreq)=ar;
PChan2:	Call StandardPlay;	{SMC}

	ar=3; ar=ar+ay1; DM(VolFreq)=ar;
PChan3:	Call StandardPlay;	{SMC}

	Modify(i4,m5);
{нннннннннннннннннннннннннннннннн}
	if mv sat mr;
	DM(Val_Output)=mr1;
	my0=DM(Vol_Output);
	MR=mr1*my0 (SU);
	if mv sat mr;
{нннннннннннннннннннннннннннннннн}
	sr=ASHIFT mr1 by -4 (hi);
	DM(DAC)=sr1;
{нннннннннннннннннннннннннннннннн}
	ay1=DM(SAMPLCNTR);
	ar=ay1+1;
	DM(SAMPLCNTR)=ar;
{нннннннннннннннннннннннннннннннн}
	I7=DM(SaveI7);
	ar=DM(VFBsv);
	ChangeBank(ar);
	rts;
{нннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{ \  /								\  / }
{  XX				TIMER				 XX  }
{ /  \								/  \ }
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
HLchanVAL:
	i6=^Val_Input;
	i5=^HiLo_Input;

	cntr=6;
	do HLop UNTIL CE;
	ar=DM(i6,m4);
	af= ABS ar;
	ar=dm(i5,m4);
	ar=ar-af;
	if GT jump Nxt;
	ar=PASS af;
	dm(i5,m4)=ar;
Nxt:	Modify(i5,m5);
HLOP:	Modify(i6,m5);

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
HL_SEND:
	i6=^HiLo_Input;
	i5=^Vol_Input;

	ax0=0;
	cntr=6;
	do HLoop UNTIL CE;
	sr0=DM(I6,m4);		{ Get HiLow Info }
	DM(I6,m5)=ax0;		{ Zero it out }

	my0=DM(I5,m5);
	MR=sr0*my0 (SU);
	if mv sat mr;

	Wait(WRITEOK);
	ar=abs mr1;
HLoop:	DM(Write68)=AR;	{ Write it }

	rts;
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
{/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\}
{нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн}
.endmod;
