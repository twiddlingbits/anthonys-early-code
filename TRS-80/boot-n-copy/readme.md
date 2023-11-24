# Boot -n- Copy Introduction
Boot -n- Copy was designed to replace the boot sector on a floppy disk so that the floppy disk could boot without a DOS.  It would then copy files to a separate user disk that had TRS-DOS on it. 

A TRS-80 Floppy Disk Drive owner generally needed a floppy disk with the Disk Operating System (like TRS-DOS) in drive 0.  If you had two drives, you would keep TRS-DOS in drive 0, and your data disks in Drive 1.  Many owners of TRS-80s that had floppy disk drives had a single disk drive (they were expensive).   But you could not legally put TRS-DOS on a floppy disk that you sent to someone else, since TRS-DOS was copyrighted. This created a problem on how to legally distribute software on a floppy disk to TRS-80 owners that had a single disk drive.

To solve this issue, I wrote Boot -n- Copy in Z-80 assembly.  After assembling (transform the .ASM to the .CMD file), the binary was installed on the boot sector of a floppy.  I did this with a disk sector editor to create the master disk.   Boot -n- copy controls the floppy disk drive controller hardware directly.   It also understands enough about the TRS-80 disk file system to correctly add the file to a DOS formatted floppy disk.

I wrote boot-n-copy for use with my "AW Software" diskettes.


# Converting .ASM EDTASM to text files -- Notes to Self
These .ASM files saved by EDTASM appeared to be normal ascii text, except each line has a line number with the high bit set in each byte of the line number.   I used a windows hex editor to mask the high bit of the first 5 bytes of each line, which converts the line number to ascii.  I then used notepad++ to cut the line numbers, set the tabs to 8, and then converted the tabs to spaces.  I then then pasted the line numbers back into the start of the columns.  This was needed because edtasm appears to have set the tab stops to start after the line numbers.  



