# Micro Adventure Language

Anthony's text adventure creation language.

   - [micro-adventure.rtf](micro-adventure.rtf)  - document that I wrote in 1998 after recreating in my memory how the micro adventure system works.
   - [tokenizer](./source/microadv.bas.txt) - tokenizes the .ADV, .LOC, and .TIT files.
   - [title screen editor](./source/advdraw.bas.txt) - Basic drawing program to create .TIT files
   - [runtime interpreter](./source/runtime.asm.txt) - Z-80 assembly code that interprets the tokens.

The original source can be found on the floppy disk copies that I have included here verbatim.
I have also pulled out what I believe are the final source files, and converted them to text files.
There are a few different adventures that I wrote using the micro adventure langauge in the game folder.

# TRS-80 Floppy Disk Copies

## DISK # 9 "" 02/23/81
	MOON/CMD	ADVENT/ASM	ADVENT/CMD	EDTASM/CMD
Sleeve say "old adventure run".  Appears to be an early rev of MicroAdv, before the basic assembler.

## DISK # 10 "" 11/07/83
	DSKTP/CMD	MICROADV/BAS	RUN/CMD	TAPE/CMD
	PENCIL01/SYS	PENCIL02/SYS	PENCIL03/SYS	ADVDRAW/BAS
	CMDFILE/CMD	MOVE/CMD	PENCIL/CMD
Contains NEWSOS/80.
Disk is named "AsmDevlp", but appears to be the main MicroAdv adventure development disk

## DISK # 102 "Alg 1-6+Ex" 08/08/81
	GOBLIN/CMD	ADVENT/CMD	ADVENT	EDTASM/CMD
Looks like an early MicroAdv version work disk (All assembly version).

## DISK # 121 "BADDISK" NoDate
	ASTEROID/ADV	GERMANY/ADV	ASTEROID/LOC	GERMANY/LOC
	GERMANY/TXT
Disk labeled Bad disk, backup only.
note 12-1-2023 - I can't find a copy of this disk.  Perhaps I didn't copy it from TRS-80.

## DISK # 122 "BAD DISK - ADV SOURCE" 6/9/83
Directory is empty.

## DISK # 123 "Micro Adv Text Files" 11/04/83 (Date from trashed disk, probably date corruption happened).
	TRY/CMD	MES/CMD	COUNT/CMD	GERMANY/ADV
	GERMANY/LOC	LCDVR/CMD	TRY/SRC	ADD/CMD
	MES/SRC	COUNT/SRC	TITLE	DISASSEM/CMD
	NAME	EDTASM/CMD
This is the disk that Tracy Rockwell trashed the directory track, and I rebuilt last weekend.   The original disk has not been changed, but on two copies, the directories are fixed. Following is the new correct dir that exists on the copies.  Both the copies and the original are labeled # 123.

	ASTEROID/CMD	GERMDSK/CMD	TIME/CMD	ASTEROID/ADV
	GERMANY/ADV	GERMANY/LOC	ASTEROID/LOC	GERM/OBJ
	GERMTAP/CMD	TIME/ADV	GERMANY/TIT	UNKNOWN/ADV
	TIME/LOC	GERMLC/TIT	ASTEROID/TIT	TIME/TIT
	TIME/TXT

## DISK # 124 "Micro Adventure Source" 06/09/83
	MICROADV/BAS	RTPAPE/ASM	CREATE/ASM	BC/BCK
	ADVDRAW/BAS	MOVE/ASM	DEALER/PCL	RUNTIME/ASM
	COMDOC/PCL
Both disks #124 and #123 were in with my C-64 disks, I have moved them to the Green TRS-80 disk holder.














