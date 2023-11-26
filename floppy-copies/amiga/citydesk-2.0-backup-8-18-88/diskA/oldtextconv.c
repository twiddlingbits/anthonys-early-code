/*###################################################################*/
/* textconv.c                                                        */
/*###################################################################*/

#include <exec/types.h>
#include <exec/exec.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include "desk.h"

#define SCRIBBLE_CENTER 0x0B
#define FF              0x0C
#define ANSI_ESCAPE     0x1B
#define ANSI_CSI        0x9B
#define WP_BOLD         0x9D
#define WP_NOBOLD       0x9C
#define WP_UNDER        0x94
#define WP_NOUNDER      0x95
#define WP_IT           0xB2
#define WP_NOIT         0xB3
#define WP_WRAP         0x0D
#define WP_ID           0x80
#define WP_CON          0xC3
#define WP_COFF         0x83
#define WP_HARD_HYPHEN1 0xA9  /** hard hyphen in line **/
#define WP_HARD_HYPHEN2 0xAA  /** hard hyphen at end of line **/
#define WP_HARD_HYPHEN3 0xAB  /** hard hyphen at end of page **/
#define WP_PAGEEND      0xDC
#define WP_LINEPAGE     0x8C  /** hard end of line soft page **/
#define WP_HARD_PAGE    0x0C
#define WP_SOFT_PAGE    0x0B

static char *dstart, *dp, *savedp;
static long dmax, dlen, savedlen;

extern SHORT strip_spaces;
extern SHORT double_newlines;
extern SHORT dot_commands;
extern SHORT scribble_style;
extern SHORT ansi_escapes;
extern SHORT real_ascii;
extern SHORT wp_style;

/*********************************************************************/
/* conv_scribble (src, srclen, dest, destlen);                       */
/*                                                                   */
/* Converts text from Scribble format into internal escape sequence  */
/* format.  Text is in src, with length of srclen.  Dest and destlen */
/* are returned.  An expandable buffer is used to accumulate the     */
/* converted text.  The original text is deallocated, then a chunk   */
/* of memory just big enough for the output is allocated, and the    */
/* output is copied from the big buffer to the destination area and  */
/* the big buffer is deallocated.                                    */
/*********************************************************************/

