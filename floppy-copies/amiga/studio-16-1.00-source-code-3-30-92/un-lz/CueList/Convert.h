
int Sample2TimeCode(length,rate,smpte)
int length,rate,smpte;
{
int lot,time;
struct TimeCode *tc=(struct TimeCode *)&time;

lot=((length*smpte)/rate);
tc->frames=lot % smpte; lot=lot/smpte;
tc->seconds=lot % 60; lot=lot/60;
tc->minutes=lot % 60; lot=lot/60;
tc->hours=lot % 24;

return(time);
}
