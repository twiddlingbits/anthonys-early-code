// Trs80Configurator.h: interface for the CConfigurator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TRS80_CONFIGURATOR_H)
#define TRS80_CONFIGURATOR_H

class CTrs80Configurator  
{
public:
	CTrs80Configurator();
	virtual ~CTrs80Configurator();

public:
	short m_syssoft_level;				// Use ROM?  Use Dos?
	short m_rom_type;					// Which ROM type?
	short m_dos_type;					// Which DOS type?
	short m_boot_mode;					// What to do during boot
	short m_autorun_mode;				// If AutoRun, what to do

	__int64 m_trs80_clk_mhz;			// clock speed to run at
	bool m_lower_case_mod_installed;	// TRUE = use LC letters (bit 6 is RAM)
	bool m_run_real_speed;				// TRUE = run at default trs80 speed
	bool m_load_drive1;					// TRUE = optional disk image in drive 1

	unsigned char* m_image;				// If AutoRun, image is here
	int m_size;							// Size of Image

	unsigned char m_dsk_image[256*17*80];	// Used to hold an optional drive 1 dsk image
};

// m_syssoft_level
// The Trs80 Emulator can operate in the following three basic modes.
// In the simplest mode, no ROM is used and basic ROM calls are emulated
// At the next level, a ROM is inserted into the memory space
// At the third level, a ROM is inserted and a DOS is inserted in virtual drive 0
//
#define TRS80_SYSSOFT_NONE		0	
#define TRS80_SYSSOFT_ROM		1
#define TRS80_SYSSOFT_DOS		2
//
// m_rom_type
//
#define TRS80_ROMTYPE_LV1		0		// Level I ROM
#define TRS80_ROMTYPE_LV2_A		1		// Level II ROM, "R/S ..."
#define TRS80_ROMTYPE_LV2_B		2		// Level II ROM, "Memory Size? "
//
// m_dos_type;
//
#define TRS80_DOSTYPE_TRS		0		// TrsDos 2.3
#define TRS80_DOSTYPE_NEW_A		1		// NewDos80  
#define TRS80_DOSTYPE_L_A		2		// LDos
//
// m_boot_mode
//
#define TRS80_BOOTMODE_NORMAL	0		// Boot as a TRS80 would normally, no autorun
#define TRS80_BOOTMODE_AUTORUN	1
//
// m_autorun_mode
//
#define TRS80_AUTORUNMODE_CMD		0
#define TRS80_AUTORUNMODE_ROMBASIC	1

//
//m_trs80_clk_mhz
//
#define TRS80_CLK_MOD1 (10000000/6)


#endif 