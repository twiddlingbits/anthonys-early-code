//
// Portable C++ Trs80 Emulator
// (c) 2002 Anthony J. Wood
// ajw@best.com
//
// 1/26/2002

#include "stdafx.h"
#include "Trs80Emulator.h"

//!!! TO DO
// Paint() happens in a seperate thread from main drawing,  yet GDI can only be called from one t
//    thread at a time.
// clean up dd objects on destruction of view
// New boot modes defined in CConfigurator need finishing implementing (need more rom's and dsk images)
// Improve user interface:  Allow user settings of configurator & software library
// FDD Code barly working:  Clean it up.
// improve the device vector patch code to know when the call is from TRSDOS or NOT (currently crashing)
// enhance dos emulation -- see MS BASIC book for more calls to emulate (i think?)
// allow execution of DISK BASIC, not just ROM BASIC
// when typing, characters are sometimes dropped
// Need user settable speed loop
// user control to turn on/off lc mod, or auto driver? user control for wierd shift-keyboard behaviour
// emulate serial port
// download all the trs80 software and test each one
// games that do many screen updates cause lots of flashing, fix.
// Add help page the describes trs80 keyboard mapping (CLEAR, for example)
// Disk emulator only supports JV1 right now, add JV3 and perhaps DMK
//****i love u **//

//**************************************************
//**************************************************
//
// CTrs80Emulator
//
//**************************************************
//**************************************************

CTrs80Emulator::CTrs80Emulator()
{
	m_fp = NULL;		// No open files
	m_load_cmd_transfer_addr=LOAD_NO_XFER_ADDR;
	m_display_on = TRUE;
	m_suspend_execution = FALSE;
	m_int_status = 0;
	m_int_clocks = 0;

	m_R.Trace = FALSE;

	m_fdd = &m_null_fdd;
}

//*************************************************

CTrs80Emulator::~CTrs80Emulator()
{
}

//*************************************************

//
// Boot boots the TRS80 Emulator according to the configuration flags (see CTrs80Configurator)
//

