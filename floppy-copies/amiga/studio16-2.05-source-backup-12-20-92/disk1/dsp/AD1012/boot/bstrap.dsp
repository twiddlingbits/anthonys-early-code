.module/ram/abs=0x03f4/boot=0		BootStrap;

.const		READOK=0x0001;
.entry		LoadCode, WaitRead;
.external	Status	,Read68;
.external	begin;

LoadCode:
	DO GetPgm UNTIL CE;
	Call WaitRead;
	px=DM(Read68);
	Call WaitRead;
	ar=DM(Read68);
GetPgm:	PM(i7,m7)=ar;
	jump	Begin;

WaitRead:
	ay0=READOK;
BSLoop:	ar=DM(Status);
	ar=ar and ay0;
	if EQ jump BSLoop;
	rts;				{12}
.endmod;
