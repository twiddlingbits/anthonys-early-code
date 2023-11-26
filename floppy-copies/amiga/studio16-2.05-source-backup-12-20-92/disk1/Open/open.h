#define OL_LISTWID 50
#define OL_MAX_PATHS 8

#define OL_EMPTY 0
#define OL_NEEDSFILLING 1
#define OL_READY 2

static struct OLPath {
	struct NewList newlist; 
	struct NewButton newbut;
	char path[STUDIO16_MAX_PATH_SIZE];
	short state;
	};

#define OL_SHOW_REGIONS			(1<<0)

struct OpenListModState {
	struct StandardModState standard;
	short flags;
	};
	