void CTrs80Emulator::Boot(CTrs80Configurator* config)
{
	// init member variables
	m_config = config;

	// Reset the Z80
	m_R.IPeriod=1000;
	ResetZ80();

	// clear memory to zero
	for (int i=0; i < sizeof(m_memory); i++)
		m_memory[i]=0x76;	// HALT

	//
	// BOOT TRS80 WITH NO ROM
	//

	if (config->m_syssoft_level == TRS80_SYSSOFT_NONE )
	{
		if (config->m_boot_mode == TRS80_BOOTMODE_AUTORUN)	// required flag
		{
			if (config->m_autorun_mode == TRS80_AUTORUNMODE_CMD)
			{
				// Set key ROM patches that we can emulate
				SetPatch(0x00);		// Reset
				SetPatch(0x33);		// Char Out
				SetPatch(0x49);		// Keyboard input
				SetPatch(0x01C9);	// Clear Screen

				LoadCMD(config->m_image, config->m_size);
				if (LOAD_NO_XFER_ADDR!=m_load_cmd_transfer_addr)
					RunCode(m_load_cmd_transfer_addr);
			}
		}
		return;
	}


	//
	// Load the ROM into the memory space

	LoadRom();	
	
	//
	// The following mod will modify the BASIC ROM so that it doesn't
	// assume bit 6 is generated automaticly by hardware
	if (config->m_lower_case_mod_installed)
	{
		// modify ROM image so that it doesn't convert LC to UC or remove bit 6
		m_memory[0x475] = 0;	// See Microsoft BASIC Decoded, pg. 83
		m_memory[0x476] = 0;	// SUB -> NOP, NOP
		m_memory[0x47B] = 0;	// See Microsoft BASIC Decoded, pg. 83
		m_memory[0x47C] = 0;	// SUB -> NOP, NOP
	}

	//
	// The default Floppy Disk Drive doesn't actually do anything.
	// If this boot is asking for a dos, insert a working FDD
	//
	if (config->m_syssoft_level == TRS80_SYSSOFT_DOS)
	{
		m_fdd = &m_the_floppy;
		m_fdd->LoadDos(config->m_dos_type);

		if (config->m_load_drive1)
			m_fdd->LoadDisk(1, config->m_dsk_image);

	}

	//
	// BOOT TRS80 WITH ROM AND EXECUTE A BASIC FILE
	//

	if (config->m_boot_mode == TRS80_BOOTMODE_AUTORUN)
	{
		if (config->m_autorun_mode == TRS80_AUTORUNMODE_ROMBASIC)
		{
			unsigned long rom_vector;

			m_display_on = FALSE;	// surpress display of "MEM SIZE? ", etc.

			// Set ROM keyboard input patch
			// when this trs80 call is made, it will cause the Z80
			// execution to suspend

			rom_vector = SetPatch(0x49);		// Keyboard input

			// Begin ROM code execution
			// Will run until the "MEM SIZE? " Prompt waits for keyboard input
			RunCode(0x0000);

			// Tell the Rom that 13 (return key) was pressed and resume execution
			// since the 0x49 patch is still set, the boot sequence will break again when
			// the ROM waits for keyboard input at the BASIC command prompt
			ResumeCode(13);		// press return (13), default memory size
			
			// Set TRSDOS patches
			// (if executed, will cause Z80 emulator to call PatchZ80()
			// which will emulate the appropriate TRSDOS function
			SetPatch(0x402D);	// Normal return to DOS
			SetPatch(0x4030);	// Error return to Dos
			SetPatch(0x441c);	// ??????
			SetPatch(0x4420);	// TRSDOS Init
			//!!!!!	SetPatch(0x1B);		// Write to device
			SetPatch(0x4428);	// TRSDOS Close
			SetPatch(0x4424);	// TRSDOS Open
			//!!!!	SetPatch(0x13);		// Input from device
			//!!!!  The above patches crash when the ROM image is executed
			//!!!!  NEED TO improve the patch code to know when the call is from TRSDOS or NOT
			SetPatch(0x0000);	// Some programs JP 0 to reset.  Catch them.

			// Finished intercepting the boot sequence
			RestorePatch(0x49, rom_vector);
			m_display_on = TRUE;

			// Load image and execute
			LoadBASIC(config->m_image, config->m_size);

			ResumeCode(13);		// press return (13), CHANGE THIS TO RUN IT

		}

		//
		// BOOT TRS80 WITH ROM AND EXECUTE A CMD FILE
		//

		else if (config->m_autorun_mode == TRS80_AUTORUNMODE_CMD)
		{
			unsigned long rom_vector;

			m_display_on = FALSE;	// surpress display of "MEM SIZE? ", etc.

			// Set ROM keyboard input patch
			// when this trs80 call is made, it will cause the Z80
			// execution to suspend

			rom_vector = SetPatch(0x49);		// Keyboard input

			// Begin ROM code execution
			// Will run until the "MEM SIZE? " Prompt waits for keyboard input
			RunCode(0x0000);

			// Tell the Rom that 13 (return key) was pressed and resume execution
			// since the 0x49 patch is still set, the boot sequence will break again when
			// the ROM waits for keyboard input at the BASIC command prompt
			ResumeCode(13);		// press return (13), default memory size
			
			// Set TRSDOS patches
			// (if executed, will cause Z80 emulator to call PatchZ80()
			// which will emulate the appropriate TRSDOS function
			SetPatch(0x402D);	// Normal return to DOS
			SetPatch(0x4030);	// Error return to Dos
			SetPatch(0x441c);	// ??????
			SetPatch(0x4420);	// TRSDOS Init
			//!!!!!	SetPatch(0x1B);		// Write to device
			SetPatch(0x4428);	// TRSDOS Close
			SetPatch(0x4424);	// TRSDOS Open
			//!!!!	SetPatch(0x13);		// Input from device
			//!!!!  The above patches crash when the ROM image is executed
			//!!!!  NEED TO improve the patch code to know when the call is from TRSDOS or NOT
			SetPatch(0x0000);	// Some programs JP 0 to reset.  Catch them.

			// Finished intercepting the boot sequence
			RestorePatch(0x49, rom_vector);
			m_display_on = TRUE;

			// Load image and execute
			bool s = LoadCMD(config->m_image, config->m_size);
			if (!s)
			{
				TRACE("Warning: LoadCMD returned error\n");
			}
			if (LOAD_NO_XFER_ADDR!=m_load_cmd_transfer_addr)
				RunCode(m_load_cmd_transfer_addr);
			else
			{
				TRACE("LoadCmd returned with no xfer address\n");
			}
		}
	}

	//
	// NO AutoRun
	//
	else
	{
		TRACE("Clean system boot\n");

		// Begin ROM code execution
		// No patches to intercept the boot
		RunCode(0x0000);

		//!!!!! NEED TO SET TRSDOS PATCHES!!
	}

	// Misc cleanup
	if (m_fp)
		fclose(m_fp);
	m_fp = NULL;		// No open files
}

