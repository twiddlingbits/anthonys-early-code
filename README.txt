awBASIC  
version 1.0 source 
released June 20, 2004

awBASIC is a complete BASIC interpreter that is compatible with Microsoft TRS-80 Level II 
BASIC circa 1979.  It emulates not only the statements, but also the bugs, TRS-80 graphics, 
keyboard, etc.  As a result, it will run thousands for basic programs from this era.  I tested about 
100 different ones (that’s how I found all the bugs.  Lots of these programs either take advantage 
of the bugs, or have subtle syntax errors that I had to explicitly allow).

The TRS-80 Level II BASIC Reference Manual is the reference text for the syntax of this 
BASIC.   It used to be available on the net, try a google search for “Level II BASIC Reference 
Manual”.  I may post it to my site.

This ZIP file contains the source code and is released under the GPL.

This version was built using Microsoft Visual C++ version 6.0 with the latest available service 
packs.

If you make modifications to this version of basic, as a courtesy I request that you call it awbasic, 
and print “awBASIC © 2002 Anthony Wood” on the console, or as appropriate, when the 
program first runs.

Although this basic is designed to emulate the original TRS-80 Level II basic, it is written in C 
and could easily be made into a command line BASIC or a basic for another platform.

If you decide to modify this code, here are some hints:
1.	I used .cpp files, but the code is mostly ANSI C.   If you convert to .c files (which I have 
in an unfinished subsequent version that I have not released), you will have to do things 
like add missing “struct” statements, perhaps change to C style comments, etc.  Things 
allowed in C++ syntax, but not C.
2.	All of the Windows specific code are in files that begin with “win”.  There’s three of 
them, and converting these, will convert to a new platform.
3.	The function WinTrsDelay() deliberately slows down execution to a close proximity of a 
1979 TRS-80.  If you want it to zip along at full speed, modify this function or remove 
the call to it.
4.	The graphics console is deliberately limited to TRS-80 resolution (128 x 48 pixels).  It 
would be easy to make it higher resolution.

Anthony Wood
Palo Alto, CA

My email address tends to change.  Check out www.awsoftware.org.  As of this writing, my 
email address that I use is at work, rokulabs.com.  My email address can be found on the roku 
web site.

