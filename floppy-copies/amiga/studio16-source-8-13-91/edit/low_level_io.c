#include <exec/types.h>
#include <exec/exec.h>
#include <fcntl.h>
#include "/include/psound.h"

struct Disk_Samp *global1_ptr;
int global1_pos;

struct Disk_Samp *global2_ptr;
int global2_pos;


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
if (global1_pos > global1_ptr->length)
   global1_ptr->length=global1_pos;
return(error);
}

/****************************************/

/****!!!!@@@@@@@ shouldn't these pointers be right!!!??? ****?*/
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
if (global1_pos > global1_ptr->length)
   global1_ptr->length=global1_pos;
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
x=read(ptr->lfp, (char *)ptr->blockl1, BLOCK_SIZE*2);
/*printf("update1: len %d x %d\n",len,x);*/
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
/** quick_dump - writes a block of data to a file pointed to
   by a sample ptr.  After calling this alot, you need to call
   update_block1(ptr, 0); to reset internal stuff
***/
quick_dump(ptr,data)
char *data;
struct Disk_Samp *ptr;
{
ptr->length += BLOCK_SIZE;
if (write(ptr->lfp, data, BLOCK_SIZE*2)!=BLOCK_SIZE*2)
   return(WRITE_ERROR);
else
   return(NO_ERROR);
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

/*############################################################
NAME
	create_sample -
FUNCTION
	allocates a Disk_Samp structure, links it to studioBase,
   and open's the required DOS files.  The sample name is copied
   so that the passed name's memory can be re-used by the calling
   program.
INPUTS
	name     pointer to sample name
	version  version number of this name
RESULTS
	returns an error code as described in studio.h
##############################################################*/

int create_sample(name, version, ptrptr)
struct Disk_Samp **ptrptr;
char *name;
int version;
{
extern struct studioBase *studioBase;
struct Disk_Samp *ptr;
char buf[80];
char temp[40];
                   /** need to decide on error handeling & **/

/*** ADD CHECKING SO THAT TWO SAMPS CAN'T HAVE SAMENAME!!**/
ptr = (struct Disk_Samp *)AllocMem(sizeof(struct Disk_Samp), MEMF_CLEAR);
if (ptr==NULL)
   return(OUT_OF_MEMORY);

ptr->name=(char *)AllocMem(strlen(name)+1,0);
if (ptr->name==0) {
   FreeMem(ptr, sizeof(struct Disk_Samp));
   return(OUT_OF_MEMORY);
   }
strcpy(ptr->name, name);
ptr->version = version;
ptr->type=RIGHT;     /**!!!!! need to support stereo!!!!!! **/
strcpy(buf,studioBase->defaults.temp_files_path);
sprintf(temp,"%s.%03dL",ptr->name,ptr->version);
add(buf,temp);    /** append filename to path **/
ptr->lfp = creat(buf, 0);
printf("low_level_io.c: creating: %s\n",buf);
if (ptr->lfp==-1) {
   FreeMem(ptr->name, strlen(name)+1);
   FreeMem(ptr, sizeof(struct Disk_Samp));
   printf("can't open file (left)\n");
   return(CANT_OPEN_FILE);
   }

if (ptr->type==STEREO) {
   strcpy(buf,studioBase->defaults.temp_files_path);
   sprintf(temp,"%s.%03dR",ptr->name,ptr->version);
   add(buf,temp);    /** append filename to path **/
   ptr->rfp = creat(buf, 0);
   printf("LOW_LEVEL_IO.C: creating: %s\n",buf);
   if (ptr->rfp==-1) {
      printf("can't open file (right)\n");   /**!!!!!!!!!!!!!!**/
      close(ptr->lfp);
      FreeMem(ptr->name, strlen(name)+1);
      FreeMem(ptr, sizeof(struct Disk_Samp));
      return(CANT_OPEN_FILE);
      }
   }


ptr->flags = SAMPLE_OPEN;  /** redundant now !!**/
global1_ptr = ptr;
global1_pos = 0;

global2_ptr = ptr;
global2_pos = 0;
AddTail(&studioBase->samps, ptr);
*ptrptr = ptr;
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

/*############################################################
NAME close_sample -

FUNCTION
   closes all files and de-allocates all memory associated with
   a file sample.         
INPUTS
   pointer to Disk_Samp struct
RESULTS
   standard error code
##############################################################*/

int close_sample(ptr)
struct Disk_Samp *ptr;
{
int error;

error=NO_ERROR;
if (ptr->flags&DIRTY1) 
   error=update_block1(ptr, 0); /*!!should add a flush() and change this */
   
if (close(ptr->lfp)!=0) error=DISK_ERROR;

if (ptr->type==STEREO)
   if (close(ptr->rfp)!=0) error = DISK_ERROR;

global1_ptr = 0;
global1_pos = -1;
Remove(ptr);   /** This is the same as Remove(ptr->all_node); **/
if (!(ptr->flags&KEEP_NAME))  /** ????!!!!!!!!! *******/
   FreeMem(ptr->name,strlen(ptr->name)+1);
FreeMem(ptr, sizeof(struct Disk_Samp));
return(error);
}

