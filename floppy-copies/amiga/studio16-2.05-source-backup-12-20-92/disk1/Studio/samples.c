#include <exec/types.h>
#include <exec/exec.h>
#include "/include/studio16.h"

extern struct StudioBase *StudioBase;
extern short se_info_sample_deleted;
extern short se_info_sample_created;

char *FindSample();
char *AllocCompletePath();
char *StrPathFileCat();
char *StrFileName();
void SlowFreeStr();

int errno, _OSERR;	/** rename() puts stuff here! **/

/** This file contains functions for creating/deleteing/manipulating **/
/** sample files.  File I/O with is in sio.c	 **/
/** and sampleio.c **/


/*############################################################
NAME	
	DeleteSample -

FUNCTION
	Given a complete path to a sample, this will attempt to
	delete's the sample's file + .graph file,
	and broadcasts the "SE_INFO_SAMPLE_DELETED"	event.
INPUTS
	name of sample to delete (not a complete path)
RESULTS
	standard error code
##############################################################*/

int DeleteSample(name)
char *name;
{
char buf[STUDIO16_MAX_PATH_SIZE];
char *str;
int i;

if (name==NULL)
	return(NO_ERROR);

str=(char *)AllocCompletePath(name,0);
i=DeleteFile(str);

strcpy(buf,str);
strncat(buf,".graph",sizeof(buf));
DeleteFile(buf);

if (i) {
	BroadcastAndLock(se_info_sample_deleted,str,0,0,0,0,0);
	i=NO_ERROR;
	}
else {
	i=STDERR_CANT_DELETE_FILE;
	}

UnlockedFreeMem(str,strlen(str)+1);
return(i);
}

/*############################################################
NAME	
	FindSample -
FUNCTION
	Searchs for the specified sample in each of the default.samp_paths.
	Treats upper and lower case the same.
	Must be called from a DOS Process, not just an Exec Task.
	Regions are ignored.  For example, "fred.region" would
	find the sample "fred"
INPUTS
	pointer to name to search for
RESULTS
	pointer to the path where sample was found, or NULL.
##############################################################*/

char *FindSample(name)
char *name;
{
long lock;
char pn[STUDIO16_MAX_PATH_SIZE];
char *path;
char *regionname;

if (name==0)
	return(0);

for (path=StudioBase->defaults.samp_paths; *path; path=(char *)strchr(path,0)+1) {
	strcpy(pn, path);
	StrPathFileCat(pn, name);
	regionname=(char *)strchr(StrFileName(pn),REGION_DELIMITER_CHAR);
	if (regionname)
		*regionname=NULL;	/** thus null replaces '.' in path **/
	
	if (lock=Lock(pn, ACCESS_READ)) {
		UnLock(lock);
		return(path);
		}
	}	
return(NULL);
}

/*############################################################
NAME	
	RenameSample -
FUNCTION
	Rename's a sample.  sample_deleted & sample_created messages
	are sent out.  A .graph file is also renamed, if one
	exists.
INPUTS
	src_path		path of file to rename
	src_name		name of file to rename.  Can be null if 
					src_path is complete.
	dest_path	path of file to rename to.
	dest_name	name of file to rename to.  Can be null if dest_path
					is complete.
RESULTS
	stanard error code
##############################################################*/

int RenameSample(src_path,src_name,dest_path,dest_name)
char *src_path, *dest_path;
char *src_name, *dest_name;
{
int error;
char *from;
char *to;
int retval;
char sp[STUDIO16_MAX_PATH_SIZE];
char dp[STUDIO16_MAX_PATH_SIZE];

from=(char *)AllocCompletePath(src_path,src_name);
to=(char *)AllocCompletePath(dest_path,dest_name);
retval=!rename(from,to);
error=STDERR_CANT_DO_RENAME_FROM_TEMP;
if (retval) {	/** do .graph file as well **/
	error=NO_ERROR;
	BroadcastAndLock(se_info_sample_deleted,from,0,0,0,0,0);
	BroadcastAndLock(se_info_sample_created,to,0,0,0,0,0);
	strcpy(sp,from);
	strcat(sp,".graph");
	strcpy(dp,to);
	strcat(dp,".graph");
	rename(sp,dp);
	}
UnlockedFreeMem(from, strlen(from)+1);
UnlockedFreeMem(to, strlen(to)+1);

return(error);
}

