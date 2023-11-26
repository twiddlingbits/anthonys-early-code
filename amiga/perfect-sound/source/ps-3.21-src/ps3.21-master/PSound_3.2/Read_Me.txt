This file contains information that was not included in your manual.

See "Perfect Sound Ver 3.21 Release 04/10/91" below for a list of changes
new to version Perfect Sound 3.21.

16 Bit and 12 Bit Samplers
==========================

We have two new cards in development for the Amiga 2000/2500/3000.

AD1012
------

* 12 bit audio in and out
* 6th order linear phase anti-aliasing filters
* 21 different filter settings
* Sampling rates up to 100KHZ
* ADSP-2105 DSP rated at 10 MIPS 
* SMPTE time code reader

AD1016
------
* Stereo 16 bit audio in and out
* 8 times oversampling digital anti-aliasing filters
* Sampling rates up to 48KHZ
* DSP56001 DSP rated at 12.5 MIPS 
* MIDI in and out ports
* SMPTE time code reader
* AES/EBU and consumer digital in and out ports

Both cards include our new "Studio 16" multitrack recording and editing
software.  Both cards will record direct to hard disk.
Pricing and a release date have not been set. For more
information call us at the phone number at the end of this file.

AudioMaster III Compatibility
=============================

Your Perfect Sound 3 hardware has several enhancements that give it better
sound quality and make it easier to use than other sound samplers.  These
include:

* Digital gain
   This allows you to adjust the input gain with easy software controls.

* Hardware based sampling clock
   This allows Perfect Sound to time the intervals between samples exactly.
   This eliminates sample jitter present on all other software based
   samplers.

* Noise filters and voltage regulation
   These filter noise out of the Amiga's power supply to ensure clean
   recordings.

* Track and Hold
   Allows very accurate sampling of fast moving sound waves.

It was impossible to add the first two improvements and maintain
compatibility with all previously written software, including AudioMaster
II.  We decided, however, to go ahead and make the changes in order to
provide the best possible 8 bit sound sampling hardware.

The latest release of AudioMaster, version III, now supports Perfect
Sound 3.  Simply change the AudioMaster sampler setting in
Options-->Sampler Config to "PSound3".  When monitoring, you can adjust
the digital gain by holding down the right mouse button.  This will cause
the gain to slowly cycle.

Note: Any software that loads IFF sounds will load sounds sampled with
Perfect Sound.  Thus, you can use the older AudioMaster II by sampling a
sound with Perfect Sound first then loading it into AudioMaster II.

AudioMaster III is a software package produced by Oxxi of Long Beach, CA.
It provides a few extra editing features not present in the current release
of the Perfect Sound software.


Audition 4
==========

Audition 4 takes off where AudioMaster III left off. Available soon from
SunRize, Audition 4 gives you awesome 8 bit editing capabilities. Just a few
of Auditon's functions are:

1. Multiple loop points
2. High Pass, Low Pass, Band Pass, and Stop Band filters
3. Flange
4. Echoes and Delays
5. Preview effects before you pick them
6. Dynamic zoom and scroll
7. Freehand edit
8. Edit in zoom mode
9. Record direct to floppy disk
10. 100% Assembler for speed and memory efficiency
 
Perfect Sound Ver 3.21 Release 04/10/91
=======================================

1. Fixed bug that would cause the program to hang up after recording if you
   had an Amiga 2000 with a Commodore 2090(A) hard disk controller.  This
   bug would show up by either locking up when asking for a sample name,
   or locking while trying to save the sample.  In either case, you could
   unlock the Amiga by poping a disk out of your drive.
2. Now saves and restores the 8520 port A&B states upon entry/exit
3. Added a NAME chunk when saving IFF files so "Bars & Pipes" users can
   see sample name.

Perfect Sound Ver 3.2 Released 03/15/91
=======================================

1. Improved display when dragging
2. Improved graphing while monitoring.  The real time graph present when you
   monitor is now much faster and causes much less distortion.
