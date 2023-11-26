
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>

menus(p)
struct MenuItem **p;
{

static struct IntuiText IText1 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   19,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Overwrite",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct MenuItem MenuItem1 = {
   NULL,   /* next MenuItem static structure */
   0,0,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   91,8,   /* hit box width and height */
   CHECKIT+ITEMTEXT+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText1,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   NULL,   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct Menu Menu3 = {
   NULL,   /* next Menu static structure */
   146,0,   /* XY origin of Menu hit box relative to screen TopLeft */
   102,0,   /* Menu hit box width and height */
   MENUENABLED,   /* Menu flags */
   "Parameters",   /* text of Menu name */
   &MenuItem1   /* MenuItem linked list pointer */
};

static struct IntuiText IText2 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Delete",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct MenuItem MenuItem5 = {
   NULL,   /* next MenuItem static structure */
   0,27,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   80,8,   /* hit box width and height */
   ITEMTEXT+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText2,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   NULL,   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText3 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Paste",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct MenuItem MenuItem4 = {
   &MenuItem5,   /* next MenuItem static structure */
   0,18,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   80,8,   /* hit box width and height */
   ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText3,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   'P',   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText4 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Copy",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct MenuItem MenuItem3 = {
   &MenuItem4,   /* next MenuItem static structure */
   0,9,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   80,8,   /* hit box width and height */
   ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText4,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   'C',   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText5 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Cut",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct MenuItem MenuItem2 = {
   &MenuItem3,   /* next MenuItem static structure */
   0,0,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   80,8,   /* hit box width and height */
   ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText5,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   'K',   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct Menu Menu2 = {
   &Menu3,   /* next Menu static structure */
   82,0,   /* XY origin of Menu hit box relative to screen TopLeft */
   57,0,   /* Menu hit box width and height */
   MENUENABLED,   /* Menu flags */
   "Edits",   /* text of Menu name */
   &MenuItem2   /* MenuItem linked list pointer */
};

static struct IntuiText IText6 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Quit",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct MenuItem MenuItem11 = {
   NULL,   /* next MenuItem static structure */
   0,45,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   80,8,   /* hit box width and height */
   ITEMTEXT+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText6,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   NULL,   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText7 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct MenuItem MenuItem10 = {
   &MenuItem11,   /* next MenuItem static structure */
   0,36,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   80,8,   /* hit box width and height */
   ITEMTEXT+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText7,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   NULL,   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText8 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Print",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct MenuItem MenuItem9 = {
   &MenuItem10,   /* next MenuItem static structure */
   0,27,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   80,8,   /* hit box width and height */
   ITEMTEXT+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText8,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   NULL,   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText9 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Save As...",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct MenuItem MenuItem8 = {
   &MenuItem9,   /* next MenuItem static structure */
   0,18,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   80,8,   /* hit box width and height */
   ITEMTEXT+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText9,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   NULL,   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText10 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Open",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct MenuItem MenuItem7 = {
   &MenuItem8,   /* next MenuItem static structure */
   0,9,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   80,8,   /* hit box width and height */
   ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText10,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   'O',   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct IntuiText IText11 = {
   3,1,COMPLEMENT,   /* front and back text pens, drawmode and fill byte */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "New",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct MenuItem MenuItem6 = {
   &MenuItem7,   /* next MenuItem static structure */
   0,0,   /* XY of Item hitbox relative to TopLeft of parent hitbox */
   80,8,   /* hit box width and height */
   ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,   /* Item flags */
   0,   /* each bit mutually-excludes a same-level Item */
   (APTR)&IText11,   /* Item render  (IntuiText or Image or NULL) */
   NULL,   /* Select render */
   'N',   /* alternate command-key */
   NULL,   /* SubItem list */
   MENUNULL   /* filled in by Intuition for drag selections */
};

static struct Menu Menu1 = {
   &Menu2,   /* next Menu static structure */
   0,0,   /* XY origin of Menu hit box relative to screen TopLeft */
   75,0,   /* Menu hit box width and height */
   MENUENABLED,   /* Menu flags */
   "Project",   /* text of Menu name */
   &MenuItem6   /* MenuItem linked list pointer */
};

*p = &MenuItem1;

return(&Menu1);
}
