//
// Portable C++ Trs80 Emulator
// (c) 2000 Anthony J. Wood
// ajw@best.com
//
// 1/1/2000

#include "stdafx.h"
#include "Trs80Core.h"


//**************************************************
//**************************************************
//
// Global Entry Points
//
//**************************************************
//**************************************************

CTrs80Emulator Trs80;
CTrs80NativeIO Io;

//**************************************************

void RunTrs80Code(CDC *dc, unsigned char* data, int size)
{
	Io.SetCDC(dc);
	
	Trs80.SetIO(&Io);
	Trs80.LoadCMD(data, size);
	Trs80.RunCode();
}

//**************************************************
// Resume Execution because a key was pressed

int ResumeTrs80Code(CDC *dc, char keypress)
{
	// If the TRS80 object isn' initialized & running yet, return
	if (Trs80.m_io == NULL)
		return SUSPEND_CODE_RUNNING;
	
	Io.SetCDC(dc);
	
	return Trs80.ResumeCode(keypress);
}

//**************************************************

void PaintTrs80Display(CDC *dc)
{
	// If the TRS80 object isn' initialized & running yet, return
	if (Trs80.m_io == NULL)
		return;
	
	Io.SetCDC(dc);

	Io.RefreshDisplay(Trs80.m_memory+0x3C00);
}

//**************************************************

void RunTrs80Basic(CDC *dc)
{
	Io.SetCDC(dc);
	
	Trs80.SetIO(&Io);
	Trs80.BootRom();
	Trs80.RunCode();
}

//**************************************************
//**************************************************
//
// CTrs80Emulator
//
//**************************************************
//**************************************************

CTrs80Emulator::CTrs80Emulator()
{
	m_io = NULL;
	m_fp = NULL;		// No open files

	Reset();

}

//*************************************************

CTrs80Emulator::~CTrs80Emulator()
{
}

//*************************************************

void CTrs80Emulator::Reset(void)
{

	// Reset the Z80
	m_R.IPeriod=20;
	ResetZ80();
	m_R.SP.W=0x4288;	// System Stack Pointer -- Not sure if this is the best address
	m_R.User=(void *)this;

	// clear memory to zero
	for (int i=0; i < sizeof(m_memory); i++)
		m_memory[i]=0;

	// Set patches (if executed, will cause Z80 emulator to call PatchZ80()
	SetPatch(00);		// Sometimes JP 0 used to reset
	SetPatch(0x49);		// Keyboard input
	SetPatch(0x33);		// Char Out
	SetPatch(0x01C9);	// Clear Screen
	SetPatch(0x402D);	// Normal return to DOS
	SetPatch(0x4030);	// Error return to Dos
	SetPatch(0x441c);	// ??????
	SetPatch(0x4420);	// TRSDOS Init
	SetPatch(0x1B);		// Write to device
	SetPatch(0x4428);	// TRSDOS Close
	SetPatch(0x4424);	// TRSDOS Open
	SetPatch(0x13);		// Input from device

	// Misc
	m_load_cmd_transfer_addr = LOAD_NO_XFER_ADDR;
	if (m_fp)
		fclose(m_fp);
	m_fp = NULL;		// No open files
}

//*************************************************

bool CTrs80Emulator::RunCode(void)
{
	if (m_load_cmd_transfer_addr == LOAD_NO_XFER_ADDR)
		return FALSE;

	if (m_io == NULL)
			return FALSE;

	// Init Cursor location to upper left of screen
	ClearScreen();
	
	m_R.PC.W = m_load_cmd_transfer_addr;
	RunZ80();

	return TRUE;
}

//*************************************************


int CTrs80Emulator::ResumeCode(char ascii_keypress)
{

	m_R.AF.B.h = ascii_keypress;
	RunZ80();

	return m_R.SuspendCode;
}


//*************************************************

