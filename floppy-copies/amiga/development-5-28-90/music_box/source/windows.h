
struct my_Window {
   unsigned short x,y;
   unsigned short width,height;
   unsigned long flags;
   unsigned short clip_x, clip_y;
   unsigned short clip_w, clip_h;
   };

struct os_base {
   struct my_Window *first_window;
   }


