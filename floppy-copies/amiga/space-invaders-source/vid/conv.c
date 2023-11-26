
void main(argc, argv)
int argc;
char **argv;
{
unsigned char val;
int in, out, i,j,k;
static unsigned char buf[64000];
static unsigned char p2[] = {128, 64, 32, 16, 8, 4, 2, 1};

if (argc < 2) {
    printf("Usage from CLI: 'conv filename '\n");
    exit(0);
    }

in = open(argv[1], 0);
out = creat("out",0);
if (in == 0 || out == 0) {
   printf("Can't open a file.\n");
   exit(0);
   }

read(in, buf, 32000);

for (j=0; j< 200; j++) {
   for (i=0; i < 320; i++) {
      val = 0;
      for (k=3; k>=0; k--) {
         val = val <<1;
         if (buf[8000*k + j*40 + (i>>3)]&p2[i&7])
            val = val+16;
         }
      write(out, &val, 1);
      }
   }
close(in);
close (out);
}