//*************************************************

int CTrs80Emulator::RunCode(unsigned short addr)
{
	m_R.PC.W = addr;
	ResetHostElapsedTime();		// reset real time counter
	m_total_clocks = 0;			// reset z80 time counter
	RunZ80();

	return m_R.SuspendCode;
}

//*************************************************


int CTrs80Emulator::ResumeCode(char ascii_keypress)
{
	ResetHostElapsedTime();
	m_total_clocks = 0;			// reset z80 time counter
	m_R.AF.B.h = ascii_keypress;
	RunZ80();

	return m_R.SuspendCode;
}

//*************************************************

void CTrs80Emulator::SuspendExecutionAsync()
{
	m_suspend_execution = TRUE;
}

//*************************************************

void CTrs80Emulator::LoadRom(void)
{
	extern unsigned char ModIRomData[];
	extern int ModIRomDataSize;

	LoadData(ModIRomData, ModIRomDataSize, 0);
}


//*************************************************

bool CTrs80Emulator::LoadCMD(unsigned char *image, int size)
{
	int index = 0;
	int count, addr;
	int addr_low = 0xFFFF;
	int addr_hi = 0x0000;

	if (image[0]==0x6F && image[1] == 0x20 && image[2] == 0x6C && image[3] == 0x6F)	// TRSDOS2.3 BASIC
		index = index + 4;	

	while (index < size)
		switch (image[index++])
		{
			case 01:	// Load the following code
				count = image[index++];	// count includes 2 bytes that are "load addr"
				if (count <= 2)	// counts of 0 == 256, 1 = 257, 2 = 258
					count += 256;
				addr = image[index] + image[index+1]*256;
				if (addr < addr_low)	// For debuging, keep track of address range used by this load
					addr_low = addr;	//!!!! CAN REMOVE
				if (addr > addr_hi)		//!!!! CAN REMOVE 
					addr_hi = addr;		
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
				{
					TRACE("LoadCMD a [%x to %x]\n",addr_low, addr_hi);
					return FALSE;
				}

				if (image [index-1]==0 && image[index]==0)	// Probably EOF (padding zeros at end)
				{
					TRACE("LoadCMD b [%x to %x]\n",addr_low, addr_hi);
					return TRUE;
				}

				index = index + image[index]+1;
				break;
		}
	
	TRACE("LoadCMD c [%x to %x]\n",addr_low, addr_hi);
	return TRUE;
}

//*************************************************

void CTrs80Emulator::LoadData(unsigned char *image, unsigned short size, unsigned short addr)
{
	memcpy(m_memory+addr, image, size);
}

//*************************************************

bool CTrs80Emulator::LoadBASIC(unsigned char *image, int size)
{
	int i, last;

	if (image[0] != 0xFF)	// BASIC disk images always start with 0xFF
		return FALSE;

	i = m_memory[0x40A4] + m_memory[0x40A5]*256;	// basic program address
	memcpy(m_memory+i, image+1, size-1);

	//
	// A BASIC file is a dump of the RAM image.  Scan and patchup address
	// in case the memory location this BASIC program was placed at has changed.
	// (BASIC RAM images have the format <addr of next line><line number>...<0><addr of next line>...)
	//
	do
	{
		last = i;
		i = i+4;

		while (m_memory[i])
			i++;

		i++;

		m_memory[last] = i&0xFF;
		m_memory[last+1] = i>>8;

	} while (m_memory[i] || m_memory[i+1]);	// two bytes of zero mark the end of a basic program

	i = i + 2;					// point to the first byte past the basic program in memory

	m_memory[0x40f9] = i;		// Adjust end of BASIC program ptr to be correct
	m_memory[0x40fa] = i>>8;

	m_memory[0x40fb] = i;		// simple variable pointer
	m_memory[0x40fc] = i>>8;

	m_memory[0x40fd] = i;		// not sure, but it resets to end of program as well	
	m_memory[0x40fe] = i>>8;

	return TRUE;
}


