
#ifndef LIBRARIES_EXPANSION_H
#define LIBRARIES_EXPANSION_H   1
#define EXPANSIONNAME   "expansion.library"
#define ADNB_STARTPROC  0
#define ADNF_STARTPROC  (1<<0)
struct ConfigDev *AllocConfigDev();
CPTR AllocExpansionMem();
struct ConfigDev *FindConfigDev();
struct DeviceNode *MakeDosNode();
#endif !LIBRARIES_EXPANSION_H