textconv (srcstart, srclen, destrtn, destlenrtn)
char *srcstart;
int srclen;
char **destrtn;
int *destlenrtn;
{
unsigned char *src, *savesrc;
char *end;
char *p1, *p2;
short dot_ok, ignore_spaces, centering;
long i;
char *AllocMem();
char style, old_style;

src = srcstart;
alloc_ct_dest (srclen);    /* Start with the same size dest buffer */
if (dmax == 0) {
   *destrtn = 0;
   *destlenrtn = 0;
   return(OUTOFMEM);
   }

append_char('\\');         /* Always start with a newline (start of par) */
append_char('n');

savedp = dp;
savedlen = dlen;
savesrc = src;
centering = FALSE;

dot_ok = TRUE;             /* dot command ok now */
ignore_spaces = TRUE;      /* don't start out with spaces */
end = src + srclen;

old_style = 0;

if (wp_style)
   src = src+2;      /** skip header info **/

while (src < end) {
   if (scribble_style) {
      if (scribble_special(*src))
         src++;                     /* skip scribble style command */
      if (scribble_special(*(src+1))) {   /* This char has a scribble */
        style = *(src+1);                 /* style cmd following it   */
         if (style != old_style) {
            if ((style ^ old_style) & 0x01) {      /* Bold */
               if (style&0x01)
                  append_string("[bd]");
               else
                  append_string("[nobd]");
            }
            if ((style ^ old_style) & 0x02) {      /* Underline */
               if (style&0x02)
                  append_string("[un]");
               else
                  append_string("[noun]");
            }
            if ((style ^ old_style) & 0x04) {      /* Italic */
               if (style&0x04)
                  append_string("[it]");
               else
                  append_string("[noit]");
            }
         }
         old_style = style;
      }
      else {
         if (old_style) {              /* went back to plain */
            append_string("[p]");
         }
         old_style = 0;
      }
   }

   switch (*src) {
   case '.':
      if (dot_ok && dot_commands) {
         if (process_dot_cmd(&src, end)) {
         append_char(*src++);      /** here if invalid dot cmd ***/
         ignore_spaces = FALSE;
         dot_ok = FALSE;
         }
      break;
      }

      append_char(*src++);
      ignore_spaces = FALSE;
      break;

   case ' ':
      if (!ignore_spaces || !strip_spaces)
         append_char(' ');
      src++;
      ignore_spaces = TRUE;
      while (*src == ' ') {         /* ignore more than one space */
         if (!strip_spaces)
            append_char(' ');
         src++;
      }
      break;
    case '\\':
      src++;
      if (*src != 'n') {
         dot_ok = FALSE;
         ignore_spaces = FALSE;
         append_char('\\');
         append_char(*src++);
         break;
      }                             /* else fall thru to newline */
    case '\n':
      dot_ok = TRUE;
      if (!double_newlines) {
         append_char('\\');
         append_char('n');
      }
      src++;
                                 /* two in a row mean paragraph break */
      if (*src == '\n' || (*src == '\\' && *(src+1) == 'n')) {
         while (*src == '\n' || (*src == '\\' && *(src+1) == 'n')) {
            append_char('\\');
            append_char('n');
            if (*src == '\n')
               src++;
            else
               src+=2;
         }
      }
      else {
         if (!ignore_spaces && double_newlines) {
            append_char(' ');
         }
      }
      ignore_spaces = TRUE;
      if (centering) {
         append_string("[nocj]");
         centering = FALSE;
      }
      savedp = dp;            /* save for Scribble's stupid centering */
      savedlen = dlen;
      savesrc = src;
      break;
   case ANSI_ESCAPE:
      if (ansi_escapes) {
         if (*(src+1) != '[')
            goto defalt;      /* not a valid escape seq */
         src++;
      }                       /* fall through */
      else
         goto defalt;
   case ANSI_CSI:
      if (ansi_escapes) {
         src++;
         process_ansi_esc(&src);
         break;
      }
      else
         goto defalt;

   case FF:                 /** Form Feed **/
      if (scribble_style) {
         src++;
         append_string("[newpage]");
         break;
      }
      else if (ansi_escapes) {     /** NotePad puts FF in file **/
         src++;
         break;
      }
      else if (wp_style) {       /** WP uses 0c as hard new page **/
         src++;
         append_string("\\n");
         break;
         }
      else {
         goto defalt;
      }

   case WP_WRAP:      /** I think wp uses a 0d as "soft new line" **/
      if (wp_style) {
         src++;
         append_char(32);
         break;
         }
      else {
         goto defalt;
         }

   case WP_BOLD:
      if (wp_style) {
         src++;
         append_string("[bd]");
         break;
         }
      else {
         goto defalt;
         }

   case WP_NOBOLD:
      if (wp_style) {
         src++;
         append_string("[nobd]");
         break;
         }
      else {
         goto defalt;
         }

   case WP_UNDER:
      if (wp_style) {
         src++;
         append_string("[under]");
         break;
         }
      else {
         goto defalt;
         }

   case WP_NOUNDER:
      if (wp_style) {
         src++;
         append_string("[nounder]");
         break;
         }
      else {
         goto defalt;
         }

   case WP_IT:
      if (wp_style) {
         src++;
         append_string("[it]");
         break;
         }
      else {
         goto defalt;
         }

   case WP_NOIT:
      if (wp_style) {
         src++;
         append_string("[noit]");
         break;
         }
      else {
         goto defalt;
         }

   case WP_CON:
      if (wp_style) {
         src=src+5;
         append_string("[cj]");
         break;
         }
      else {
         goto defalt;
         }

   case WP_COFF:
      if (wp_style) {
         src++;
         append_string("[nocj]");
         break;
         }
      else {
         goto defalt;
         }

   case WP_HARD_HYPHEN1:
   case WP_HARD_HYPHEN2:
   case WP_HARD_HYPHEN3:
      if (wp_style) {
         src++;
         append_char('-');
         break;
         }
      else {
         goto defalt;
         }

   case WP_PAGEEND:
      if (wp_style) {
         src = src + 8;
         while (*src != WP_PAGEEND)
            src++;

         src++;
         break;
         }
      else {
         goto defalt;
         }

   case WP_LINEPAGE:
      if (wp_style) {
         src++;
         append_string("\\n");
         break;
         }
      else {
         goto defalt;
         }

   case SCRIBBLE_CENTER:
      if (scribble_style) {   /* Center command is AFTER the line (grr)  */
         *src = '\n';         /* make it into a newline */
         src = savesrc;       /* Back up and redo the line */
         dp = savedp;
         dlen = savedlen;
         append_string("[cj]");
         centering = TRUE;
         break;
      }

      if (wp_style) {   /** soft new page in wp **/
         src++;
         append_char(32);
         break;
         }

                     /* else fall through to default */
   default:
   defalt:
      dot_ok = FALSE;
      ignore_spaces = FALSE;
      if (real_ascii) {             /* strip funny characters */
         if (((*src)&0x80) || *src==0x7f || *src<0x20)
            src++;                                       /* ha ha */
         else
            append_char(*src++);
      }
      else
         append_char(*src++);
      break;
   }
}

if (*(dp-1) != '\n' && (*(dp-2) != '\\' && *(dp-1) != 'n')) {
   append_char('\\');         /* end last paragraph */
   append_char('n');
}
append_char('\000');          /* end marker */

FreeMem (srcstart, srclen+1);
*destrtn = AllocMem (dlen, MEMF_CLEAR);
if (*destrtn != NULL) {
   *destlenrtn = dlen;
   p1 = *destrtn;
   p2 = dstart;
   for (i=0; i<dlen; i++)
      *p1++ = *p2++;
   FreeMem (dstart, dmax);
}
else {
   if (dmax == 0) {
      *destrtn = 0;
      *destlenrtn = 0;
      return(OUTOFMEM);
      }

   *destrtn = dstart;
   *destlenrtn = dmax;     /* extra should be 0, which should be ignored */
}
return(AOK);
}

