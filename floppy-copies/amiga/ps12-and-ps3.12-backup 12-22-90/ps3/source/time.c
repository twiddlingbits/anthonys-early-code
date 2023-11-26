

main()
{
float val, total, t1,t2;
float h1,h2;
float s1,s2;

h1 = 1;
total = 0;
while(h1) {
   printf("Enter two times: ");
   scanf("%f,%f",&t1,&t2);

   h1 = (int)t1;
   h2 = (int)t2;
   s1 = (t1-h1)*100.0/60.0;
   s2 = (t2-h2)*100/60.0;
   if (h2 < h1) h2=h2+12;
   val = h2-h1+s2-s1;
   total = total + val;
   printf("time: %f total: %f\n",val,total);
   }
}



