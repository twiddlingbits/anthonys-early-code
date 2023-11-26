#include <exec/types.h>
#include <exec/libraries.h>
#include <libraries/dos.h>

struct ExtLibrary
{  struct Library lib;
   long seglist;
};

#define el lib->lib

struct ExtLibrary *CInit(lib)
struct ExtLibrary *lib;
{
  AddLibrary(lib);
  return(lib);
}

struct ExtLibrary *LibOpen(lib,version)
struct ExtLibrary *lib;
long version;
{
  ++el.lib_OpenCnt;
  el.lib_Flags &= ~LIBF_DELEXP;
  return(lib);
}

LibClose(lib)
struct ExtLibrary *lib;
{
  if (lib->lib.lib_OpenCnt)
  {
  --lib->lib.lib_OpenCnt;
  return(NULL);
  }
  if (lib->lib.lib_Flags & LIBF_DELEXP)
    return(LibExpunge(lib));
  return(NULL);
}

LibExpunge(lib)
struct ExtLibrary *lib;
{
  long seg;
  if (lib->lib.lib_OpenCnt)
    {
    lib->lib.lib_Flags |= LIBF_DELEXP;    
    return(NULL);
    }
    seg = lib->seglist;
    Remove(lib);
    return(seg);
}
