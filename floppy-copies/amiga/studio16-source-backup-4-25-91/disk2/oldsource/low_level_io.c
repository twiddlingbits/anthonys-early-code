#include "exec/types.h"
#include "fcntl.h"
#include "psound.h"

struct Disk_Samp *global1_ptr;
int global1_pos;

struct Disk_Samp *global2_ptr;
int global2_pos;

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

/****************************************/

short get_left_sample1(ptr, pos)

struct Disk_Samp *ptr;
int pos;

{
if (!is_block1_current(ptr, pos))
   if (update_block1(ptr, pos)!=NO_ERROR)
      return(0);
return(ptr->blockl1[pos-ptr->block1_start]);
}

/****************************************/

short get_left_sample2(ptr, pos)

struct Disk_Samp *ptr;
int pos;

{
if (!is_block2_current(ptr, pos))
   if (update_block2(ptr, pos)!=NO_ERROR)
      return(0);
return(ptr->blockl2[pos-ptr->block2_start]);
}

/****************************************/

short get_right_sample1(ptr, pos)

struct Disk_Samp *ptr;
int pos;

{
if (!is_block1_current(ptr, pos))
   if (update_block1(ptr, pos)!=NO_ERROR)
      return(0);
return(ptr->blockr1[pos-ptr->block1_start]);
}

/****************************************/

short get_right_sample2(ptr, pos)

struct Disk_Samp *ptr;
int pos;

{
if (!is_block2_current(ptr, pos))
   if (update_block2(ptr, pos)!=NO_ERROR)
      return(0);
return(ptr->blockr2[pos-ptr->block2_start]);
}

/****************************************/

short get_samplel1()
{
short val;

if (!is_block1_current(global1_ptr, global1_pos))
   if (update_block1(global1_ptr, global1_pos)!=NO_ERROR)
      return(0);
val = global1_ptr->blockl1[global1_pos-global1_ptr->block1_start];
global1_pos++;
return(val);
}

/****************************************/

short get_samplel2()
{
short val;

if (!is_block2_current(global2_ptr, global2_pos))
   if (update_block2(global2_ptr, global2_pos)!=NO_ERROR)
      return(0);
val = global2_ptr->blockl2[global2_pos-global2_ptr->block2_start];
global2_pos++;
return(val);
}

/****************************************/

short get_sampler1()
{
short val;

if (!is_block1_current(global1_ptr, global1_pos))
   if (update_block1(global1_ptr, global1_pos)!=NO_ERROR)
      return(0);
val = global1_ptr->blockr1[global1_pos-global1_ptr->block1_start];
global1_pos++;
return(val);
}

/****************************************/

short get_sampler2()
{
short val;

if (!is_block2_current(global2_ptr, global2_pos))
   if (update_block2(global2_ptr, global2_pos)!=NO_ERROR)
      return(0);
val = global2_ptr->blockr2[global2_pos-global2_ptr->block2_start];
global2_pos++;
return(val);
}

/****************************************/

int put_samplel1(val)

short val;

{
int index;
int error;

error = NO_ERROR;
if (!is_block1_current(global1_ptr, global1_pos))
   error=update_block1(global1_ptr, global1_pos);
index = global1_pos-global1_ptr->block1_start;
global1_ptr->blockl1[index]=val;
global1_ptr->flags |= DIRTY1;
global1_pos++;
return(error);
}

/****************************************/

int put_sampler1(val)

short val;

{
int index,error;

error=NO_ERROR;
if (!is_block1_current(global1_ptr, global1_pos))
   error = update_block1(global1_ptr, global1_pos);
index = global1_pos-global1_ptr->block1_start;
global1_ptr->blockr1[index]=val;
global1_ptr->flags |= DIRTY1;
global1_pos++;
return(error);
}


/****************************************/

int update_block1(ptr, pos)

struct Disk_Samp *ptr;
int pos;

{
int len;
int x;
/* printf("update1 %s %d\n",ptr->name,pos); */
if (ptr->flags&DIRTY1) {
   ptr->flags &= (~DIRTY1);
   lseek(ptr->lfp, ptr->block1_start*2, 0);
   if (write(ptr->lfp, (char *)ptr->blockl1, BLOCK_SIZE*2)!=BLOCK_SIZE*2)
      return(WRITE_ERROR);
   if (ptr->type==STEREO) {
      lseek(ptr->rfp, ptr->block1_start*2, 0);
      if (write(ptr->rfp, (char *)ptr->blockr1, BLOCK_SIZE*2)!=BLOCK_SIZE*2)
         return(WRITE_ERROR);
      }
   }
ptr->block1_start = pos;
/*printf("block1start %d\n",ptr->block1_start); */
lseek(ptr->lfp, ptr->block1_start*2, 0);
len=(ptr->length - ptr->block1_start)*2;
/*printf("update1: len %d\n",len);*/
x=read(ptr->lfp, (char *)ptr->blockl1, BLOCK_SIZE*2);
if (!(x>=len || x==BLOCK_SIZE*2)) {
/*printf("update1: x %d\n",x);*/
   return(READ_ERROR);
   }

if (ptr->type==STEREO) {
   lseek(ptr->rfp, ptr->block1_start*2, 0);
   x=read(ptr->rfp, (char *)ptr->blockr1, BLOCK_SIZE*2);
   if (!(x>=len || x==BLOCK_SIZE*2))
      return(READ_ERROR);
   }
return(NO_ERROR);
}