bool CTrs80Emulator::BootRom(void)
{
extern unsigned char ModIRomData[];
extern int ModIRomDataSize;

	if (m_io == NULL)
			return FALSE;

	LoadData(ModIRomData, ModIRomDataSize, 0);

//	SetPatch(00);		// Sometimes JP 0 used to reset
	SetPatch(0x49);		// Keyboard input
//	SetPatch(0x33);		// Char Out
//	SetPatch(0x01C9);	// Clear Screen
//	SetPatch(0x402D);	// Normal return to DOS
//	SetPatch(0x4030);	// Error return to Dos
//	SetPatch(0x441c);	// ??????
//	SetPatch(0x4420);	// TRSDOS Init
//	SetPatch(0x1B);		// Write to device
//	SetPatch(0x4428);	// TRSDOS Close
//	SetPatch(0x4424);	// TRSDOS Open
//	SetPatch(0x13);		// Input from device
	
	m_R.PC.W = 0;
	RunZ80();

	return TRUE;
}

//*************************************************

void CTrs80Emulator::SetIO(CTrs80NativeIO *io)
{
	m_io = io;
}

//*************************************************

bool CTrs80Emulator::LoadCMD(unsigned char *image, int size)
{
	int index = 0;
	int count, addr;

	while (index < size)
		switch (image[index++])
		{
			case 01:	// Load the following code
				count = image[index++];	// count includes 2 bytes that are "load addr"
				if (count == 0)
					count = 256;
				addr = image[index] + image[index+1]*256;
				index = index+2;
				count = count - 2;
				if (count < 0)
					return FALSE;	//Bad Image
				while (count-- > 0)
					WrZ80(addr++, image[index++]);
				break;

			case 02:	// Here is transfer address
				if (image[index++]!=02)
					return FALSE;
				m_load_cmd_transfer_addr = image[index] + image[index+1]*256;
				index = index+2;
				break;

			default:	// skip next x bytes
				if (image[index-1] > 0x1F)
					return FALSE;

				if (image [index-1]==0 && image[index]==0)	// Probably EOF (padding zeros at end)
					return TRUE;

				index = index + image[index]+1;
				break;
		}
	return TRUE;
}

//*************************************************

void CTrs80Emulator::LoadData(unsigned char *image, unsigned short size, unsigned short addr)
{
	memcpy(m_memory+addr, image, size);
}

//*************************************************

void CTrs80Emulator::Display(unsigned char c)
{
	unsigned short cursor;
	
//	if (c > 127)	//Until we support graphics
//		c = '*';

	cursor = RdZ80(0x4020)+RdZ80(0x4021)*256;
	if (c==13 || c==10)	// return
	{
		if (m_cursor_on)
			WrZ80(cursor, ' ');
		
		cursor = cursor/64;
		cursor = cursor*64;
		cursor = cursor + 64;

		// Do we need to scroll?
		if (cursor == 0x4000)	
		{
			cursor = 0x4000 - 64;
			for (int i=0; i < (1024 - 64); i++)
				WrZ80(0x3c00+i, RdZ80(0x3c00+i+64));

			for (i=0; i < 64; i++)
				WrZ80(0x4000-i-1, 0x20);
		}
	}
	else if (c==8)	// backspace
	{
		if (cursor > 0x3C00)
		{
			if (m_cursor_on)
				WrZ80(cursor, 0x20);
			cursor--;
			WrZ80(cursor, 0x20);
		}
	}
	else if (c==0xE)	// Turn on cursor
	{
		m_cursor_on = TRUE;
	}
	else if (c==0xF)	// Turn off cursor
	{
		WrZ80(cursor, ' ');
		m_cursor_on = FALSE;
	}
	else if (c >=192)	// 192 to 255 are tabs for 0 to 63 spaces, respectively
	{
		cursor = cursor + (c-192);
	}
	else
	{
		WrZ80(cursor, c);
		cursor++;
	}

	if (m_cursor_on)
		WrZ80(cursor, '_');

	WrZ80(0x4020, cursor%256);
	WrZ80(0x4021, cursor/256);
}

//*************************************************

void CTrs80Emulator::ClearScreen(void)
{
	for (int i=0x3c00; i < 0x4000; i++)
		WrZ80(i, 0x20);

	// Set cursor position to upper left corner
	WrZ80(0x4020, 00);
	WrZ80(0x4021, 0x3c);

	m_cursor_on = FALSE;
}