3. The sample value at the start and end markers is now displayed
4. When setting the record rate, only viable rates are now shown.
5. IFF graphic file provided with description of buttons
6. Fixed a bug that caused the arp.library to be loaded, but not used.  When
   you are using the file requester, press the right mouse button to display
   a list of devices.
7. Added a new option in "Save" and "Save As..." The option, ASF, causes a
   sound to be saved as an executable file.  You can cause the sound to play
   by executing it. From a shell, you would type in the name of the sound.
   From workbench, you would double click on its icon.  Since Perfect Sound
   doesn't create a .info file (the icon) you will need to copy a .info file
   into the same directory as the sound file.  Make sure the icon you copy
   is a PROJECT icon.
8. Removed support for hardware other the Perfect Sound 3
9. Added support for German Language.  To select German, click on the
   Deutsch icon. For English, click on the English icon.
10.Added an icon to copy arp.library to libs: directory.  If you
   have dragged Perfect Sound over to your hard disk, double click
   the "copyARP" icon to install arp.library on our hard disk.
11.Fixed a bug that caused IFF saves to be incompatible with some programs

Perfect Sound Ver 3.11 Released 10/01/90
========================================

1. Should now work with '030 and '020 cards.  Tested with a A3000, A2500,
   and the 2630 '030 board.  However, psound.library has not been updated
   on this release.  If you need a version of this library compatible with
   '030 cards, contact us at the phone number below. Note: psound.library
   is not currently used by perfect sound.
2. Minor bug fixes
3. See the hard disk installation section below.  Your manual is wrong.

Perfect Sound Ver 3.10 Released 05/21/90
========================================

Additions:

1. Tech drawer contains psound.library for programmers
2. Tech drawer contains voice recognition demo for programmers
3. Real time echo and Real time delay functions added to editor.
   These functions have been added to the "other" menu.  To use, select
   the desired menu, and connect the source to be echoed or delayed to
   the left channel or the mic jack. The echo/delay options function similar
   to the monitor option, except they apply an echo or delay to the sound
   while you listen.  To exit, click the left mouse button.  The sampling
   rate used is fixed at about 14,000 samples per second. You enter the
   echo and delay amounts in "milliseconds" 1 millisec = 1/1000 sec. For
   example, a 500 millisec delay is the same as .5 seconds.
4. noisy.library added to Public Domain drawer allows BASIC programmers
   to load and play IFF sound files.
5. New DUMP format that saves marked range (see below for more info)
6. Miscellaneous minor improvements


Additions To Your Manual  Nov 18, 1989
======================================

Installation on a hard disk
---------------------------

Drag the entire PSound_3.2 drawer over to your hard disk.  In addition,
you need to have the file "arp.library" in your libs: drawer.  If you
don't have the arp library already installed double click the copyARP icon.


Release 3.01 Feb 4, 1990
========================

Version 3.01 fixes several bugs that existed in version 3.0.  If you find
any bugs and wish to see them fixed for the next release, please report
them to SunRize Industries at (408) 374-4962.

Release 3.01 now includes a Public Domain drawer.  In this directory you
will find programs that are NOT supported by SunRize Industries, but were
selected because of their high quality or usefulness.  Included in this
drawer is a small program that plays IFF files in mono or stereo.
 
DUMP FORMAT 3.1
===============

If you select "Save DUMP" or "Save As... DUMP" and the current sample has
a marked range, additional information is saved in the file.  The sampling
rate, start marker, and end marker are also save.  The file format is:

4 bytes  DUMP
4 bytes  # of samples (n)
4 bytes  sampling rate
4 bytes  start marker
4 bytes  end marker
n bytes  samples

OUR ADDRESS
===========

SunRize Industries
2959 S. Winchester Blvd., Suite 204
Campbell, CA 95008 USA
(408) 374-4962 (tel)
(408) 374-4963 (fax)