/****************************************/

int update_block2(ptr, pos)

struct Disk_Samp *ptr;
int pos;

{
int len,x;
/* printf("update2 %d\n",pos); */
ptr->block2_start = pos;
lseek(ptr->lfp, ptr->block2_start*2, 0);
len = (ptr->length - ptr->block1_start)*2;
x=read(ptr->lfp, (char *)ptr->blockl2,BLOCK_SIZE*2);
if (!(x>=len || x==BLOCK_SIZE*2))
   return(READ_ERROR);

if (ptr->type==STEREO) {
   lseek(ptr->rfp, ptr->block2_start*2, 0);
   x=read(ptr->rfp, (char *)ptr->blockr2, BLOCK_SIZE*2);
   if (!(x>=len || x==BLOCK_SIZE*2))
      return(READ_ERROR);
   }
return(NO_ERROR);
}

/****************************************/

int is_block1_current(ptr, pos)
struct Disk_Samp *ptr;
int pos;
{

return(pos >= ptr->block1_start && pos < (ptr->block1_start+BLOCK_SIZE));
}

/****************************************/

int is_block2_current(ptr, pos)
struct Disk_Samp *ptr;
int pos;
{

return(pos >= ptr->block2_start && pos < (ptr->block2_start+BLOCK_SIZE));
}

/****************************************/

int init_sample1(ptr, pos)
struct Disk_Samp *ptr;
int pos;
{
int error;

error=NO_ERROR;
if (!is_block1_current(ptr, pos))
   error=update_block1(ptr, pos);

global1_ptr = ptr;
global1_pos = pos;
return(error);
}

/****************************************/

int init_sample2(ptr, pos)
struct Disk_Samp *ptr;
int pos;
{
int error;

error=NO_ERROR;
if (!is_block2_current(ptr, pos))
   error=update_block2(ptr, pos);

global2_ptr = ptr;
global2_pos = pos;
return(error);
}

/****************************************/

int create_sample(ptr)
struct Disk_Samp *ptr;
{
int i;
char buf[80];
                   /** need to decide on error handeling & **/
sprintf(buf,"ram:%s.%03dL",ptr->name,ptr->version);
ptr->lfp = creat(buf, 0);
printf("low_level_io.c: creating: %s\n",buf);
if (ptr->lfp==-1) {
   printf("can't open file (left)\n");
   return(CANT_OPEN_FILE);
   }

if (ptr->type==STEREO) {
   sprintf(buf,"ram:%s.%03dR",ptr->name,ptr->version);
   ptr->rfp = creat(buf, 0);
   printf("LOW_LEVEL_IO.C: creating: %s\n",buf);
   if (ptr->rfp==-1) {
      printf("can't open file (right)\n");   /**!!!!!!!!!!!!!!**/
      close(ptr->lfp);
      return(CANT_OPEN_FILE);
      }
   }


ptr->block1_start = 0;
ptr->block2_start = 0;
ptr->flags = SAMPLE_OPEN;
for (i=0; i < BLOCK_SIZE; i++) {
   ptr->blockl1[i]=0;
   ptr->blockl2[i]=0;
   ptr->blockr1[i]=0;
   ptr->blockr2[i]=0;
   }
global1_ptr = ptr;
global1_pos = 0;

global2_ptr = ptr;
global2_pos = 0;
return(NO_ERROR);
}

/****************************************/

int open_sample(ptr)
struct Disk_Samp *ptr;
{
int error;

if (ptr->type==STEREO) printf("STEREO DOESNT WORK IN OPEN YET!!\n");

ptr->lfp = open(ptr->name, O_RDWR, 0);
if (ptr->lfp==-1)
   return(CANT_OPEN_FILE);
ptr->block1_start = 0;
ptr->block2_start = 0;
ptr->flags |= SAMPLE_OPEN;
error=update_block1(ptr,0);
global1_ptr = ptr;
global1_pos = 0;
return(error);
}

/****************************************/

int close_sample(ptr)
struct Disk_Samp *ptr;
{
int error;

if (ptr->flags&DIRTY1) {
   error=update_block1(ptr, 0); /*!!should add a flush() and change this */
   if (error!=NO_ERROR)
      return(error);
   }
if (close(ptr->lfp)!=0)
   return(DISK_ERROR);
if (ptr->type==STEREO)
   if (close(ptr->rfp)!=0)
      return(DISK_ERROR);
ptr->block1_start = -1;
ptr->block2_start = -1;
ptr->version = 0;
global1_ptr = 0;
global1_pos = -1;
if (!(ptr->flags&KEEP_NAME))
   FreeMem(ptr->name,strlen(ptr->name)+1);
ptr->flags = 0;
return(NO_ERROR);
}

