/*
 *	SoundSwitch Audio Mixer Library
 *	ARexx script to demonstrate the operation of the
 * soundswitch.library REXX function library interface.
 *
 * (C) Copyright 1994 SunRize Industries
 *	All Rights Reserved
 *
 *	$Id: soundswitch.rexx,v 1.0 1994/07/19 05:21:02 benfuller Exp $
 *
 */

/* Figure out what we have to do...
 */
PARSE UPPER ARG action arg1 arg2 arg3 .
OPTIONS RESULTS

/* Check if the soundswitch.library is on the list of
 * function libraries.  If it isn't, then add it.
 */
IF ~SHOW('L','soundswitch.library') THEN DO
   IF ~ADDLIB('soundswitch.library',1,-30) THEN DO
      SAY "Unable to add soundswitch.library to the function library list."
      EXIT
   END
END

/* If 'arg1' isn't set to anything, then set it to something
 * useful.  Since 'arg1' is most often used as a channel
 * specifier, set it to channel 8 (Left Master).
 */
IF arg1 == "" THEN arg1 = "8"

/* If 'arg2' isn't set to anything, then set it to something
 * useful.  Since 'arg2' is most often used as a level
 * specifier, set it to Unity Gain (0.0 dB).
 */
IF arg2 == "" THEN arg2 = "192"

/* Process the 'action' here --
 *
 * We will respond to these eight commands:
 *   GETGAIN <channel>  -- returns the gain setting for the channel.
 *   GETTRIM <channel>  -- returns the trim setting for the channel.
 *   GETMETERINFO <channel> -- returns a channel's current meter level.
 *   GETUNITINFO  -- returns information about the mixer.
 *   SETGAIN <channel> <level> -- used to change a channel's gain setting.
 *   SETTRIM <channel> <level> -- used to change a channel's trim setting.
 *   STEREO -- forces the SoundSwitch into "Stereo Lock" mode.
 *   MONO -- forces the SoundSwitch out of "Stereo Lock" mode.
 *
 * Basically what we do here is build a command string that will
 * be "interpreted" and perform the requested operation.
 *
 */
SELECT
   WHEN action == "GETGAIN" THEN DO
      operation  = "'ssw_GetChannelGain'"
      parameters = arg1
      rvaldesc   = "Gain:"
   END

   WHEN action == "GETTRIM" THEN DO
      operation  = "'ssw_GetChannelTrim'"
      parameters = arg1
      rvaldesc   = "Trim:"
   END

   WHEN action == "GETMETERINFO" THEN DO
      operation  = "'ssw_GetMeterInfo'"
      parameters = arg1 "," arg2
      rvaldesc   = "Meter:"
   END

   WHEN action == "GETUNITINFO" THEN DO
      operation  = "'ssw_GetUnitInfo'"
      parameters = ""
      rvaldesc   = "UnitInfo:"
   END

   WHEN action == "SETGAIN" THEN DO
      operation  = "'ssw_SetChannelGain'"
      parameters = arg1 "," arg2
      rvaldesc   = ""
   END

   WHEN action == "SETTRIM" THEN DO
      operation  = "'ssw_SetChannelTrim'"
      parameters = arg1 "," arg2
      rvaldesc   = ""
   END

   WHEN action == "STEREO" THEN DO
      operation  = "'ssw_SetMode'"
      parameters = "1, 1"
      rvaldesc   = ""
   END

   WHEN action == "MONO" THEN DO
      operation  = "'ssw_SetMode'"
      parameters = "1, 0"
      rvaldesc   = ""
   END

   OTHERWISE DO
      operation = ""
      SAY "Usage:"
      SAY "  rx soundswitch <action> <arg1> <arg2>"
   END
END

/* Check if we have something to do...
 */
IF operation ~= "" THEN DO

   /* Get access to the mixer...
    */
   unit = ssw_OpenUnit()

   /* Build the arguments part of the command...
    * if we have parameters, tack them on to the
    * end of the unit address.
    */
   IF parameters == "" THEN parameters = unit
   ELSE                     parameters = unit "," parameters

   /* Build the complete command line.
    */
   operation = "rval =" operation||"(" parameters ")"

   /* Execute the command and get the result.
    */
   INTERPRET operation

   /* If we were expecting a result, then show us what it was.
    */
   IF rvaldesc ~= "" THEN DO
      SAY rvaldesc rval
   END

   /* Release access to the mixer...
    */
   call ssw_CloseUnit unit
END

/* We're outta here.
 */
EXIT