/*********************************************************************/
/* process_dot_cmd(&sp, end);                                        */
/*                                                                   */
/* Processes a dot command pointed to by *sp.  Sp is returned        */
/* pointing after the newline at the end of the dot command.         */
/* if an invalid dot cmd is found, sp is not changed and TRUE is     */
/* returned. Else, FALSE (if AOK) is returned.                       */
/*********************************************************************/

process_dot_cmd(sp, end)
char **sp;
char *end;
{
char *p;
char cmd[80];
char ch;
int val, i;
COORD line_to_coord(), col_to_coord();

p = *sp;

while ((*p == '.' || *p == ',') && (p < end) && (*p != '\n')) {
   p++;
   i = 0;
   cmd[0] = '\0';
   while (*p != '=' && *p != '\n' && p < end) {
      if (!scribble_special(*p)) {
         cmd[i++] = *p;
         cmd[i] = '\0';
      }
      p++;
      if (i > 78)
         i = 78;
   }
   if (*p == '=')
      p++;                 /* skip (ignore) the = */

   if (strcmp(cmd, "lm") == 0) {
      val = get_num_dot_val(&p, end);
      append_string("[lm=");
      append_val(col_to_coord(val));
      append_char(']');
   }
   else if (strcmp(cmd, "rm") == 0) {
      val = get_num_dot_val(&p, end);
      append_string("[rm=");
      append_val(col_to_coord(val));
      append_char(']');
   }
   else if (strcmp(cmd, "hi") == 0) {
      val = get_num_dot_val(&p, end);
      append_string("[indent=");
      append_val(col_to_coord(val));
      append_char(']');
   }
   else if (strcmp(cmd, "ls") == 0) {
      val = get_num_dot_val(&p, end);
      append_string("[leading=");
      append_val(line_to_coord(val));
      append_char(']');
   }
   else if (strcmp(cmd, "ju") == 0) {
      ch = *p++;
      if (ch == 'y' || ch == 'Y')
         append_string("[fj]");
      else
         append_string("[lj]");
   }
   else if (strcmp(cmd, "fr") == 0) {
      ch = *p++;
      if (ch == 'y' || ch == 'Y')
         append_string("[rj]");
      else
         append_string("[lj]");
   }
   else if (strcmp(cmd, "ce") == 0) {
      ch = *p++;
      if (ch == 'y' || ch == 'Y')
         append_string("[cj]");
      else
         append_string("[lj]");
   }
   else {
/*      printf("invalid: %s\n",cmd); */
      return(TRUE);
   }
}

while (*p != '\n' && p < end)       /* skip what we don't understand */
   p++;
if (*p == '\n')
   p++;                       /* don't include the second line terminator */

*sp = p;
return(FALSE);
}


/*********************************************************************/
/* process_ansi_esc(&sp);                                            */
/*                                                                   */
/* Process an ANSI escape sequence pointed to by *sp.  Sp is         */
/* incremented to point past the escape sequence.                    */
/*********************************************************************/

process_ansi_esc(sp)
char **sp;
{
int val;
char buf[10];

val = 0;
while (isdigit(**sp) || **sp==' ') {
   if (**sp != ' ')
      val = val*10 + (**sp-'0');
   (*sp)++;
}
if (**sp == 'm') {
   (*sp)++;
   if (**sp == '\0')             /* ARRRGGHH!! Why does Notepad do this? */
      (*sp)++;
   if (val == 0)
      append_string("[p]");
   if (val == 3)
      append_string("[it]");
   if (val == 23)
      append_string("[noit]");
   if (val == 4)
      append_string("[un]");
   if (val == 24)
      append_string("[noun]");
   if (val == 1)
      append_string("[bd]");
   if (val == 22)
      append_string("[nobd]");
}
else if (**sp == ';') {             /* font change */
   append_string("[font ");
   while (**sp == ';') {
      append_char((char)val);
      val = 0;
      (*sp)++;
      while (isdigit(**sp) || **sp == ' ') {
         if (**sp != ' ')
            val = val*10 + (**sp-'0');
         (*sp)++;
      }
   }
   if (val) {
      sprintf(buf, " %d", val);
      append_string(buf);
   }
   append_char(']');
   if (**sp == '\"')             /* eat "f\0 terminator */
      (*sp)++;
   if (**sp == 'f')
      (*sp)++;
   if (**sp == '\0')    /* Notepad puts a null byte after a font change! */
      (*sp)++;          /* why in the HELL does notepad do this? */
}
}


