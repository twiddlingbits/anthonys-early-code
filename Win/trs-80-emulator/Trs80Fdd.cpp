// Trs80Fdd.cpp: implementation of the CTrs80FddSimple class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Trs80Fdd.h"

//////////////////////////////////////////////////////////////////////
// Base CTrs80Device Class
//////////////////////////////////////////////////////////////////////

CTrs80Device::CTrs80Device()
{

}

CTrs80Device::~CTrs80Device()
{

}

short CTrs80Device::CanWrZ80(unsigned short, unsigned char, CTrs80Configurator* )
{
	return TRS80_DEVRET_UNHANDLED;
}

short CTrs80Device::CanRdZ80(unsigned short, unsigned char *)
{
	return TRS80_DEVRET_UNHANDLED;
}

short CTrs80Device::ClockTick(int newclocks)
{
	return TRS80_DEVRET_UNHANDLED;
}

//////////////////////////////////////////////////////////////////////
// Base FDD Class
//////////////////////////////////////////////////////////////////////

CTrs80Fdd::CTrs80Fdd()
{

}

CTrs80Fdd::~CTrs80Fdd()
{

}


void CTrs80Fdd::LoadDos(short)
{
}

void CTrs80Fdd::LoadDisk(short drive_num, unsigned char* dsk)
{
}


//////////////////////////////////////////////////////////////////////
// Simple implementation of true FDD Emulation
//////////////////////////////////////////////////////////////////////

CTrs80FddSimple::CTrs80FddSimple()
{

	LoadDos(TRS80_DOSTYPE_NEW_A);
	m_fdd_state = FDD_UNINIT;
	m_fdd_status_reg = FDD_STATUS_NOT_READY;	
}

CTrs80FddSimple::~CTrs80FddSimple()
{

}

//*************************************************

void CTrs80FddSimple::LoadDos(short dos_type)
{
	extern unsigned char NewDos80DskData[];
	extern unsigned char LdosDskData[];

	if (dos_type == TRS80_DOSTYPE_NEW_A)
		m_disk_image[0] = NewDos80DskData;
	else if (dos_type == TRS80_DOSTYPE_L_A)
		m_disk_image[0] = LdosDskData;
}

void CTrs80FddSimple::LoadDisk(short drive_num, unsigned char* dsk)
{
	if (drive_num < FDD_MAX_DRIVES)
		m_disk_image[drive_num] = dsk;
}

//*************************************************

short CTrs80FddSimple::CanRdZ80(register unsigned short Addr, unsigned char* retvalue)
{
	if (Addr==0x37EC)		// FDD Status Reg
	{
	//	TRACE("S-PC %x HL %x --",m_R.PC.W, m_R.HL.W);
		byte rs;
		rs = m_fdd_status_reg;
		
		if (m_fdd_state&FDD_TYPE_I)				
		{
			static unsigned long k = 0;

			if ( (k++)&0x100)
				rs |= (byte)FDD_STATUS_INDEX_PULSE;	// bad simulation of rotating index pulse

			if (m_fdd_track_reg == 0)			// book says this is an address error bit???
				rs |= (byte)FDD_STATUS_TRACK_ZERO;	

			rs|= FDD_STATUS_HEAD_LOADED;
		}
		// TRACE("FDD status = %02x\n",rs);
		*retvalue =  rs;	
		return TRS80_DEVRET_CLEAR_INT;
	}
	else if (Addr == 0x37EF) 	// FDD Data
	{
		//	TRACE("DATA REG READ T %x S %x B %x R.HL %x R.BC %x\n",m_fdd_track_reg, m_fdd_sector_reg, m_cur_secoffset, m_R.HL.W, m_R.BC.W );
		if (m_fdd_state == FDD_READ_SEC)
		{
			short rv = TRS80_DEVRET_HANDLED;
			byte d = *(m_disk_image[m_drive_select]+m_fdd_track_reg*2560+m_fdd_sector_reg*256+m_cur_secoffset++);
			if (m_cur_secoffset >= 256)
			{
				m_fdd_status_reg &= (~(FDD_STATUS_BUSY|FDD_STATUS_DRQ));	// Comand is clear busy & no more data (clear DRQ)
				rv = TRS80_DEVRET_TRIG_INT;

				if (m_cur_secoffset > 256)
				{
					TRACE("Unexpected Read -- no more data\n");
				}
			}
			*retvalue= d;
			return rv;
		}
		else
		{
			TRACE("FDD Data read with unsupported state\n");
			return TRS80_DEVRET_HANDLED;
		}
	}
	else if (Addr == 0x37ED)	// Track register
	{
		TRACE("TRACK REG READ T %x\n", m_fdd_track_reg);
		*retvalue =  m_fdd_track_reg;
		return TRS80_DEVRET_HANDLED;
	}
	else if (Addr == 0x37EE)	// Sector register
	{
		TRACE("SEC REG READ\n");
		*retvalue = m_fdd_sector_reg;
		return TRS80_DEVRET_HANDLED;
	}

	return TRS80_DEVRET_UNHANDLED;
}

