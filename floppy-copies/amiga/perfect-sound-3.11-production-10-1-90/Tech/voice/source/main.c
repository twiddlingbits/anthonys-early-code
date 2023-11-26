#include "exec/types.h"
#include "psound.h"

/**  Speech Recongnition Copyright 1990 Anthony Wood
 **  Anthony J. Wood
 **  April 30, 1990  **/

#define ORDER 8         /** Number of LPC coef. to calc **/
                        /** will work as low as 1 **/
struct matrix {
   int rows;
   int cols;
   double val[ORDER+2][ORDER+2];   /** VAL [row][column] **/
   };

struct Frame {
   int num_frames;
   int length;
   double a[100][ORDER+1];       /** [frame #][A's for this frame] **/
   double r[100][ORDER+1];       /** [frame #][R's for this frame] **/
   };


struct Samp samp[MAX_SAMPLES];
struct Frame frames[MAX_SAMPLES];

void main()
{
void mult();
void build_rmat();
void calculate_frames();
double log();
double d,x;
int choice;
int num_learned, test_samp;
int i,j,k;
char fname[40];
struct matrix a,b,c,r,at,bt;

num_learned = 0;

while (1) {
   printf("\nWelcome to Anthony's Speech Recognition Experiment\n\n");
   printf("1. Learn an untterance from a file on disk\n");
   printf("2. Compare an utterance on disk to learned phrases (mthd 1)\n");
   printf("3. Compare an utterance on disk to learned phrases (mthd 2)\n");
   printf("4. Exit\n");

   scanf("%d\n",&choice);
   printf("\n");
   if (choice==4) {
      while (num_learned > 0)
         delete_sample(--num_learned);
      exit(0);
      }

   if (choice==3) {     /** test using d=ln(ARA/BRB) **/
      printf("Enter file name to compare: ");
      scanf("%s\n",fname);
      printf("Loading %s   ",fname);
      test_samp = num_learned;            /** unused sample structure **/
      if (load_file(fname,test_samp)==0) {
         printf("length: %d  ",samp[test_samp].length);
         printf("sampling rate: %d\n",samp[test_samp].rate);

/** search all previouslly learned for best match **/

         for (i=0; i<num_learned; i++) {
            d=0;

/** calculate A's and R's for each frame in the sample just loaded **/
/** The # of frames is warped to match the sample we are comparing to **/

            calculate_frames(test_samp, frames[i].num_frames);

/** Now compare the A's just calculated with every frame **/

            for (j=0; j < frames[i].num_frames; j++) {
               build_rmat(&frames[i], &r, j);   /** R matrix of b's samps */

/** calculate ARA(t) **/

               a.rows=1; a.cols=ORDER+1;
               at.cols=1; at.rows=ORDER+1;
               a.val[1][1]=1;
               at.val[1][1]=1;
               for (k=1; k<=ORDER; k++) {
                  a.val[1][k+1]=-frames[test_samp].a[j][k];
                  at.val[k+1][1]=a.val[1][k+1];
                  }
               mult(&a,&r,&c);
               mult(&c,&at,&a);  /** answer is in a[1][1] **/

/** do same thing for b... calculate BRB(t) **/

               b.rows=1; b.cols=ORDER+1;
               bt.cols=1; bt.rows=ORDER+1;
               b.val[1][1]=1;
               bt.val[1][1]=1;
               for (k=1; k<=ORDER; k++) {
                  b.val[1][k+1]=-frames[i].a[j][k];
                  bt.val[k+1][1]=b.val[1][k+1];
                  }
               mult(&b,&r,&c);
               mult(&c,&bt,&b);  /** answer is in b[1][1] **/

/* The following optional code will calculate BRB using R(0)-SUM(AR) **/
/*            x = r.val[1][1];                        */
/*            for (k=2; k <=(ORDER+1); k++)           */
/*               x=x-frames[i].a[j][k-1]*r.val[1][k]; */
/* printf("%f a:%f b:%f altb:%f\n",a.val[1][1]/b.val[1][1],a.val[1][1],b.val[1][1],x); */

/** Calc distance measure d=ln(ARA/BRB) **/

               d=d+log((double)(a.val[1][1]/b.val[1][1]));
               }
printf("i: %d %s d=%f d(norm)=%f\n",i,samp[i].name,d,d/frames[i].num_frames);
            }
         }
      else {
         printf("Error while loading file\n");
         }
      }

   if (choice==2) {     /** Calc d using d=(A-B)^2; **/
      printf("Enter file name to compare: ");
      scanf("%s\n",fname);
      printf("Loading %s   ",fname);
      test_samp = num_learned;
      if (load_file(fname,test_samp)==0) {
         printf("length: %d  ",samp[test_samp].length);
         printf("sampling rate: %d\n",samp[test_samp].rate);

/** search all previouslly learned samples for best match **/

         for (i=0; i<num_learned; i++) {
            d=0;

/** Calculate A's and R's for sample just loaded **/

            calculate_frames(test_samp,frames[i].num_frames);

/** Compare every frame of test sample to current trained sample **/

            for (j=0; j < frames[i].num_frames; j++) {
               for (k=1; k<=ORDER; k++) {
                  x = (frames[i].a[j][k]-frames[test_samp].a[j][k]);
                  d = d + x*x;
                  }
               }
            printf("i: %d %s d=%f d(norm)=%f\n",i,samp[i].name,d,d/frames[i].num_frames);
            }
         }
      else {
         printf("Error while loading file\n");
         }
      }

   if (choice==1) {
      printf("Enter file name to load: ");
      scanf("%s\n",fname);
      printf("Loading %s  ",fname);
      if (load_file(fname,num_learned)==0) {
         printf("length: %d  ",samp[num_learned].length);
         printf("sampling rate: %d\n",samp[num_learned].rate);
         printf ("Learning sample # %d...\n",num_learned+1);
         calculate_frames(num_learned,0);
         num_learned++;
         }
      else {
         printf("Error while loading file\n");
         }
      }
   }
}

