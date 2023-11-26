**********************************************************************
*
* City Desk 2.0 Gray Scale Conversion Driver
*
**********************************************************************
*   csect      printer

*------ Included Files -----------------------------------------------

endc:macro
    endif
    endm

   INCLUDE      "aw:asmpro/includes/exec/types.i"
   INCLUDE      "aw:asmpro/includes/exec/nodes.i"
   INCLUDE      "aw:asmpro/includes/exec/strings.i"
   INCLUDE      "aw:cd/driver/include/devices/prtbase.i"

*------ Imported Names -----------------------------------------------
    data

   XREF      _Init
   XREF      _Expunge
   XREF      _Open
   XREF      _Close
   XREF      _CommandTable
   XREF      _PrinterSegmentData
   XREF      _DoSpecial
   XREF      _Render
   XREF      _ExtendedCharTable
*------ Exported Names -----------------------------------------------
    XDEF      _PEDData

**********************************************************************

      MOVEQ   #0,D0      ; show error for OpenLibrary()
      RTS

      data

      DC.W   35          ; for 1.3 driver
      DC.W   1

_PEDData:

      DC.L   printerName
      DC.L   _Init
      DC.L   _Expunge
      DC.L   _Open
      DC.L   _Close
      DC.B   PPC_BWGFX   ; PrinterClass
      DC.B   PCC_BW      ; ColorClass
      DC.B   0      ; MaxColumns
      DC.B   0      ; NumCharSets
      DC.W   1      ; NumRows
      DC.L   600      ; MaxXDots
      DC.L   795      ; MaxYDots
      DC.W   75      ; XDotsInch
      DC.W   75      ; YDotsInch
      DC.L   _CommandTable   ; Commands
      DC.L   _DoSpecial
      DC.L   _Render
      DC.L   30
      DC.L   _ExtendedCharTable   ; 8BitChars
      DS.L   1   ; PrintMode (reserve space)
      DC.L   0   ; ptr to char conversion function

printerName:

      STRING   <'City_Desk'>

      END
