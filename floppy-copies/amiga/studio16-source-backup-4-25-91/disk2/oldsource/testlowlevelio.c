#if 0
void main()
{
struct Disk_Samp test_samp;
int i,j;

printf("test program\n");

test_samp.name="ram:test";

create_sample(&test_samp);
for (i=0; i < 1000; i++)
   put_samplel1(i);
close_sample(&test_samp);

open_sample(&test_samp);
for (i=0; i < 1000; i++)
   if (i!=(j=get_samplel1()))
      printf("samp mismatch %d %d\n",i,j);

init_sample1(&test_samp, 500);
init_sample2(&test_samp, 400);
for (i=500; i < 800; i++)
   if (i!=(j=get_samplel1()))
      printf("samp mismatch2 %d %d\n",i,j);

for (i=400; i < 800; i++)
   if (i!=(j=get_sample2()))
      printf("samp mismatch4 %d %d\n",i,j);

init_sample1(&test_samp, 500);
for (i=500; i < 800; i++)
   put_samplel1(i-100);

init_sample1(&test_samp, 500);
for (i=500; i < 800; i++)
   if (i!=(j=get_samplel1()+100))
      printf("samp mismatch3 %d %d\n",i,j);

close_sample(&test_samp);
printf("done\n");
}
#endif