/************************************************************************/
/** calcualte_frames                                                   **/
/**                                                                    **/
/** Given an index into the global array "samp", calculate R's and A's **/
/** for each frame in sample.  Put results into global array "frames". **/
/************************************************************************/

void calculate_frames(i,force_size)
int force_size;     /** non zero means force frame count to this value **/
int i;
{
void calc_frame();
int frame_cnt;
int framesize;
int k;

if (force_size==0) {
   frame_cnt=(50*samp[i].length)/samp[i].rate;     /** 20 milisec frames **/
   framesize=samp[i].length/frame_cnt;

   }
else {
   frame_cnt=force_size;     /** force frame count to value passed **/
   framesize=samp[i].length/frame_cnt;
   }

/* printf("%d frames of length %d samples\n",frame_cnt,framesize); */

frames[i].num_frames=frame_cnt;

for (k=0; k < frame_cnt; k++)      /** frame counter **/
   calc_frame(&samp[i].lmem[k*framesize],framesize,k,&frames[i]);
printf("\015   \015");
}

/************************************************/
/** cacl_frame                                 **/
/**                                            **/
/** Calcualte R's and A's for a single frame   **/
/************************************************/

void calc_frame(data, len, frame, frame_ptr)

char *data;
int len;
int frame;
struct Frame *frame_ptr;
{
void print_mat();
int i,k,j;
double sum;
double a[ORDER+1][ORDER+1];
double p[ORDER+1];
double err,olderr,x;

/*
for (j=0; j<len; j++)
   printf("%d, ",data[j]);
printf("\n");
*/

/** CALCULATE R's **/

for (j=0; j<=ORDER; j++) {
   sum=0;
   for (k=0; k < (len-j); k++) {
      sum=sum+data[k]*data[k+j];
      }
   frame_ptr->r[frame][j]=sum;
/* printf("r[%d]=%f\n",j,r[frame][j]); */
   }

/** Normalize R's to R(0) **/

/* for (j=0; j<=ORDER; j++)
   frame_ptr->r[frame][j] = frame_ptr->r[frame][j]/frame_ptr->r[frame][0]; */


/** CALCULATE P's and A's **/

err = frame_ptr->r[frame][0];
for(i=1; i <= ORDER; i++) {
   x=0;
   for(j=1; j <=(i-1); j++) {
      x=x+a[j][i-1]*frame_ptr->r[frame][i-j];
      }
   p[i]=(frame_ptr->r[frame][i]-x)/err;
   a[i][i]=p[i];
   for (j=1; j <= (i-1); j++) {
      a[j][i]=a[j][i-1]-p[i]*a[i-j][i-1];
      }
   olderr=err;
   err=(1-p[i]*p[i])*olderr;

/*   printf("err %f p %d = %f\n",err,i,p[i]);
   for (j=1; j <=i; j++)  {
      printf("a %d,%d=%f\n",j,i,a[j][i]);
      }       */
   }

/** PUT A's into frames structure **/

printf("%d\015",frame);
for (j=1; j <=ORDER; j++)  {
   frame_ptr->a[frame][j]=a[j][ORDER];
 /*  printf("'%f' ",a[j][ORDER]); */
   }
}

/************************************************/
/** mult                                       **/
/**                                            **/
/** given a pointer to marix a,b,c, calc C=AB  **/
/************************************************/

void mult(a,b,c)

struct matrix *a,*b,*c;

{
void print_mat();
int i,j,k;

if (a->cols != b->rows) {
   printf("can't multiply these two matrixes\n");
   return;
   }

c->rows=a->rows;
c->cols=b->cols;

for (i=1; i <= a->rows; i++) {
   for (j=1; j <= b->cols; j++) {
      c->val[i][j]=0;
      for (k=1; k <= a->cols; k++) {
         c->val[i][j] += a->val[i][k]*b->val[k][j];
         }
      }
   }
}

/*****************************************************/
/** print_mat                                       **/
/**                                                 **/
/** Given a pointer to a matrix, print its contents **/
/*****************************************************/

void print_mat(a)

struct matrix *a;

{
int i,j;

printf("%d X %d\n",a->rows,a->cols);
for (i=1; i<=a->rows; i++) {
   for (j=1; j <= a->cols; j++) {
      printf("%4.2f ",a->val[i][j]);
      }
   printf("\n");   /** octal C/R **/
   }
}

/*****************************************************/
/** build_rmat                                      **/
/**                                                 **/
/** take R's for a given frame, and put them into a **/
/** toplitz form in matrix "rmat"                   **/
/*****************************************************/

void build_rmat(frame_ptr, rmat, frame_num)

int frame_num;
struct Frame *frame_ptr;
struct matrix *rmat;
{
int i,j,k;

/** BUILD R TOPLITZ MATRIX **/

rmat->rows=ORDER+1;
rmat->cols=ORDER+1;

for (i=0; i <=ORDER; i++) {
   k=1;
   for (j=i; j > 0; j--)
      rmat->val[i+1][k++]=frame_ptr->r[frame_num][j];
   for (j=0; j <= ORDER-i; j++)
      rmat->val[i+1][k++]=frame_ptr->r[frame_num][j];
   }

/* print_mat(rmat);  */
}