//*************************************************

void CTrs80Emulator::SetPatch(word Addr)
{
	m_memory[Addr] =   0xED;		// Z80 Emu Patch Code
	m_memory[Addr+1] = 0xFE;		// Z80 Emu Patch Code
	m_memory[Addr+2] = 0xC9;		// RET
}

//**************************************************
//**************************************************
// Z80 overides for TRS80 Specific implementation of
// memory reads, writes, etc.
//**************************************************
//**************************************************

// Using to find calls that we don't trap yet that we should

byte CTrs80Emulator::CallCheckZ80(register word Addr)
{

//	return TRUE;	// HACK HACK FOR ROM EXECUTE CODE

	// TRACE("CallCheckZ80 CALL %x from PC %x\n",Addr, m_R.PC.W);

	// screen known calls
	switch (Addr)
	{
		case 0x01C9:	// Clear Screen
		case 0x49:		// Get Key Press	
		case 0x33:		// Print character
		case 0x441C:	// ?
		case 0x4420:	// TRSDOS Init
		case 0x1B:		// Device Output
		case 0x4428:	// TRSDOS Close
		case 0x4424:	// TRSDOS Open
		case 0x13:		// Read from device

		return TRUE;
	}

	if ( Addr < 0x4500 )	// When is end of sys calls?  Not sure.  This is past all TRSDOS Calls.
	{
		TRACE("CallCheckZ80 Addr Unknown System Call %0x\n", (int)Addr);
		return FALSE;
	}
	return TRUE;
}

//*************************************************

void CTrs80Emulator::WrZ80(register word Addr,register byte Value)
{
	m_memory[Addr] = Value;

	if (Addr < 0x4000)
		if (Addr >= 0x3C00)
			m_io->DrawChar(Addr - 0x3C00, Value);
}

//*************************************************

byte CTrs80Emulator::RdZ80(register word Addr)
{
	return m_memory[Addr];
}

//*************************************************

void CTrs80Emulator::PatchZ80()
{
	int i;
	char buffer[64];

	switch (m_R.PC.W-2)
	{
		case 00:			// Reset
		case 0x402D:		// Return to DOS
		case 0x4030:		// Error return to DOS
			m_R.Suspend=TRUE;
			m_R.SuspendCode=SUSPEND_CODE_EXIT;
			break;
		
		case 0x01C9:		// Clear Screen
			ClearScreen();
			break;

		case 0x49:		// Get Key Press	
			m_R.Suspend=TRUE;
			m_R.SuspendCode=SUSPEND_CODE_WAIT_FOR_KEY;
			break;

		case 0x33:		// Print character
			Display(m_R.AF.B.h);
			break;

		case 0x441c:	// ??????
			break;

		case 0x4420:	// TRSDOS Init
			// CheapO Init
			// Currently ignore's DCB, LRL
			// Only 1 file open at a time
			// Don't set correct TRSDOS error codes
			// Don't support POSN / READ/ WRITE / VERF / KILL
			// Also, assumes a PC style file spec.  Should parse and convert trs80 to pc
			
			if (m_fp)
			{
				TRACE("0x4420 failed: currently only support one open file\n");
				break;
			}
			for (i=0; m_memory[m_R.HL.W+i] > 13 && i < 63; i++)
				buffer[i] = m_memory[m_R.HL.W+i];
			buffer[i] = 0;
			m_fp = fopen(buffer, "wb");
			if (m_fp==NULL)
			{
				m_R.AF.B.l &= (~Z_FLAG);
				break;
			}
			m_R.AF.B.l |= Z_FLAG;
			break;

		case 0x1B:		// Write to device
			if (fwrite(&m_R.AF.B.h, sizeof (byte), 1, m_fp)==1)
				m_R.AF.B.l |= Z_FLAG;
			else
				m_R.AF.B.l &= (~Z_FLAG);
			break;

		case 0x4428:	// TRSDOS Close
			if (m_fp)
				fclose(m_fp);
			m_fp = NULL;
			m_R.AF.B.l |= Z_FLAG;
			break;

		case 0x4424:	// TRSDOS Open
			// CheapO OPEN.  See notes in CheapO INIT
			
			if (m_fp)
			{
				TRACE("0x4420 failed: currently only support one open file\n");
				break;
			}
			for (i=0; m_memory[m_R.HL.W+i] > 13 && i < 63; i++)
				buffer[i] = m_memory[m_R.HL.W+i];
			buffer[i] = 0;
			m_fp = fopen(buffer, "rb");
			if (m_fp==NULL)
			{
				m_R.AF.B.l &= (~Z_FLAG);
				break;
			}
			m_R.AF.B.l |= Z_FLAG;
			break;

		case 0x13:		// Read from device
			if (fread(&m_R.AF.B.h, sizeof (byte), 1, m_fp)==1)
			{
				m_R.AF.B.l |= Z_FLAG;
			}
			else
			{
				m_R.AF.B.l &= (~Z_FLAG);
				m_R.AF.B.h = 28;	// TRSDOS error code for EOF
				if (m_fp)	// close the file after error:  probably EOF & many programs don't close
				{
					fclose(m_fp);
					m_fp = NULL;
				}
			}
			break;

		default:
			TRACE("Unexpected PatchZ80()\n");
			break;
	}
}

