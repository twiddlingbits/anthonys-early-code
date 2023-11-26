#ifndef	SSW_SOUNDSWITCH_PROTOS_H
#define	SSW_SOUNDSWITCH_PROTOS_H
/*
 *	SoundSwitch Audio Mixer Library Include file
 *	Library Function Prototypes...
 *
 *	This information is CONFIDENTIAL and PROPRIETARY
 * (C) Copyright 1994 SunRize Industries
 *	All Rights Reserved
 *
 *	$Id: soundswitch_protos.h,v 1.4 1994/07/19 05:20:35 benfuller Exp $
 *
 */

#ifndef	SSW_SOUNDSWITCH_H
#include "soundswitch.h"
#endif


LONG  ssw_RexxHost( struct RexxMsg *rexxmsg );
void  ssw_LockBase( void );
ULONG ssw_AttemptLockBase( void );
void  ssw_UnlockBase( void );
void  ssw_ObtainHardware( void );
void  ssw_ReleaseHardware( void );
LONG  ssw_OpenUnit( LONG unit, ULONG flags, APTR *unitptr );
void  ssw_CloseUnit( APTR unit );
LONG  ssw_TestUnit( APTR unit );
LONG  ssw_GetUnitInfo( APTR unit, struct ssw_UnitInfo *unitinfo );
struct ssw_MonitorHook *ssw_AddMonitor( APTR unit, STRPTR name, ULONG flags, BYTE priority, void (*callback)() );
void  ssw_RemoveMonitor( struct ssw_MonitorHook *monitorhook );
ULONG ssw_SetMode( APTR unit, ULONG mask, ULONG bits );
LONG  ssw_SetChannelGain( APTR unit, ULONG channel_num, LONG dBgain );
LONG  ssw_GetChannelGain( APTR unit, ULONG channel_num );
LONG  ssw_SetChannelTrim( APTR unit, ULONG channel_num, LONG dBtrim );
LONG  ssw_GetChannelTrim( APTR unit, ULONG channel_num );
ULONG ssw_GetMeterInfo( APTR unit, ULONG channel_num );


#ifndef	SSW_SOUNDSWITCH_PRAGMAS_H
#include "soundswitch_pragmas.h"
#endif

#endif /* SSW_SOUNDSWITCH_PROTOS_H */
