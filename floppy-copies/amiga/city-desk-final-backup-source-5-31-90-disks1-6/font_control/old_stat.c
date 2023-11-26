

void stat(w,s)


char *s;

{
Move(w->RPort,10,w->Height-10);
Text(w->RPort,s,strlen(s));
ScrollRast(w->RPort, 0, 9, 0,0, w->Width-1, w->Height-1);
}
