extern "C"
{
	#include "Z80/Z80.h"
}

//*************************************************
// Globals

void RunTrs80Code(CDC *dc, unsigned char *, int);
int ResumeTrs80Code(CDC *dc, char keypress);
void PaintTrs80Display(CDC *dc);
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

class CTrs80NativeIO
{
public:
	CTrs80NativeIO();
	~CTrs80NativeIO();
	void RefreshDisplay(unsigned char *);
	void DrawChar(unsigned short offset, unsigned char value);
	void DrawTrs80Graphic(unsigned short offset, unsigned char value);
	void SetCDC(CDC *dc);		// To make more protable, i could move to a derived class
private:
	CDC *m_dc;
	CSize m_char_size;
	int m_cell_w;
	int m_cell_h;
	COLORREF m_color;
};

#define WIN_X_OFFSET 10
#define WIN_Y_OFFSET 10
	
//*************************************************
// Trs80

class CTrs80Emulator : public CZ80
{
public:
	CTrs80Emulator();
	~CTrs80Emulator();
	void Reset(void);
	bool RunCode(void);
	int ResumeCode(char keypress);
	bool BootRom(void);
	bool LoadCMD(unsigned char *image, int size);
	void LoadData(unsigned char *image, unsigned short size, unsigned short addr);
	void Display(unsigned char c);
	void ClearScreen(void);
	void SetIO(CTrs80NativeIO *);
	void SetPatch(word Addr);

public:
	// Overrides of Z80 virtual functions
	virtual byte CallCheckZ80(register word Addr);
	virtual void WrZ80(register word Addr,register byte Value);
	virtual byte RdZ80(register word Addr);
	virtual void PatchZ80(void);

public:
	CTrs80NativeIO *m_io;
	unsigned char m_memory[64*1024];

private:
	int m_load_cmd_transfer_addr;
	bool m_cursor_on;
	FILE *m_fp;
};

#define LOAD_NO_XFER_ADDR -1

#define SUSPEND_CODE_RUNNING		0
#define SUSPEND_CODE_WAIT_FOR_KEY	1
#define SUSPEND_CODE_EXIT			2

