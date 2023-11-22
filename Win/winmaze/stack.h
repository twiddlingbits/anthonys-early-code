#define MAX_STACKSIZE 5000

struct Stack {
	unsigned short sp;
	unsigned short x[MAX_STACKSIZE];
	unsigned short y[MAX_STACKSIZE];
};


BOOL PushStack(struct Stack* stack, unsigned short x, unsigned short y);
BOOL PopStack(struct Stack* stack, unsigned short *x, unsigned short *y);
void InitStack(struct Stack* stack);