//**************************************************
//**************************************************
//
// CTrs80NativeIO
//
// Interface between CTrs80Emulator and 
// Native (Windows) IO code
//
//**************************************************
//**************************************************

CTrs80NativeIO::CTrs80NativeIO()
{
}

//**************************************************

CTrs80NativeIO::~CTrs80NativeIO()
{
}

//**************************************************

void CTrs80NativeIO::SetCDC(CDC *dc)
{
	m_dc = dc;

	// cache some stuff for speed
	m_char_size = dc->GetTextExtent("X",1);	// font is fixed, so any char will work
	m_cell_w = m_char_size.cx / 2 + 1;  // Need if font sizes don't devide evenly
	m_cell_h = m_char_size.cy / 3 + 1;
	m_color = dc->GetTextColor();
}

//**************************************************

void CTrs80NativeIO::RefreshDisplay(unsigned char *videoram)
{
//	CSize size;
//	size = m_dc->GetTextExtent((char *)videoram, 64);
	
//	for (int i=0; i < 16; i++)
//		m_dc->TextOut(WIN_X_OFFSET, WIN_Y_OFFSET+size.cy*i, (char *)(videoram+i*64), 64);


	for (int i=0; i < 1024; i++)
		DrawChar(i, videoram[i]);
}

//**************************************************

void CTrs80NativeIO::DrawChar(unsigned short offset, unsigned char value)
{
	if (value&128 || value==32)
	{
		DrawTrs80Graphic(offset, value);
	}
	else
	{
		int x, y;

		x = WIN_X_OFFSET + (offset%64)*m_char_size.cx;
		y = WIN_X_OFFSET + (offset/64)*m_char_size.cy;

		if (value < 32)		// No Lowercase mod installed
			value |= 64;	// Bit 6 stuck high if no lower case mod installed

		m_dc->TextOut(x, y, (char *)&value, 1);
	}
}

//**************************************************freesswzzbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbmmmmmm

void CTrs80NativeIO::DrawTrs80Graphic(unsigned short offset, unsigned char val)
{
	int x, y;
	COLORREF bg;

	x = WIN_X_OFFSET + (offset%64)*m_char_size.cx;
	y = WIN_X_OFFSET + (offset/64)*m_char_size.cy;

	bg = m_dc->GetBkColor();
	m_dc->FillSolidRect(x, y, m_char_size.cx, m_char_size.cy, bg);

	if (val == 32)
		return;

	if (val&1)
		m_dc->FillSolidRect(x, y, m_cell_w, m_cell_h, m_color);

	y=y+m_cell_h;

	if (val&4)
		m_dc->FillSolidRect(x, y, m_cell_w, m_cell_h, m_color);

	y=y+m_cell_h;

	if (val&16)
		m_dc->FillSolidRect(x, y, m_cell_w, m_cell_h, m_color);

	x=x+m_cell_w;

	if (val&32)
		m_dc->FillSolidRect(x, y, m_cell_w, m_cell_h, m_color);

	y=y-m_cell_h;

	if (val&8)
		m_dc->FillSolidRect(x, y, m_cell_w, m_cell_h, m_color);

	y=y-m_cell_h;

	if (val&2)
		m_dc->FillSolidRect(x, y, m_cell_w, m_cell_h, m_color);

	m_dc->SetBkColor(bg);

}


