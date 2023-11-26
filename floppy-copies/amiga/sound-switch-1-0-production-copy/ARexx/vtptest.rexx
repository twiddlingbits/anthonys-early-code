/* SoundSwitch / Video Toaster Patch Rexx Interface Test Program
 */

address SOUNDSWITCH

options results

say "SoundSwitch / Video Toaster Patch Rexx Interface Test Suite"
say "-----------------------------------------------------------"

say
say "---> RESET"
RESET

say
say "---> VERSION"
VERSION
say RESULT

say
say "REXX commands"
say "-------------"
HELP
cmds = RESULT;
do until cmds = ""
	parse var cmds nextcmd cmds
	HELP nextcmd
	cmdargs = RESULT
	say LEFT( nextcmd, 24 ) cmdargs
end

say
say "AUTOSWITCH settings"
say "-------------------"
GETAUTOSWITCH
say RESULT

say
say "AUTOMAP settings"
say "-------------------"
GETAUTOMAP
say RESULT

say
say "EFFECT settings"
say "-------------------"
GETEFFECT MAIN PREVIEW
say RESULT

say
say "GAIN settings"
say "-------------------"
GETGAIN
say RESULT

say
say "TRIM settings"
say "-------------------"
GETTRIM
say RESULT

say
say "PEAK levels"
say "-------------------"
GETPEAK
say RESULT

say
say "---> TAKE from IN1 to IN2"
TAKE MAIN IN1 PREVIEW IN2
address command "wait 3 secs"

say
say "GAIN settings"
say "-------------------"
GETGAIN
say RESULT

say
say "---> TAKE it back..."
TAKE
address command "wait 3 secs"

say
say "GAIN settings"
say "-------------------"
GETGAIN
say RESULT

say
say "---> SETAUTOMAP V1 IN4 V2 IN3 V3 IN2 V4 IN1 DV1 IN2 DV2 IN3 DV3 OUT"
SETAUTOMAP V1 IN4 V2 IN3 V3 IN2 V4 IN1 DV1 IN2 DV2 IN3 DV3 OUT

say
say "AUTOMAP settings"
say "-------------------"
GETAUTOMAP
say RESULT

say
say "---> SETGAIN IN1 168 IN2 168 IN3 168 IN4 168 OUT 168"
SETGAIN IN1 168 IN2 168 IN3 168 IN4 168 OUT 168
address command "wait 2 secs"

say
say "GAIN settings"
say "-------------------"
GETGAIN
say RESULT

say
say "---> AUTO from V1 to V2 in .5 seconds"
AUTO MAIN V1 PREVIEW V2 MICROS 500000
address command "wait 1 secs"

say
say "GAIN settings"
say "-------------------"
GETGAIN
say RESULT

say
say "---> AUTO back in .5 seconds"
AUTO MICROS 500000
address command "wait 1 secs"

say
say "GAIN settings"
say "-------------------"
GETGAIN
say RESULT

say
say "---> RESET"
RESET


say
say "---> SETAUTOSWITCH V1 OFF"
SETAUTOSWITCH V1 OFF
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH V2 OFF"
SETAUTOSWITCH V2 OFF
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH V3 OFF"
SETAUTOSWITCH V3 OFF
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH V4 OFF"
SETAUTOSWITCH V4 OFF
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH DV1 OFF"
SETAUTOSWITCH DV1 OFF
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH DV2 OFF"
SETAUTOSWITCH DV2 OFF
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH DV3 OFF"
SETAUTOSWITCH DV3 OFF
address command "wait 1 secs"


say
say "AUTOSWITCH settings"
say "-------------------"
GETAUTOSWITCH
say RESULT

say
say "---> SETAUTOSWITCH V1 ON"
SETAUTOSWITCH V1 ON
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH V3 ON"
SETAUTOSWITCH V3 ON
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH V2 ON"
SETAUTOSWITCH V2 ON
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH DV1 ON"
SETAUTOSWITCH DV1 ON
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH V4 ON"
SETAUTOSWITCH V4 ON
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH DV2 ON"
SETAUTOSWITCH DV2 ON
address command "wait 1 secs"

say
say "---> SETAUTOSWITCH DV3 ON"
SETAUTOSWITCH DV3 ON
address command "wait 1 secs"


say
say "AUTOSWITCH settings"
say "-------------------"
GETAUTOSWITCH
say RESULT

say
say "---> AUTO from IN1 to IN2 in 3 seconds"
AUTO MAIN IN1 PREVIEW IN2 SECS 3
address command "wait 1 secs"

say
say "GAIN settings"
say "-------------------"
GETGAIN
say RESULT

say
say "---> AUTO back in 6 seconds"
AUTO SECS 6
address command "wait 1 secs"

say
say "GAIN settings"
say "-------------------"
GETGAIN
say RESULT

say
say "---> AUTO from IN1 to IN2 in .5 seconds"
AUTO MAIN IN1 PREVIEW IN2 MICROS 500000
address command "wait 1 secs"

say
say "GAIN settings"
say "-------------------"
GETGAIN
say RESULT

say
say "---> AUTO back in .5 seconds"
AUTO MICROS 500000
address command "wait 1 secs"

say
say "GAIN settings"
say "-------------------"
GETGAIN
say RESULT




say
exit