/*********************************************************************/
/* scribble_special(ch);                                             */
/*                                                                   */
/* Returns TRUE if ch is a scribble special character (used to       */
/* indicate bold, italic, and underline), FALSE otherwise.           */
/*********************************************************************/

int scribble_special(ch)
UBYTE ch;
{

if (ch >= 0x81 && ch <= 0x87)
   return(TRUE);
return(FALSE);
}


/*********************************************************************/
/* val = get_num_dot_val (&p, end);                                  */
/*                                                                   */
/* Get a numeric value pointed to by *p                              */
/*********************************************************************/

int get_num_dot_val (pp, end)
char **pp;
char *end;
{
char *p;
int val;

p = *pp;
val = 0;

while (p < end && *p >= '0' && *p <= '9')
   val = val*10 + (*p++ - '0');

*pp = p;
return(val);

}


/*********************************************************************/
/* alloc_ct_dest (req_size);                                         */
/*                                                                   */
/* Allocates memory for the text conversion.  First try to allocate  */
/* req_size amount of memory.  If it doesn't fit, try 3/4 of         */
/* req_size, and so on until it gets below 32 bytes, at which point  */
/* just give up.                                                     */
/*********************************************************************/

alloc_ct_dest (req_size)
int req_size;
{
char *AllocMem();

dstart = AllocMem(req_size, MEMF_CLEAR);
while (dstart == NULL && req_size >= 32) {
   req_size = (req_size * 3) / 4;
   dstart = AllocMem(req_size, MEMF_CLEAR);
}

dmax = req_size;
if (dstart == NULL)
   dmax = 0;

dp = dstart;
dlen = 0;
}


/*********************************************************************/
/* append_string(ch);                                                */
/*                                                                   */
/* Appends a string to the end of the destination string.  If        */
/* necessary, it will allocate a larger chunk of memory.             */
/*********************************************************************/

append_string(str)
char *str;
{

for ( ; *str!='\0'; str++)
   append_char(*str);
}


/*********************************************************************/
/* append_char(ch);                                                  */
/*                                                                   */
/* Appends a character to the end of the destination string.  If     */
/* necessary, it will allocate a larger chunk of memory.             */
/*********************************************************************/

append_char(ch)
char ch;
{
char *olddstart, *p;
long olddmax, i;

if (dlen < dmax) {
   *dp++ = ch;
   dlen++;
}
else {
   olddstart = dstart;
   olddmax = dmax;
   alloc_ct_dest (olddmax * 2);        /* try a buffer twice as big */
   if (dmax > olddmax) {
      p = olddstart;
      savedp += (dp-p);
      for (i=0; i<olddmax; i++) {
         *dp++ = *p++;
         dlen++;
      }
      FreeMem (olddstart, olddmax);
      *dp++ = ch;
      dlen++;
   }
}
}


/*********************************************************************/
/* append_val(val);                                                  */
/*                                                                   */
/* Appends a (COORD) value to the end of the destination string.  If */
/* necessary, it will allocate a larger chunk of memory.             */
/*********************************************************************/

append_val(val)
COORD val;
{
char buf[10];

sprintf(buf, "%dpt", val>>3);
append_string(buf);

}


/*********************************************************************/
/* ans = col_to_coord(val);                                          */
/*                                                                   */
/* Converts an integer number of columns (val) to COORDS.  It        */
/* assumes the standard 10 chars to the inch, so the conversion is   */
/* coord = val(col) * .1(inch/col) * 72(points/inch) * 8(coord/pt)   */
/*       = val * 576 / 10                                            */
/*********************************************************************/

COORD col_to_coord(val)
int val;
{

return ((COORD) (val * 576 / 10));
}


/*********************************************************************/
/* ans = line_to_coord(val);                                         */
/*                                                                   */
/* Converts an integer number of lines (val) to COORDS.  It          */
/* assumes the standard 6 lines to the inch, so the conversion is    */
/* coord = val(line) * 1/6(inch/line) * 72(pt/inch) * 8(coord/pt)    */
/*       = val * 576 / 6 = val * 96                                  */
/*********************************************************************/

COORD line_to_coord(val)
int val;
{

return ((COORD) (val * 96));
}

