extern struct Screen *is_screen;
extern struct Window *is_window;

show_title()
{

if (!load_iff_path("Img_Title.iff")) {
   telluser2(is_screen,"Can't load file 'Img_Title.iff' from",
   "the current directory.",0);
    closewindow(is_window);
    CloseScreen(is_screen);
   closelibraries();
   exit(0);
   }
}