//*************************************************

short CTrs80FddSimple::CanWrZ80(register unsigned short Addr,register unsigned char Value, CTrs80Configurator* config)
{

	if (Addr==0x37EC)				// FDD Command Reg
	{
		//
		// CMD: SEEK
		//
		if ( (Value&0xF0) == 0x10)	
		{
			TRACE("SEEK cmd%x TRACK%x\n",Value, m_fdd_data_reg);
			m_fdd_state = FDD_SEEK;
			if (m_fdd_track_reg != m_fdd_data_reg)
			{
				m_fdd_status_reg = FDD_STATUS_BUSY;
				// Everything else handled in LoopZ80()
			}
			else
			{
				m_fdd_status_reg = 0;
				return TRS80_DEVRET_TRIG_INT;
			}
		}
		//
		// CMD: RESTORE to track zero
		//
		else if ( (Value&0xF0) == 0)
		{
			TRACE("RESTORE\n");
			m_fdd_state = FDD_RESTORE;
			m_fdd_track_reg = 0;
			m_fdd_status_reg = 0;	
			return TRS80_DEVRET_TRIG_INT;
		}
		//
		// CMD: STEP
		//
		else if ( (Value&0xE0) == 0x20)		// Step again
		{
			TRACE("STEP\n");
			if (m_fdd_state == FDD_STEP_IN)
			{
				m_fdd_track_reg--;
				if (m_fdd_track_reg < 0)
					m_fdd_track_reg = 0;
			}
			else // Assume Step out
				m_fdd_track_reg++;
		
			m_fdd_status_reg = 0;
			return TRS80_DEVRET_TRIG_INT;
		}
		//
		// CMD: STEP IN
		//
		else if ( (Value&0xE0) == 0x40)	// Step in
		{
			TRACE("STEP IN\n");

			m_fdd_status_reg = 0;
			m_fdd_state = FDD_STEP_IN;
			m_fdd_track_reg++;

			return TRS80_DEVRET_TRIG_INT;
		}
		//
		// CMD: STEP OUT
		//
		else if ( (Value&0xE0) == 0x60)	// Step out
		{
			TRACE("STEP OUT\n");
			m_fdd_state = FDD_STEP_OUT;
			m_fdd_status_reg = 0;
			m_fdd_track_reg--;
			if (m_fdd_track_reg < 0)
				m_fdd_track_reg = 0;
			return TRS80_DEVRET_TRIG_INT;
		}
		//
		// CMD:  READ SECTOR
		//
		else if ( (Value&0xE3) == 0x80)
		{
			TRACE("READ SEC T %x S %x\n",m_fdd_track_reg, m_fdd_sector_reg);

/*
			if (m_fdd_track_reg==17 && m_fdd_sector_reg==8)
			{
				static kk = 0;

				if (kk++ == 1)
			}

		*/

			if (Value&0x10)
			{
				TRACE("Multi Sector Read!\n");
			}
			m_fdd_state = FDD_READ_SEC;
			m_fdd_status_reg = FDD_STATUS_BUSY;   // | FDD_STATUS_DRQ handled by LoopZ80

			if (m_fdd_track_reg == 17)	// Directory track's have non standard DAM of 0xFA on Mod I
				m_fdd_status_reg |= FDD_STATUS_DDM;		// Note standard DAM is 0xFB on Mod I
		}		
		//
		// CMD:  INTURRUPT
		//
		else if ( (Value & 0xF0) == 0xD0)	// Inturrupt:  Terminate command (no int)
		{
			m_cur_secoffset = 0;
			m_fdd_status_reg = 0;		// Halt comand, clear register
			m_fdd_state = FDD_FORCE;

			if (Value&8)	// Immediate Interrupt
			{
				return TRS80_DEVRET_TRIG_INT;
			}

		}
		else
		{
			TRACE("WR: Unhandeled FDD CMD %x\n", Value);
		}

	}
	//
	// FDD Write to data reg
	//
	else if (Addr == 0x37EF) 	// FDD Data
	{
		//TRACE("DATA REG WRITE\n");
		m_fdd_data_reg = Value;		// Also used by seek
		if (m_fdd_state == FDD_WRITE_SEC)
			*(m_disk_image[m_drive_select] + m_fdd_track_reg*2560+m_fdd_sector_reg*256+m_cur_secoffset++) = Value;
	}
	//
	// Set the sector 
	//
	else if (Addr == 0x37EE)
	{
		//TRACE("SECTOR REG WRITE %x\n", Value);
		if (Value > 9)
			Value = 9;
		m_fdd_sector_reg = Value;
		m_cur_secoffset = 0;
	}
	//
	// Set the track 
	//
	else if (Addr == 0x37ED)
	{
		//TRACE("TRACK REG WRITE %x\n", Value);
		if (Value > 39)		// !!!! NEED TO NOT HARD CODE MAX TRACK
			Value = 39;
			
		m_fdd_track_reg = Value;
	}
	//
	// Select a drive 
	//
	else if (((Addr)&~3) ==  0x37E0)	// 0x37E1, but bits 0,1 not decoded on Mod I
	{
		//TRACE("SELECT DRIVE %x\n", Value);
		if (m_fdd_timeout_clocks < 0)
		{
			TRACE("Drive was timed out!\n");
		}

		if (Value&1)
			m_drive_select = 0;
		else if (Value&2)
			m_drive_select = 1;
		else if (Value&4)
			m_drive_select = 2;
		else if (Value&8)
			m_drive_select = 3;
		else if (Value&0x10)
			m_drive_select = 4;
		else if (Value&0x20)
			m_drive_select = 5;
		else if (Value&0x40)
			m_drive_select = 6;
		else if (Value&0x80)
			m_drive_select = 7;

		m_fdd_timeout_clocks = config->m_trs80_clk_mhz*3;		// 3 second of clocks

		if (Value == 0)
			m_fdd_status_reg = 0x80;		// Not Ready
		else
			m_fdd_status_reg = 0x00;
	}
	else
	{
		return TRS80_DEVRET_UNHANDLED;
	}

	return TRS80_DEVRET_HANDLED;
}