//**************************************************
//**************************************************
//
// CZ80
//
// Based on Marat Fayzullin's portable Z80 Emulator
//
//**************************************************
//**************************************************

CZ80::CZ80()
{

	if (m_global_z80!=NULL)
	{
		TRACE0("ERROR!  Only one CZ80 allowed\n");
	}
	
	m_global_z80 = this;	// Hack to interface non context sensative C callbacks
}

//*************************************************

CZ80::~CZ80()
{
	m_global_z80 = NULL;
}

//*************************************************

void CZ80::ResetZ80(void)
{
	ASSERT(m_global_z80);

	::ResetZ80(&m_R);
}

//*************************************************

word CZ80::ExecZ80(void)
{
	return ::ExecZ80(&m_R);
}

//*************************************************

void CZ80::IntZ80(register word Vector)
{
	::IntZ80(&m_R, Vector);
}

//*************************************************

word CZ80::RunZ80(void)
{
	return ::RunZ80(&m_R);
}

//*************************************************
// Callbacks
// Override these to provide your own implementation
//*************************************************

void CZ80::WrZ80(register word Addr,register byte Value)
{
	return;
}

//*************************************************

byte CZ80::RdZ80(register word Addr)
{
	return 0;
}

//*************************************************

void CZ80::OutZ80(register word Port,register byte Value)
{
	TRACE("Unhandled Out %x,%x\n",Port, Value);
	return;
}

//*************************************************

byte CZ80::InZ80(register word Port)
{
	TRACE("Unhandled In %x\n",Port);
	return 0xFF;
}

//*************************************************

void CZ80::PatchZ80()
{
	return;
}

//*************************************************

word CZ80::LoopZ80()
{
	return INT_NONE;
	//	return INT_QUIT;
}

//*************************************************

byte CZ80::CallCheckZ80(register word Addr)
{
	return TRUE;	// go ahead and execute the call
}

//**************************************************
//**************************************************
//
// ::Z80
//
// These callback's are required by the Z80 emulator. 
// They are 'C' calling conventions, and are global.
//
//**************************************************
//**************************************************

CZ80 *CZ80::m_global_z80 = NULL;

byte ::CallCheckZ80(register word Addr, register Z80 *R)
{
	// If R was passed to all callbacks, then I could use
	// (CZ80 *)R->User->...
	// This would allow multiple Z80's
	
	return CZ80::m_global_z80->CallCheckZ80(Addr);
}

//*************************************************

void ::WrZ80(register word Addr,register byte Value)
{
	CZ80::m_global_z80->WrZ80(Addr, Value);
}

//*************************************************

byte ::RdZ80(register word Addr)
{
	return CZ80::m_global_z80->RdZ80(Addr);
}

//*************************************************

void ::OutZ80(register word Port,register byte Value)
{
	CZ80::m_global_z80->OutZ80(Port, Value);
}

//*************************************************

byte ::InZ80(register word Port)
{
	return 	CZ80::m_global_z80->InZ80(Port);
}

//*************************************************

void ::PatchZ80(register Z80 *R)
{
	CZ80::m_global_z80->PatchZ80();
}

//*************************************************

word ::LoopZ80(register Z80 *R)
{
	return 	CZ80::m_global_z80->LoopZ80();
}

//*************************************************

extern "C" void TraceHack(char *s)
{
	TRACE0(s);
}

//*************************************************
