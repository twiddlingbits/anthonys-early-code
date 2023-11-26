
#include <exec/types.h>
#include <fcntl.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <exec/interrupts.h>
#include <exec/exec.h>
#include <exec/errors.h>
#include <hardware/custom.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <devices/audio.h>
#include <graphics/gfxbase.h>
#include <graphics/copper.h>
#include <graphics/regions.h>
#include <graphics/gels.h>
#include <hardware/blit.h>
#include <devices/keymap.h>
#include <stdio.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <math.h>
#include <devices/serial.h>
#include <ctype.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/ports.h>
#include <exec/libraries.h>
#include <exec/devices.h>
#include <exec/io.h>
#include <devices/timer.h>
#define tmsg tr.tr_node.io_Message
#define STEREO 6
#define LEFT 2
#define RIGHT 4
