	IFND	SSW_SOUNDSWITCH_I
SSW_SOUNDSWITCH_I	SET	1
 *
 * SoundSwitch Audio Mixer Library Include file
 * Include file
 *
 * This information is CONFIDENTIAL and PROPRIETARY
 * (C) Copyright 1994 SunRize Industries
 * All Rights Reserved
 *
 * $Id: soundswitch.i,v 1.5 1994/08/02 12:28:38 benfuller Exp $
 *
 *

	IFND EXEC_TYPES_I
	INCLUDE "exec/types.i"
	ENDC

	IFND EXEC_NODES_I
	INCLUDE "exec/nodes.i"
	ENDC

* Some handy constants...
*
SSW_GAIN_dB_MAXIMUM	EQU	255	;  +31.5 dB
SSW_GAIN_dB_UNITY	EQU	192	;  +00.0 dB
SSW_GAIN_dB_MINIMUM	EQU	1	;  -95.5 dB
SSW_GAIN_dB_OFF		EQU	0	;  -oo   dB

SSW_TRIM_dB_MAXIMUM	EQU	127	;  +63.5 dB
SSW_TRIM_dB_UNITY	EQU	0	;   +0.0 dB
SSW_TRIM_dB_MINIMUM	EQU	-127	;  -63.5 dB

* Unit Flags
*
SSWUF_EXCLUSIVE		EQU	$01

* Error code values
*
SSWERR_NOERROR		EQU	0
SSWERR_NOUNIT		EQU	-1
SSWERR_UNITINUSE	EQU	-2
SSWERR_ECHOFAILED	EQU	-3
SSWERR_FIRMWAREOLD	EQU	-4

* Flags for ssw_SetMode()
*
	BITDEF	SSWMODE,STEREO_LOCK,0	; set of left channel will also set right channel

* Channel descriptors
*
SSW_CHANNEL_IN_1L	EQU	0
SSW_CHANNEL_IN_1R	EQU	1
SSW_CHANNEL_IN_2L	EQU	2
SSW_CHANNEL_IN_2R	EQU	3
SSW_CHANNEL_IN_3L	EQU	4
SSW_CHANNEL_IN_3R	EQU	5
SSW_CHANNEL_IN_4L	EQU	6
SSW_CHANNEL_IN_4R	EQU	7
SSW_CHANNEL_OUT_L	EQU	8
SSW_CHANNEL_OUT_R	EQU	9
SSW_CHANNEL_TOTAL	EQU	10

* Level Information structure
*
 STRUCTURE ssw_LevelInfo,0
	ULONG	li_Channel
	LONG	li_Level
	LABEL	size_ssw_LevelInfo

LI_CHANNEL_END		EQU	$FFFFFFFF

* Level Monitoring Hook
*
 STRUCTURE ssw_MonitorHook,0
	STRUCT	mh_Node,LN_SIZE
	UWORD	mh_Status
	ULONG	mh_Flags
	APTR	mh_Unit
	FPTR	mh_NotifyCallback
	APTR	mh_UserData
	LABEL	size_ssw_MonitorHook

MH_FLG_GAIN		EQU	$00000001	; notify on gain changes
MH_FLG_TRIM		EQU	$00000002	; notify on trim changes


* Notification message for level changes
*
 STRUCTURE ssw_NotifyMsg,0
	ULONG	nm_MethodID
	APTR	nm_LevelInfo
	LABEL	size_ssw_NotifyMsg


SSWNM_Dummy	EQU	0
SSWNM_SETGAIN	EQU	(SSWNM_Dummy+1)
SSWNM_SETTRIM	EQU	(SSWNM_Dummy+2)



* Unit Information structure
*
 STRUCTURE ssw_UnitInfo,0
	* Descriptive information about the unit...
	*
	UWORD	ui_Version
	UWORD	ui_Revision

	LONG	ui_NumChannels		; how many channels
	LONG	ui_MeterBits		; how many meter bits
	LONG	ui_GainBits		; how many gain bits
	LONG	ui_TrimBits		; how many trim bits

	STRUCT	ui_Reserved,4*3		; will be zero

	LABEL	size_ssw_UnitInfo



	ENDC	; SSW_SOUNDSWITCH_I