short CTrs80FddSimple::ClockTick(int new_clocks)
{

	//
	// 3 seconds after the last write to 0x37E1
	// the drive turns off automaticlly
	//

	m_fdd_timeout_clocks-= new_clocks;
	if (m_fdd_timeout_clocks < 0)
		m_fdd_status_reg |= FDD_STATUS_NOT_READY;


	//
	// When a READ SECTOR command is issued, we pause a bit before asserting DRQ
	// to more realisticly simulate waiting for a byte
	// (NOT needed?  -- didn't fix problem i'm trying to fix)
	//
	if (m_fdd_state == FDD_READ_SEC && (m_fdd_status_reg&FDD_STATUS_BUSY))
		 m_fdd_status_reg|= FDD_STATUS_DRQ;

	//
	// This allows a few clocks to pass between the SEEK comand
	// and the busy bit going away (see WrZ80 -- SEEK)
	// (NOT needed? -- didn't fix problem i'm trying to fix)
	//
	if (m_fdd_state == FDD_SEEK && (m_fdd_status_reg&FDD_STATUS_BUSY))
	{
		m_fdd_track_reg = m_fdd_data_reg;
		m_fdd_status_reg&= (~FDD_STATUS_BUSY);

		
		return TRS80_DEVRET_TRIG_INT;
	}

	return TRS80_DEVRET_HANDLED;
}



