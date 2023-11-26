/*
 *	SoundSwitch Audio Mixer Library
 * Rexx program to test the soundswitch.library rexx interface.
 *
 * (C) Copyright 1994 SunRize Industries
 *	All Rights Reserved
 *
 *	$Id: sstest.rexx,v 1.0 1994/07/19 05:23:00 benfuller Exp $
 *
 */

/* Some Useful constants...
 */
GAIN_DB_MAX   = 255  /* +31.5 dB */
GAIN_DB_UNITY = 192  /*   0.0 dB */
GAIN_DB_MIN   = 1    /* -95.5 dB */
GAIN_DB_OFF   = 0    /*   -oo dB */
GAIN_DB_INC   = 2    /*   1.0 dB */

GAIN_DB_POS_12 = (GAIN_DB_UNITY + (GAIN_DB_INC * 12)) /* +12.0 dB */
GAIN_DB_NEG_24 = (GAIN_DB_UNITY - (GAIN_DB_INC * 24)) /* -24.0 dB */

CHAN_IN_1_L   = 0
CHAN_IN_1_R   = 1
CHAN_IN_2_L   = 2
CHAN_IN_2_R   = 3
CHAN_IN_3_L   = 4
CHAN_IN_3_R   = 5
CHAN_IN_4_L   = 6
CHAN_IN_4_R   = 7
CHAN_MASTER_L = 8
CHAN_MASTER_R = 9

/* set of left channel will also set right channel */
SSWMODEF_STEREO_LOCK = 1

/* Make sure the soundswitch library is there...
 */
if ~SHOW('L','soundswitch.library') then
	say "ADDLIB =" ADDLIB( "soundswitch.library", 0, -30, 0 )

/* Get and display all rexx functions
 */
cmds = ssw_Help()
say "soundswitch.library REXX functions"
say "------------------------------"
do until cmds = ""
	parse var cmds nextcmd cmds
	cmdargs = ssw_Help( nextcmd )
	say LEFT( nextcmd, 24 ) cmdargs
end
say

unit = ssw_OpenUnit( )

say "Unit      =" unit
say "TestUnit  =" ssw_TestUnit( unit )
say "UnitInfo  =" ssw_GetUnitInfo( unit );
say
do channel = CHAN_IN_1_L to CHAN_MASTER_R
	gain = ssw_GetChannelGain( unit, channel );
	trim = ssw_GetChannelTrim( unit, channel );
	say "Channel" channel " Gain:" gain " Trim:" trim
end
say

call ssw_SetChannelGain unit CHAN_IN_1_L GAIN_DB_UNITY /* 0.0 dB */
call ssw_SetChannelGain unit CHAN_IN_1_R GAIN_DB_UNITY /* 0.0 dB */

call ssw_SetChannelGain unit CHAN_MASTER_L GAIN_DB_UNITY
call ssw_SetChannelGain unit CHAN_MASTER_R GAIN_DB_UNITY

say "SetMode   =" ssw_SetMode( unit, SSWMODEF_STEREO_LOCK, 0 )

call TIME('R')

do db = GAIN_DB_OFF to GAIN_DB_POS_12 by 1
	call ssw_SetChannelGain unit CHAN_MASTER_L db
	call ssw_SetChannelGain unit CHAN_MASTER_R db
end

do db = GAIN_DB_POS_12 to GAIN_DB_NEG_24 by -1
	call ssw_SetChannelGain unit CHAN_MASTER_L db
	call ssw_SetChannelGain unit CHAN_MASTER_R db
end

do db = GAIN_DB_NEG_24 to GAIN_DB_POS_12 by 1
	call ssw_SetChannelGain unit CHAN_MASTER_L db
	call ssw_SetChannelGain unit CHAN_MASTER_R db
end

do db = GAIN_DB_POS_12 to GAIN_DB_NEG_24 by -1
	call ssw_SetChannelGain unit CHAN_MASTER_L db
	call ssw_SetChannelGain unit CHAN_MASTER_R db
end

do db = GAIN_DB_NEG_24 to GAIN_DB_UNITY by 1
	call ssw_SetChannelGain unit CHAN_MASTER_L db
	call ssw_SetChannelGain unit CHAN_MASTER_R db
end

say "Elapsed   =" TIME('E')

say "SetMode   =" ssw_SetMode( unit, SSWMODEF_STEREO_LOCK, SSWMODEF_STEREO_LOCK )

call TIME('R')

do db = GAIN_DB_OFF to GAIN_DB_POS_12 by 1
	call ssw_SetChannelGain unit CHAN_MASTER_L db
end

do db = GAIN_DB_POS_12 to GAIN_DB_NEG_24 by -1
	call ssw_SetChannelGain unit CHAN_MASTER_L db
end

do db = GAIN_DB_NEG_24 to GAIN_DB_POS_12 by 1
	call ssw_SetChannelGain unit CHAN_MASTER_L db
end

do db = GAIN_DB_POS_12 to GAIN_DB_NEG_24 by -1
	call ssw_SetChannelGain unit CHAN_MASTER_L db
end

do db = GAIN_DB_NEG_24 to GAIN_DB_UNITY by 1
	call ssw_SetChannelGain unit CHAN_MASTER_L db
end

say "Elapsed   =" TIME('E')

say "SetMode   =" ssw_SetMode( unit, SSWMODEF_STEREO_LOCK, 0 )

/*
do samples = 0 to 128 by 1
	meter.samples	=	ssw_GetMeterInfo( unit, CHAN_MASTER_L );
	say meter.samples
end
*/

call ssw_SetChannelGain unit CHAN_MASTER_L GAIN_DB_UNITY /* 0.0 dB */
call ssw_SetChannelGain unit CHAN_MASTER_R GAIN_DB_UNITY /* 0.0 dB */

say "CloseUnit =" ssw_CloseUnit( unit )
say

