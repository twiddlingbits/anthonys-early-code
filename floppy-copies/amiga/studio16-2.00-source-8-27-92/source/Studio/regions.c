#include <exec/types.h>
#include <exec/exec.h>
#include <stdio.h>
#include "/include/soundedit.h"
#include "/include/studio16.h"

/*############################################################
NAME	
	FindRegionBySfp

FUNCTION
	Given a pointer to an Open Sample, this function will search
	the sample for a region who matches the passed name.
INPUTS
	sfp			Indicates which sample to scan
	regionname	just the region name.  No sample name prefixs
	regin			This is filled if regionname is found.
RESULTS
	standard error code
##############################################################*/

int FindRegionBySfp(sfp, regionname, region)
struct SampleFilePointer *sfp;
char *regionname;
struct Region *region;
{
int i;

if (regionname[0])
	for (i=0; i < SFT_NUM_REGIONS; i++)
		if (stricmp(regionname,sfp->tag.dregions[i].region_name)==0) {
			*((struct DiskRegion *)region)=sfp->tag.dregions[i];
			strcpy(region->samp_name, sfp->samp_name);
			return(NO_ERROR);
			}
return(STDERR_CANT_FIND_REGION);
}


/*############################################################
NAME	
	AddRegionBySfp

FUNCTION
	Given a pointer to an Open Sample, this function will 
	add a new region entry.
INPUTS
	sfp			Indicates which sample to add to
	regin			Copies this information into a free region entry
					in sfp->tag.dregions.
RESULTS
	standard error code
##############################################################*/

int AddRegionBySfp(sfp, region)
struct SampleFilePointer *sfp;
struct Region *region;
{
int i;

if (sfp->ff->flags&FF_REGIONS) {
	for (i=0; i < SFT_NUM_REGIONS; i++)
		if (sfp->tag.dregions[i].region_name[0]==NULL) {
			sfp->tag.dregions[i]=*((struct DiskRegion *)region);
			return(NO_ERROR);
			}
		}
return(STDERR_CANT_ADD_REGION);
}

/*############################################################
NAME	
	DeleteRegionBySfp

FUNCTION
	Given a pointer to an Open Sample, this function will search
	the sample for a region who matches the passed name and remove
	it.
INPUTS
	sfp			Indicates which sample to scan
	regionname	just the region name.  No sample name prefixs
RESULTS
	standard error code
##############################################################*/

int DeleteRegionBySfp(sfp, regionname)
struct SampleFilePointer *sfp;
char *regionname;
{
int i;

if (regionname[0])
	for (i=0; i < SFT_NUM_REGIONS; i++)
		if (stricmp(regionname,sfp->tag.dregions[i].region_name)==0) {
			setmem(&sfp->tag.dregions[i], sizeof(struct DiskRegion), 0);
			return(NO_ERROR);
			}
return(STDERR_CANT_FIND_REGION);
}

/*############################################################
NAME	
	FindRegion

FUNCTION
	Given a region name, this will fill in the passed region structure.
	The region name can have the from of "SampleName" or
	"SampleName.Region".  For example "Fred" would return a region
	encompassing the entire sample "Fred", and "Fred.Yow" would
	return the "Yow" region, which should be defined in "Fred"

	This function requires complete paths.  EG "DH0:proj1/samp.reg"
INPUTS
	name			SampleName.RegionName (complete path!)
	regio			This is filled in if name is found.
RESULTS
	standard error code
##############################################################*/


int FindRegion(name, region)
char *name;
struct Region *region;
{
struct SampleFilePointer *sfp;
char *sname;
char *regionname;
char *path;
int error;

path=(char *)AllocCompletePath(name, 0);	/** Dup **/
sname=(char *)StrFileName(path);
regionname=(char *)strchr(sname,REGION_DELIMITER_CHAR);
if (regionname)
	*regionname=NULL;	/** thus null replaces '>' in path **/

sfp=(struct SampleFilePointer *)SampleOpen(path,SP_MODE_OLDFILE, 0);
if (sfp==NULL) {
	FreeMem(path,strlen(name)+1);
	return(STDERR_CANT_OPEN_SAMPLE);
	}

if (regionname) {
	error=FindRegionBySfp(sfp, regionname+1, region);
	}
else {
	region->region_name[0]=0;
	strcpy(region->samp_name, path);
	region->start=0;
	region->end=sfp->tag.length-1;
	region->parms=sfp->tag.parms;
	region->flags=0;
	error=NO_ERROR;
	}
FreeMem(path,strlen(name)+1);
SampleClose(sfp);
return(error);
}
