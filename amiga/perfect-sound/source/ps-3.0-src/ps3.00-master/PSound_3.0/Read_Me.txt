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

