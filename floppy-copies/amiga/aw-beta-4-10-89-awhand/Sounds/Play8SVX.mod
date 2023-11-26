MODULE Play8SVX;  (* Play8SVX 3.0  Benchmark Modula-2 *)
(*
   Play8SVX 3.0                Mike Scalora                  PLink : SCARY

   This MODULE is public domain.   Freely distributable as long as this 
   notice stays in.

   This program was originally uploaded to PeopleLink's Amiga Zone.  The 
   Amiga Zone has well over 3000 members, and a library of thousands of 
   public domain files.  If you're interested in joining us, call 
      800-524-0100 (voice) 
   or 800-826-8855 (modem).

   SYNTAX:

     PLAY8SVX filename [ filename ...]

*)
FROM PortsUtil   IMPORT CreatePort, DeletePort;
FROM AudioDevice IMPORT AudioName, IOAudio, ADIOPerVol;
FROM SYSTEM      IMPORT BYTE, ADR, ADDRESS;
FROM IODevices   IMPORT CmdWrite, IOQuick, IOFlagsSet, BeginIO, WaitIO,
                        OpenDevice, CloseDevice;
FROM Memory      IMPORT AllocMem, FreeMem, MemReqSet, MemPublic, MemChip;
FROM System      IMPORT argv, argc;
FROM AmigaDOS    IMPORT Open, Read, Seek, Close, FileHandle, ModeOldFile,
                        Write, Output;
CONST
  FORMid = 0464F524DH;  (*FORM*)
  BODYid = 0424F4459H;  (*BODY*)
  VHDRid = 056484452H;  (*VHDR*)
  SVXid  = 038535658H;  (*8SVX*)

VAR
  C,D : CARDINAL;
  stdout : FileHandle;

PROCEDURE WriteString(S : ARRAY OF CHAR);
  VAR
    C : CARDINAL;
    err : LONGINT;
  BEGIN
    IF stdout=NIL THEN RETURN; END;
    C := 0; WHILE S[C]#0C DO INC(C); END;
    err := Write(stdout,ADR(S),LONGCARD(C));
  END WriteString;

PROCEDURE MAIN(S : ADDRESS);

VAR
  Sound : IOAudio;
  SoundData : ADDRESS;
  I         : INTEGER;
  err       : LONGINT;
  SoundUnit : LONGCARD;  

  SFH : FileHandle;

  ILen : LONGINT;
  AnID, ALen : LONGCARD;
  
  VHDR : RECORD
    OSHS, RHS, SPHC : LONGCARD;
    SamPerSec : CARDINAL;
    Octave, Comp, Vol : BYTE; 
    a, b, c : LONGCARD;         
  END;

PROCEDURE Read4(a : ADDRESS);
  BEGIN
    IF Read(SFH,a,4D)#4D THEN 
      WriteString('Error reading from input file!\n'); Close(SFH); HALT; END;
  END Read4;

BEGIN
  SFH := Open(S,ModeOldFile);
  IF SFH=FileHandle(0) THEN
    WriteString('Could not open file input file!\n'); HALT; END;

  Read4(ADR(AnID));       (* read FORM id *)
  IF AnID#FORMid THEN WriteString('Input file is not IFF format!\n');
    Close(SFH); HALT; END;
  Read4(ADR(ALen));       (* read FORM length *)
  Read4(ADR(AnID));       (* read 8SVX id *)
  IF AnID#SVXid THEN WriteString('Input file is not 8SVX IFF format!\n');
    Close(SFH); HALT; END;
  Read4(ADR(AnID));
  IF AnID#VHDRid THEN 
    WriteString('Input file is not valid 8SVX IFF format!\n'); HALT; END; 
  Read4(ADR(ALen));  
  ILen := Read(SFH,ADR(VHDR),ALen);    
  IF LONGCARD(ILen)#ALen THEN
    WriteString('Error reading file!\n'); Close(SFH); HALT; END;

  Read4(ADR(AnID));       (* read next id *)
  Read4(ADR(ALen));       (* read next length *)
  I := 0;
  WHILE (AnID#BODYid) AND (I<20) DO INC(I);
    ILen := Seek(SFH,LONGINT(ALen),0D); 
    IF ILen=-1D THEN
      WriteString('Error seeking file!\n'); Close(SFH); HALT; END;
    Read4(ADR(AnID));       (* read next id *)
    Read4(ADR(ALen));       (* read next length *)
  END;
  IF AnID#BODYid THEN WriteString('No BODY found in input file!\n');
    Close(SFH); HALT; END;

  SoundData := AllocMem(ALen,MemReqSet{MemPublic,MemChip});
  IF SoundData=NIL THEN WriteString('Could not allocate CHIP memory!\n');
    Close(SFH); HALT; END;

  AnID := Read(SFH,SoundData,ALen); Close(SFH);

  Sound.ioaRequest.ioMessage.mnReplyPort := CreatePort(ADR('MyAIOPort'),0);
  IF Sound.ioaRequest.ioMessage.mnReplyPort=NIL THEN 
    FreeMem(SoundData,ALen);
    WriteString('Could not create Reply Port!\n'); HALT; END;

  SoundUnit := 03050A0CH;                              (* any stereo pair *)
  Sound.ioaRequest.ioMessage.mnNode.lnPri := BYTE(10);
  Sound.ioaData := ADR(SoundUnit);
  Sound.ioaLength := 4D; (*SIZE(SoundUnit);*)

  err := OpenDevice(ADR(AudioName),0D,ADR(Sound.ioaRequest),0D);
  IF err#0D THEN WriteString('Could not open Audio Device!\n'); 
    FreeMem(SoundData,ALen); HALT; END; 

  WITH Sound DO
    ioaRequest.ioCommand := CmdWrite;
    ioaRequest.ioFlags := IOFlagsSet{IOQuick,ADIOPerVol};
    ioaData := SoundData;
    ioaCycles := 1;
    ioaLength := ALen;
    ioaPeriod := CARDINAL(3579545D DIV LONGCARD(VHDR.SamPerSec));
    ioaVolume := 64;
  END;

  BeginIO(ADR(Sound.ioaRequest));
  err := WaitIO(ADR(Sound.ioaRequest));

  FreeMem(SoundData,ALen);

  CloseDevice(ADR(Sound));

  DeletePort(Sound.ioaRequest.ioMessage.mnReplyPort^);
END MAIN;


BEGIN
  stdout := Output();

  IF (argc<2) OR ((argv^[1]^[0]='?') AND (argv^[1]^[1]=0C)) THEN 
    WriteString('\nUsage\n\n  ');
    C := 0;
      WHILE argv^[0]^[C]#0C DO
        argv^[0]^[C] := CAP(argv^[0]^[C]); INC(C);
      END;
    WriteString(argv^[0]^);
    WriteString(' filename\n\n'); 
    HALT;
  ELSE
    FOR C := 1 TO argc-1 DO
      MAIN(argv^[C])
    END;
  END; 
END Play8SVX.
