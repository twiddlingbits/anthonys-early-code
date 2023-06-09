// Configurator.cpp: implementation of the CConfigurator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Trs80Configurator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrs80Configurator::CTrs80Configurator()
{
	m_lower_case_mod_installed = TRUE;
	m_trs80_clk_mhz =	TRS80_CLK_MOD1;		//MOD I Speed
	m_run_real_speed =	TRUE;				//run at trs80 clock speed
	
	m_syssoft_level =	TRS80_SYSSOFT_DOS;
	m_rom_type		=	TRS80_ROMTYPE_LV2_A;
	m_dos_type		=	TRS80_DOSTYPE_NEW_A;
	m_boot_mode		=	TRS80_BOOTMODE_NORMAL;	
	m_autorun_mode	=	TRS80_AUTORUNMODE_CMD;	
	
	m_image = 0;
	m_size = 0;

	m_load_drive1 = FALSE;
}

CTrs80Configurator::~CTrs80Configurator()
{

}
