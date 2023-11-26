#ifndef SMPTE_H
#define SMPTE_H TRUE

/* COPYRIGHT 1992 SunRize Industries */

#define SMPTE_ERROR_TIMEOUT	1
#define SMPTE_ERROR_INVALID	2
#define SMPTE_ERROR_JUMP	4

void __asm InjectTimeCode(register __d1 struct SmpteSource *,register __d2 int);
void __asm InjectTimeCodeError(register __d1 struct SmpteSource *,register __d3 int);
int __asm TestTimeCode(register __d2 int,register __d3 int);
int __asm IncTimeCode(register __d0 int,register __d3 int);
int __asm AddTimeCode(register __d0 int,register __d1 int,register __d3 int);
int __asm SubTimeCode(register __d0 int,register __d1 int,register __d3 int);

struct TimeCode {
   unsigned char hours;
   unsigned char minutes;
   unsigned char seconds;
   unsigned char frames;
   };

struct NewSmpteSource {
	char *name;
	int unit;
	unsigned long flags;
	};

struct SmpteSource {
	struct MinNode node;
	char *name;
	int unit;
	unsigned long flags;
	};

struct NewSmpteSink {
	int priority;
	unsigned long flags;
	void (*func)();
	APTR userdata;
	};

struct SmpteSink {
	struct Node node;
	unsigned long flags;
	void (*func)();
	APTR userdata;
	};
#endif
