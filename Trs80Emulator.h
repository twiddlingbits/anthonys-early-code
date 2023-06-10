#ifndef TRS80EMULATOR_H
#define TRS80EMULATOR_H

extern "C"
{
	#include "Z80/Z80.h"
}

#include "Trs80Configurator.h"
#include "Trs80Fdd.h"

//*************************************************
// Globals

void RunTrs80Code(CDC *dc, unsigned char *, int);
int ResumeTrs80Code(CDC *dc, char keypress);
void RunTrs80Basic(CDC *dc);

//*************************************************
// CZ80
//
// Based on Marat Fayzullin's portable Z80 Emulator


class CZ80
{
public:
	CZ80();
	~CZ80();

	// Functions implemented by the Z80 emulator code
	void ResetZ80(void);
	word ExecZ80(void);
	void IntZ80(register word Vector);
	word RunZ80(void);


	// callbacks from the Z80 emulation code
	virtual byte CallCheckZ80(register word Addr);
	virtual void WrZ80(register word Addr,register byte Value);
	virtual byte RdZ80(register word Addr);
	virtual void OutZ80(register word Port,register byte Value);
	virtual byte InZ80(register word Port);
	virtual void PatchZ80(void);
	virtual word LoopZ80(void);

	// The Z80 registers and contect
	Z80 m_R;

	// There can only be one CZ80 because of the way the Z80 emulator code is written
	// RdZ80, WrZ80, etc have no context information

	static CZ80 *m_global_z80;
};

//*************************************************
// Trs80

class CTrs80Emulator : public CZ80
{
public:
	CTrs80Emulator();
	~CTrs80Emulator();
	void Boot(CTrs80Configurator* config);
	int RunCode(unsigned short addr);
	int ResumeCode(char keypress);
	void SuspendExecutionAsync();
	void LoadRom(void);
	bool LoadCMD(unsigned char *image, int size);
	bool LoadBASIC(unsigned char *image, int size);
	void LoadData(unsigned char *image, unsigned short size, unsigned short addr);
	void Display(unsigned char c);
	void ClearScreen(void);
	unsigned long SetPatch(word Addr);
	void RestorePatch(word Addr, unsigned long vector);
	byte PeekKeyboard(word Addr);

protected:	// Must be over ridden with system specific IO
	virtual void DrawChar(unsigned short offset, unsigned char value) = NULL;
	virtual unsigned char* PeekKeyStatus() = NULL;
	virtual __int64 GetHostElapsedTime() = NULL;
	virtual void ResetHostElapsedTime() = NULL;

public:
	// Overrides of Z80 virtual functions
	virtual byte CallCheckZ80(register word Addr);
	virtual void WrZ80(register word Addr,register byte Value);
	virtual byte RdZ80(register word Addr);
	virtual void PatchZ80(void);
	virtual word LoopZ80(void);

public:
	bool m_display_on;					// TRUE = render video RAM, FALSE = don't render
	unsigned char m_memory[64*1024];
	CTrs80Configurator* m_config;

private:
	int m_load_cmd_transfer_addr;
	bool m_cursor_on;
	FILE *m_fp;
	bool m_suspend_execution;
	__int64 m_total_clocks;		// how many z80 cycles have elapsed since code started running?
	__int64 m_int_clocks;		// how many z80 cycles since last "heartbeat" int?
	long m_delay_count;			// delay loop counter used to sync z80 speed with acutal speed
	unsigned char m_int_status;
	CTrs80FddSimple m_the_floppy;
	CTrs80Fdd m_null_fdd;
	CTrs80Fdd *m_fdd;
};


// int status reg bits (0x37E0)
#define INT_FDD					(1<<6)
#define INT_TIMER				(1<<7)

#define LOAD_NO_XFER_ADDR -1

#define SUSPEND_CODE_RUNNING		0
#define SUSPEND_CODE_WAIT_FOR_KEY	1
#define SUSPEND_CODE_EXIT			2

#define TRS80_KEY_SHIFT	1
#define TRS80_KEY_CTRL	2
#define TRS80_KEY_LAST_SPECIAL 2

#define TRS80_KEY_BREAK	3
#define TRS80_KEY_CLEAR	28
#define TRS80_KEY_UP	27
#define TRS80_KEY_DOWN	26
#define TRS80_KEY_LEFT	24
#define TRS80_KEY_RIGHT	25


#endif