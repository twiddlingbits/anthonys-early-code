#ifndef	SSW_SOUNDSWITCH_H
#define	SSW_SOUNDSWITCH_H
/*
 *	SoundSwitch Audio Mixer Library
 *	Include file
 *
 *	This information is CONFIDENTIAL and PROPRIETARY
 * (C) Copyright 1994 SunRize Industries
 *	All Rights Reserved
 *
 *	$Id: soundswitch.h,v 1.5 1994/08/02 11:19:44 anthony Exp $
 *
 */

#ifndef	EXEC_TYPES_H
#include <exec/types.h>
#endif


/* Some handy constants...
 */
#define	SSW_GAIN_dB_MAXIMUM		255	//  +31.5 dB
#define	SSW_GAIN_dB_UNITY			192	//  +00.0 dB
#define	SSW_GAIN_dB_MINIMUM		1		//  -95.5 dB
#define	SSW_GAIN_dB_OFF			0		//  -oo   dB

#define	SSW_TRIM_dB_MAXIMUM		127	//  +63.5 dB
#define	SSW_TRIM_dB_UNITY			0		//   +0.0 dB
#define	SSW_TRIM_dB_MINIMUM		-127	//  -63.5 dB


/* Unit Flags
 */
#define	SSWUF_EXCLUSIVE		0x01

/* Error code values
 */
#define	SSWERR_NOERROR			0
#define	SSWERR_NOUNIT			-1
#define	SSWERR_UNITINUSE		-2
#define	SSWERR_ECHOFAILED		-3
#define  SSWERR_FIRMWAREOLD	-4

/* Flags for ssw_SetMode()
 */
#define	SSWMODEF_STEREO_LOCK	0x01	// set of left channel will also set right channel

/* Channel descriptors
 */
#define	SSW_CHANNEL_IN_1L		0
#define	SSW_CHANNEL_IN_1R		1
#define	SSW_CHANNEL_IN_2L		2
#define	SSW_CHANNEL_IN_2R		3
#define	SSW_CHANNEL_IN_3L		4
#define	SSW_CHANNEL_IN_3R		5
#define	SSW_CHANNEL_IN_4L		6
#define	SSW_CHANNEL_IN_4R		7
#define	SSW_CHANNEL_OUT_L		8
#define	SSW_CHANNEL_OUT_R		9
#define	SSW_CHANNEL_TOTAL		10

/* Level Information structure
 */
struct ssw_LevelInfo
{
	ULONG	li_Channel;
	LONG	li_Level;
};
#define	size_ssw_LevelInfo		sizeof(struct ssw_LevelInfo)

#define	LI_CHANNEL_END	((ULONG)~0)

/* Level Monitoring Hook
 */
struct ssw_MonitorHook
{
	struct Node			mh_Node;
	UWORD					mh_Status;
	ULONG					mh_Flags;
	APTR					mh_Unit;
	void		(* __asm mh_NotifyCallback)(
								register __a0 struct ssw_MonitorHook *mh,
								register __a1 APTR                   message );
	APTR					mh_UserData;
};
#define	size_ssw_MonitorHook	sizeof(struct ssw_MonitorHook)

#define	MH_FLG_GAIN		0x00000001		// notify on gain changes
#define	MH_FLG_TRIM		0x00000002		// notify on trim changes



/* Notification message for level changes
 */
struct ssw_NotifyMsg
{
	ULONG						nm_MethodID;
	struct ssw_LevelInfo	*nm_LevelInfo;
};
#define	size_ssw_NotifyMsg		sizeof(struct ssw_NotifyMessage)

enum
{	SSWNM_Dummy,
	SSWNM_SETGAIN,	
	SSWNM_SETTRIM,
};



/* Unit Information structure
 */
struct ssw_UnitInfo
{
	/* Descriptive information about the unit...
	 */
	UWORD		ui_Version;
	UWORD		ui_Revision;

	LONG		ui_NumChannels;		// how many channels
	LONG		ui_MeterBits;			// how many meter bits
	LONG		ui_GainBits;			// how many gain bits
	LONG		ui_TrimBits;			// how many trim bits

	LONG		ui_Reserved[3];		// will be zero
};
#define	size_ssw_UnitInfo		sizeof(struct ssw_UnitInfo)



#endif /* SSW_SOUNDSWITCH_H */
