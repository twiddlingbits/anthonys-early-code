#define INSTANCE_SHOW_UTILITIES 	(1<<0)
#define INSTANCE_SHOW_APPS			(1<<1)

struct InstanceModState {
	struct StandardModState standard;
	short flags;
	};
	