/*############################################################
NAME	
	AllocCompletePath
FUNCTION
	Given a path and name pointer, this function will combine
	them into a complete path.  New memory is alloced using
	Exec AllocMem() to hold the complete path.  The callre
	of this functions is responsable for freeing the memory
	when finished witht the complete path.
INPUTS
	path	pointer to path.  E.G "DH1:Project1"
	name	pointer to a name (NULL okay.)  E.G "Bang"
RESULTS
	pointer to new complete path.  E.G "DH1:Project1/Bang"
##############################################################*/

char *AllocCompletePath(path, name)
char *path;
char *name;
{
char buf[STUDIO16_MAX_PATH_SIZE];
char *cp;

strcpy(buf,path);
if (name)
	StrPathFileCat(buf,name);
cp=(char *)AllocMem(strlen(buf)+1,0);
if (cp)
	strcpy(cp,buf);
return(cp);
}

/*############################################################
NAME	
	StrFileName
FUNCTION
	Given a string that is a complete path, this returns a
	pointer to the file name.  For example, if passed
	"DH0:dir1/fred", it would return a pointer to "fred".
INPUTS
	path
RESULTS
	Pointer to file name at end of path
##############################################################*/

char *StrFileName(char *path)
{
char *name;

name=path+strlen(path)-1;

while (*name!=':' && *name!='/' && path!=name)
		name--;

if (path==name)
	return(name);
else
	return(name+1);	/** skip ':' or '/' **/
}

/*############################################################
NAME	
	StrRegionName
FUNCTION
	Given a string that is a complete or partial path, this returns a
	pointer to the region name.  For example, if passed
	"DH0:dir1/fred>regiona", it would return a pointer to "regiona"
INPUTS
	path
RESULTS
	Pointer to region name at end of path or pointer to NULL if no region
##############################################################*/

char *StrRegionName(char *path)
{
char *name;

name=path+strlen(path)-1;

while (*name!=REGION_DELIMITER_CHAR && path!=name) {
	if (*name==':' || *name=='/')
		return(path+strlen(path));		/** no region, return null **/
	name--;
	}

if (path==name)
	return(path+strlen(path));		/** no region, return null **/
else
	return(name+1);	/** skip region delimiter **/
}

/*############################################################
NAME	
	StrLastDir
FUNCTION
	Given a string that is a path, this returns a
	pointer to the last directory or device.  For example,
	"DH0:dir1", it would return a pointer to "dir1" and
	"DH0:" would return "DH0:".
INPUTS
	path
RESULTS
	Pointer to last dir, or if none exists, pointer to device.
##############################################################*/

char *StrLastDir(char *path)
{
char *name;
char *last;

name=path;
do {
	last=name;
	name=path;
	while (*path!=':' && *path!='/' && *path!=NULL)
		path++;
	} while (*path++);
if (*name)
	return(name);
else
	return(last);
}

/*############################################################
NAME	
	StrPathFileCat
FUNCTION
	Concatinates a dos path and filename together resulting in a
	complete path in the path field
INPUTS
	char *path  char *file
RESULTS
	char * to complete path (same as *path)
###############################################################*/
char *StrPathFileCat(path,file)
char *path,*file;
{
int pathlen;

if (*path==0) {
	strcpy(path,file);
	return(path);
	}
pathlen=strlen(path);
if (path[pathlen-1]==':' || path[pathlen-1]=='/') {
	strcpy(path+pathlen,file);
	return(path);
	}
path[pathlen]='/';
strcpy(path+pathlen+1,file);
return(path);
}
