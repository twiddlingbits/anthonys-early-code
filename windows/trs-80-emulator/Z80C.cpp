#include "stdafx.h"
#include "Trs80Emulator.h"


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
	// TRACE("Unhandled Out %x,%x\n",Port, Value);
	return;
}

//*************************************************

byte CZ80::InZ80(register word Port)
{
	// TRACE("Unhandled In %x\n",Port);
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