//*************************************************

void CTrs80Emulator::Display(unsigned char c)
{
	unsigned short cursor;

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

unsigned long CTrs80Emulator::SetPatch(word Addr)
{
	unsigned long oldvalue;

	memcpy(&oldvalue, m_memory+Addr, 4);

	m_memory[Addr] =   0xED;		// Z80 Emu Patch Code
	m_memory[Addr+1] = 0xFE;		// Z80 Emu Patch Code
	m_memory[Addr+2] = 0xC9;		// RET

	return oldvalue;
}

//*************************************************

void CTrs80Emulator::RestorePatch(word Addr, unsigned long vector)
{
	memcpy(m_memory+Addr, &vector, 4);
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

	return TRUE;	// HACK HACK FOR ROM EXECUTE CODE

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

byte CTrs80Emulator::RdZ80(register word Addr)
{
	if (m_int_status&INT_FDD)	//!!!! DOESNT SEEM TO BE NEEDED????
	{
		// TRACE("RdZ80:  TRIGGERED INT_FDD\n");
		::IntZ80(&m_R,INT_IRQ);
	}

	byte retvalue;
	short rv;

	if (rv=m_fdd->CanRdZ80(Addr, &retvalue))
	{
		if (rv == TRS80_DEVRET_CLEAR_INT)
			m_int_status &= (~INT_FDD);			// reading status reg clears interupt request
		else if (rv == TRS80_DEVRET_TRIG_INT)
		{
			m_int_status |= INT_FDD;
			::IntZ80(&m_R,INT_IRQ);
		}

		return retvalue;
	}
	else if (((Addr)&~3) ==  0x37E0)	// Interupt status register
	{
		byte t=m_int_status;		
		m_int_status&= (~INT_TIMER);		// reading status reg clears interupt request
		return t;	
	}
	else if (Addr == 0x37E8)	// Printer
	{
	    return 0x30;	/* printer selected, ready, with paper, not busy */
	}
	else if (Addr >= 0x37E1 && Addr <= 0x37EF)
	{
		//m_R.Trace = TRUE;
		TRACE("UNHANDLED IO Reg Read %x\n",Addr);
		return 0;
	}
	else if (Addr >= 0x3801 && Addr <= 0x3880)
		return (PeekKeyboard(Addr));
	else
		return m_memory[Addr];
}

//*************************************************

void CTrs80Emulator::WrZ80(register word Addr,register byte Value)
{

	short rv;

	if (Addr < 0x3000) // ROM addr
	{
		TRACE("Wr: Write to ROM addr!\n");
		return;
	}

	m_memory[Addr] = Value;

	if (rv = m_fdd->CanWrZ80(Addr, Value, m_config))
	{
		if (rv == TRS80_DEVRET_TRIG_INT)
		{
			m_int_status|= INT_FDD;
			::IntZ80(&m_R,INT_IRQ);
		}
		return;
	}
	else if (Addr == 0x37E8)	// Printer
	{
	    TRACE("Printer: %x\n", Value);
	}
	//
	// Catch any other IO access and TRACE
	//
	else if (Addr >= 0x37E1 && Addr <= 0x37EF)
	{
		TRACE("UNHANDLD FDD Reg Write %x\n",Addr);
	}
	//
	// Display update?
	//
	else if (Addr < 0x4000)
		if (Addr >= 0x3C00)
			if (m_display_on)
				DrawChar(Addr - 0x3C00, Value);
}

//*************************************************


word CTrs80Emulator::LoopZ80()
{
	if (m_suspend_execution)
	{
		m_R.SuspendCode=SUSPEND_CODE_EXIT;
		return INT_QUIT;
	}
	else
	{
		//
		// The following code will throttle the speed back via Sleep()'s
		// so the at simulated z80 speed matches the speed this PC is running at
		//
		short new_clocks;
		__int64 elapsed_z80_time;
		__int64 elapsed_sys_time;


		// Run at max spped if "real speed" flag not set
		if (!m_config->m_run_real_speed)
			return INT_NONE;

		// Run at max speed if the display is off
		if (!m_display_on)	//!!!!! BUG THAT STUFF BELOW DOESN'T HAPPEN
			return INT_NONE;

		new_clocks = m_R.IPeriod + (-m_R.ICount);
		
		m_total_clocks += new_clocks;
		elapsed_z80_time = (m_total_clocks*1000000)/m_config->m_trs80_clk_mhz;  // in microseconds
		elapsed_sys_time = GetHostElapsedTime();

		if (elapsed_sys_time < elapsed_z80_time)
		{
			unsigned long time_delta;

			time_delta = (unsigned long)(elapsed_z80_time - elapsed_sys_time);
			if ( time_delta >= TRS80_SLEEP_TIME*1000)
			{
				AboutToSleep();	// notify parent
				::Sleep(time_delta/1000);
			}
		}

		if (m_fdd->ClockTick(new_clocks)==TRS80_DEVRET_TRIG_INT)
		{
			m_int_status|= INT_FDD;
			::IntZ80(&m_R,INT_IRQ);
		}
		
		//
		// Issue an interupt every 25ms -- the Expansion Interface timer interupt
		//

		m_int_clocks+= new_clocks;
		if ( (m_int_clocks*1000000)/m_config->m_trs80_clk_mhz > 25000)
		{
		
			//
			// rest the counter and trigger the timer interupt
			//

			m_int_clocks = 0;	
			m_int_status|= INT_TIMER;
			return INT_IRQ;	
		}

		return INT_NONE;
	}
}


//*************************************************

#define KB0(x) ((c[x]&1)<<0)
#define KB1(x) ((c[x]&1)<<1)
#define KB2(x) ((c[x]&1)<<2)
#define KB3(x) ((c[x]&1)<<3)
#define KB4(x) ((c[x]&1)<<4)
#define KB5(x) ((c[x]&1)<<5)
#define KB6(x) ((c[x]&1)<<6)
#define KB7(x) ((c[x]&1)<<7)

byte CTrs80Emulator::PeekKeyboard(word Addr)
{
	unsigned char* c;
	//
	// Get a 256 byte byte array.  One entry for each ASCII character, plus
	// special TRS80_KEY_ characters, such as TRS80_KEY_SHIFT.  A value of 1
	// indicates key is pressed, 0 means key is not pressed.
	//
	c = PeekKeyStatus();

	switch (Addr)
	{
	case 0x3801:
		return KB0('@')|KB1('A')|KB2('B')|KB3('C')|KB4('D')|KB5('E')|KB6('F')|KB7('G') |
					    KB1('a')|KB2('b')|KB3('c')|KB4('d')|KB5('e')|KB6('f')|KB7('g');

	case 0x3802:
		return KB0('H')|KB1('I')|KB2('J')|KB3('K')|KB4('L')|KB5('M')|KB6('N')|KB7('O') |
		       KB0('h')|KB1('i')|KB2('j')|KB3('k')|KB4('l')|KB5('m')|KB6('n')|KB7('o');

	case 0x3804:
		return KB0('P')|KB1('Q')|KB2('R')|KB3('S')|KB4('T')|KB5('U')|KB6('V')|KB7('W') |
		       KB0('p')|KB1('q')|KB2('r')|KB3('s')|KB4('t')|KB5('u')|KB6('v')|KB7('w');

	case 0x3808:
		return KB0('X')|KB1('Y')|KB2('Z') | KB0('x')|KB1('y')|KB2('z');
	
	case 0x3810:
		return KB0('0')|KB1('1')|KB2('2')|KB3('3')|KB4('4')|KB5('5')|KB6('6')|KB7('7') |
			            KB1('!')|KB2('"')|KB3('#')|KB4('$')|KB5('%')|KB6('&')|KB7('\'');

	case 0x3820:
		return KB0('8')|KB1('9')|KB2(':')|KB3(';')|KB4(',')|KB5('-')|KB6('.')|KB7('/') |
			   KB0('(')|KB1(')')|KB2('*')|KB3('+')|KB4('<')|KB5('=')|KB6('>')|KB7('?');
	
	case 0x3840:
		return KB0(0xD)|KB1(TRS80_KEY_CLEAR)|KB2(TRS80_KEY_BREAK)|KB3(TRS80_KEY_UP)|KB4(TRS80_KEY_DOWN)|KB5(TRS80_KEY_LEFT)|KB6(TRS80_KEY_RIGHT)|KB7(' ');
	
	case 0x3880:

		// This is set up to emulate the wierd TRS80 keyboard behavior of shift-for-lowercase
		// to change, swap the A,B.. and a,b,..

		unsigned char yes_shift, no_shift;
		yes_shift = // set to 1 if any char that needs shift key is pressed
			c['A']|c['B']|c['C']|c['D']|c['E']|c['F']|c['G']|
 			c['H']|c['I']|c['J']|c['K']|c['L']|c['M']|c['N']|c['O']|
			c['P']|c['Q']|c['R']|c['S']|c['T']|c['U']|c['V']|c['W']|
			c['X']|c['Y']|c['Z']|			
			c['!']|c['"']|c['#']|c['$']|c['%']|c['&']|c['\'']|
			c['(']|c[']']|c['*']|c['+']|c['<']|c['=']|c['>']|c['?'];

		no_shift = 
			c['a']|c['b']|c['c']|c['d']|c['e']|c['f']|c['g']|
 			c['h']|c['i']|c['j']|c['k']|c['l']|c['m']|c['n']|c['o']|
			c['p']|c['q']|c['r']|c['s']|c['t']|c['u']|c['v']|c['w']|
			c['x']|c['y']|c['z']|
			c['@']|c[':']|c[';']|c[',']|c['-']|c['.']|c['/'];

			// is_shift is set by any key that requires shift be pressed
			// on the trs80 keyboard.
			// no_shift is set by any key that never has shift pressed to generate
			// on the trs80 keyboard.
			// KB0(TRS80_KEY_SHIFT) is set if the shift key is pressed on the host keyboard.
			
			return ( (KB0(TRS80_KEY_SHIFT)&(~no_shift))) | yes_shift | KB4(TRS80_KEY_CTRL);

	default:
		return 0;
	}
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

//
//
// OLD CODE for copy & paste
//
//	m_R.SP.W=0x4288;	// System Stack Pointer -- Not sure if this is the best address

//	SetPatch(0x49);		// Keyboard input
//	SetPatch(0x33);		// Char Out
//	SetPatch(0x01C9);	// Clear Screen
//	SetPatch(0x402D);	// Normal return to DOS
//	SetPatch(0x4030);	// Error return to Dos
//	SetPatch(0x441c);	// ??????
//	SetPatch(0x4420);	// TRSDOS Init
//	SetPatch(0x1B);		// Write to device
///	SetPatch(0x4428);	// TRSDOS Close
//	SetPatch(0x4424);	// TRSDOS Open
//	SetPatch(0x13);		// Input from device


//	ClearScreen();		// Init Cursor location to upper left of screen

	// FOR DEBUGING
//	if (m_display_on)			// DELTE THIS STUFF (this is to skip mem test, etc)
//	{
//		if (Addr >= 0x4152 && Addr <=0x41A5)
//		{
//			TRACE("RD: Disk Basic Entry Point Read: %x\n",Addr);
//		//	return 0xC9;		// RET
//		}
//		else if (Addr >= 0x4400 && Addr <= 0x447b)
//		{
//			TRACE("RD: DOS Call Area Read: %x\n", Addr);
//		//	return 0xC9;		// RET
//		}
//
//		else if (Addr >= 0x402D && Addr <= 0x4032)
//		{
//			TRACE("RD: DOS Return Call Area Read: %x\n", Addr);
//		//	return 0;
//		}
//		else if (Addr >= 0x4152 && Addr < 0x5700)	// Below Newdos DISKBASIC
//		{
//			TRACE("RD: Unexpected Address: %x\n", Addr);
//		//	return 0;
//		}
//	}


	//
	// DEBUGING INFO START
	//

//	if (m_display_on)			// DELTE THIS STUFF (this is to skip mem test, etc)
//	{
//		if (Addr >= 0x4152 && Addr <=0x41A5)
//		{
//			TRACE("WR: Disk Basic Entry Point Write: %x\n",Addr);
//		}
//		else if (Addr >= 0x4400 && Addr <= 0x447b)
//		{
//			TRACE("WR: DOS Call Area Write: %x\n", Addr);
//		}
//
//		else if (Addr >= 0x402D && Addr <= 0x4032)
//		{
//			TRACE("WR: DOS Return Call Area Write: %x\n", Addr);
//		}
//		else if (Addr >= 0x4152 && Addr < 0x5700)	// Below Newdos80 DISKBASIC
//		{
//			TRACE("WR: Unexpected Address: %x\n", Addr);
//		}
//
//	}

	//
	// DEBUGING INFO END
	//