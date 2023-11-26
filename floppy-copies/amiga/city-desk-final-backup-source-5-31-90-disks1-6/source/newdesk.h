/** "object.type" definitions  **/

#define TEXT 0
#define GRAPHIC 1
#define LINE 2

/*** Text flags ***/
#define TEXT_PARSTART   1
#define TEXT_PAREND     2

/*** structure definitions ***/

typedef unsigned short COORD;

struct object {
   struct Node node;
   struct Image image;
   COORD x, y;
   COORD h, w;
   APTR  data;
   UWORD type;
   };

struct text_state {
   struct TextFont *font;
   COORD leading;
   COORD paragraph_indent;
   COORD paragraph_undent;
/* color, style, etc. */
};

struct text_object {
   struct object *next;
   struct text_line *line;
   char *textstart;
   char *textend;
   UWORD Flags;
   COORD height;
   struct text_state state, endstate;
};

struct text_line {
   struct text_line *next;
   char *text;
   COORD w, maxh, maxd;
   SHORT num_chars, num_spaces;
   };

