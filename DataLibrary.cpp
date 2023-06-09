#include "stdafx.h"
#include "trs80configurator.h"
#include "LibraryDlg.h"

struct LibraryNodeData n1 = {
	NULL,
	"LDos",
	TRS80_SYSSOFT_DOS,
	TRS80_ROMTYPE_LV2_A,
	TRS80_DOSTYPE_L_A,
	TRS80_BOOTMODE_NORMAL,
	0,
	TRS80_CLK_MOD1,
	TRUE,
	TRUE,
	NULL,
	0};

struct LibraryNodeData n2 = {
	&n1,
	"NewDos 80",
	TRS80_SYSSOFT_DOS,
	TRS80_ROMTYPE_LV2_A,
	TRS80_DOSTYPE_NEW_A,
	TRS80_BOOTMODE_NORMAL,
	0,
	TRS80_CLK_MOD1,
	TRUE,
	TRUE,
	NULL,
	0};

struct LibraryNodeData n3 = {
	&n2,
	"BASIC L2 'R/S'",
	TRS80_SYSSOFT_ROM,
	TRS80_ROMTYPE_LV2_A,
	0,
	TRS80_BOOTMODE_NORMAL,
	0,
	TRS80_CLK_MOD1,
	TRUE,
	TRUE,
	NULL,
	0};

struct LibraryNodeData n4 = {
	&n3,
	"BASIC L2 'Memory Size?'",
	TRS80_SYSSOFT_ROM,
	TRS80_ROMTYPE_LV2_B,
	0,
	TRS80_BOOTMODE_NORMAL,
	0,
	TRS80_CLK_MOD1,
	TRUE,
	TRUE,
	NULL,
	0};

/////////////////////////////////////////////////////////////////

extern unsigned char GermanyData[];
extern int GermanyDataSize;

extern unsigned char QuestData[];
extern int QuestDataSize;

extern unsigned char TimeData[];
extern int TimeDataSize;

extern unsigned char AsteroidData[];
extern int AsteroidDataSize;

struct LibraryNodeData n10 = {
	NULL,
	"Space Hunt",
	TRS80_SYSSOFT_ROM,
	TRS80_ROMTYPE_LV2_B,
	0,
	TRS80_BOOTMODE_AUTORUN,
	TRS80_AUTORUNMODE_CMD,
	TRS80_CLK_MOD1,
	TRUE,
	TRUE,
	AsteroidData,
	AsteroidDataSize};

	struct LibraryNodeData n11 = {
	&n10,
	"Ruins at Time's Edge",
	TRS80_SYSSOFT_ROM,
	TRS80_ROMTYPE_LV2_B,
	0,
	TRS80_BOOTMODE_AUTORUN,
	TRS80_AUTORUNMODE_CMD,
	TRS80_CLK_MOD1,
	TRUE,
	TRUE,
	TimeData,
	TimeDataSize};

	struct LibraryNodeData n12 = {
	&n11,
	"Quest For Fire",
	TRS80_SYSSOFT_ROM,
	TRS80_ROMTYPE_LV2_B,
	0,
	TRS80_BOOTMODE_AUTORUN,
	TRS80_AUTORUNMODE_CMD,
	TRS80_CLK_MOD1,
	TRUE,
	TRUE,
	QuestData,
	QuestDataSize};

	struct LibraryNodeData n13 = {
	&n12,
	"Germany 1942",
	TRS80_SYSSOFT_ROM,
	TRS80_ROMTYPE_LV2_B,
	0,
	TRS80_BOOTMODE_AUTORUN,
	TRS80_AUTORUNMODE_CMD,
	TRS80_CLK_MOD1,
	TRUE,
	TRUE,
	GermanyData,
	GermanyDataSize};


//struct LibraryBranchData root1 = {NULL, "Scott Adams Adventure"};
//struct LibraryBranchData root2 = {&root1, "Clasic Arcade"};
struct LibraryBranchData root3 = {NULL, "AW Software",&n13};
struct LibraryBranchData root4 = {&root3, "Trs-80 ROM & DOS", &n4};

extern LibraryBranchData* LibraryDataRoot = &root4;

