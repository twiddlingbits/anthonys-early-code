Perfect Sound Ver 3.10 Rel 05/21/90
===================================

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

Paste & Mix
-----------

Both the paste and mix tools mix the samples from left to right.  This can
cause a recursive effect when you are mixing into the middle of a marked
range.  For example, load the demo2 sound. Mark the entire sample.  Next,
select the mix menu and move the mix marker about one quarter of
the way through the word "welcome".  Select the mix menu again to perform
the mix.  When the mix is complete, you will hear alot of distortion.
This is becuase you are mixing into the marked range.  To avoid the
distortion, make a copy of the original sound (copy range to slot)
and then mix into the copy.

However, if you perform the above mix with the paste button, you get an
effect very much like someone talking in a big room.  The effect is
often better than a straight mix.

Installation on a hard disk
---------------------------

Drag the entire PSound_3.0 drawer over to your hard disk.  In addition,
you need to have the file "arp.library" in your libs: drawer.  If you
don't have the arp library already installed, run the shell and type:

copy PSound_3.0:libs/arp.library libs:
endcli

Release 3.01 Feb 4, 1990
========================

Version 3.01 fixes several bugs that existed in version 3.0.  If you find
any bugs and wish to see them fixed for the next release, please report
them to SunRize Industries at (409) 846-1311 or the PO Box in your manual.

Release 3.01 now includes a Public Domain drawer.  In this directory you
will find programs that are NOT supported by SunRize Industries, but were
selected because of their high quality or usefulness.  Included in this
drawer is a small program that plays IFF files in mono or stereo.
 
At the time of this release, AudioMaster did not work directly with the 3.0
Perfect Sound hardware.  You can load files created with Perfect Sound into
AudioMaster, but you can't digitize directly.  However, Oxxi has informed us
that they are planning to release an Audiomaster upgrade that will support
the new hardware sometime "before summer".

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

