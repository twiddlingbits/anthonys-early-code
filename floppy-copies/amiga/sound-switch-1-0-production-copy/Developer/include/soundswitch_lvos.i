	IFND	SSW_SOUNDSWITCH_LVOS_I
SSW_SOUNDSWITCH_LVOS_I	SET	1
 *
 * SoundSwitch Audio Mixer Library Include file
 * LVO definitions for the library
 *
 * This information is CONFIDENTIAL and PROPRIETARY
 * (C) Copyright 1994 SunRize Industries
 * All Rights Reserved
 *
 * $Id: soundswitch_lvos.i,v 1.2 1994/07/19 05:20:35 benfuller Exp $
 *
 *

	IFND EXEC_LIBRARIES_I
	INCLUDE "exec/libraries.i"
	ENDC

* Library vector offsets for "soundswitch.library"
*
	LIBINIT
	LIBDEF	_LVOssw_RexxHost
	LIBDEF	_LVOssw_LockBase
	LIBDEF	_LVOssw_AttemptLockBase
	LIBDEF	_LVOssw_UnlockBase
	LIBDEF	_LVOssw_ObtainHardware
	LIBDEF	_LVOssw_ReleaseHardware
	LIBDEF	_LVOssw_OpenUnit
	LIBDEF	_LVOssw_CloseUnit
	LIBDEF	_LVOssw_TestUnit
	LIBDEF	_LVOssw_GetUnitInfo
	LIBDEF	_LVOssw_AddMonitor
	LIBDEF	_LVOssw_RemoveMonitor
	LIBDEF	_LVOssw_SetMode
	LIBDEF	_LVOssw_SetChannelGain
	LIBDEF	_LVOssw_GetChannelGain
	LIBDEF	_LVOssw_SetChannelTrim
	LIBDEF	_LVOssw_GetChannelTrim
	LIBDEF	_LVOssw_GetMeterInfo


	ENDC	; SSW_SOUNDSWITCH_LVOS_I
