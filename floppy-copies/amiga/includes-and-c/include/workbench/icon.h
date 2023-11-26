#ifndef LIBRARIES_ICON_H
#define LIBRARIES_ICON_H
#define ICONNAME        "icon.library"
struct WBObject *GetWBObject(), *AllocWBObject();
struct DiskObject *GetDiskObject();
LONG PutWBObject(), PutIcon(), GetIcon(), MatchToolValue();
VOID FreeFreeList(), FreeWBObject(), AddFreeList();
char *FindToolType();
#endif !LIBRARIES_ICON_H
