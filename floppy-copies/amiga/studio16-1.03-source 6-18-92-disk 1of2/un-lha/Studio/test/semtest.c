#include "//include/studio16.h"

/** lc -b0 -v **/

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();

struct SharedSemaphore lock;
struct GfxBase *GfxBase;
void MyDelay();

main()
{
struct Task *task1;
struct Task *task2;

GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==0)
	exit(10);

InitSharedSemaphore(&lock);
kprintf("\n\nDid Init\n");

/*** Test 1 ***/

task1=(struct Task *)CreateTask("Task_1",0,test1,5000);
MyDelay(30);
task2=(struct Task *)CreateTask("Task_2",0,test2,5000);
MyDelay(500);
kprintf("DELETE TASKS (Test 1)\n\n");
DeleteTask(task1);
DeleteTask(task2);

/*** Test 2 ***/

task1=(struct Task *)CreateTask("Task_1",0,test3,5000);
MyDelay(30);
task2=(struct Task *)CreateTask("Task_2",0,test4,5000);
MyDelay(500);
kprintf("DELETE TASKS (Test 2)\n\n");
DeleteTask(task1);
DeleteTask(task2);

/*** Test 3 ***/

test5();

/*** Test 4 ***/

task1=(struct Task *)CreateTask("Task_1",0,test6,5000);
MyDelay(30);
task2=(struct Task *)CreateTask("Task_2",0,test7,5000);
MyDelay(500);
kprintf("DELETE TASKS (Test 4)\n\n");
DeleteTask(task1);
DeleteTask(task2);


CloseLibrary(GfxBase);
}

/*************************************************/

void test1()
{
int i;

ObtainSharedSemaphore(&lock);
kprintf("1: got lock 1\n");
MyDelay(60);

ObtainExclusiveSemaphore(&lock);
kprintf("1: got lock 2\n");
MyDelay(60);

ReleaseExclusiveSemaphore(&lock);
kprintf("1: Free lock 2\n");
MyDelay(60);

ReleaseSharedSemaphore(&lock);
kprintf("1: free lock 1\n");

Wait(0);
}

/*************************************************/

void test2()
{
ObtainSharedSemaphore(&lock);
kprintf("2: got lock 1\n");
MyDelay(60);

ReleaseSharedSemaphore(&lock);
kprintf("2: free lock 1\n");

Wait(0);
}

/*************************************************/

void test3()
{
ObtainSharedSemaphore(&lock);
kprintf("3: got lock 1\n");
MyDelay(60);
ObtainSharedSemaphore(&lock);
kprintf("3: got lock 2\n");
MyDelay(60);
ReleaseSharedSemaphore(&lock);
kprintf("3: Freed lock 2\n");
MyDelay(60);
ReleaseSharedSemaphore(&lock);
kprintf("3: Freed lock 1\n");
Wait(0);
}

/*************************************************/

void test4()
{
ObtainSharedSemaphore(&lock);
kprintf("4: got lock 1\n");
MyDelay(60);
ObtainSharedSemaphore(&lock);
kprintf("4: got lock 2\n");
MyDelay(60);
ReleaseSharedSemaphore(&lock);
kprintf("4: Freed lock 2\n");
MyDelay(60);
ReleaseSharedSemaphore(&lock);
kprintf("4: Freed lock 1\n");
Wait(0);
}

/*************************************************/

void test5()
{
ObtainSharedSemaphore(&lock);
kprintf("5: got lock 1\n");
ObtainExclusiveSemaphore(&lock);
kprintf("5: got lock 2\n");
ReleaseExclusiveSemaphore(&lock);
kprintf("5: Freed lock 2\n");
ReleaseSharedSemaphore(&lock);
kprintf("5: Freed lock 1\n");
}

/*************************************************/

void test6()
{
ObtainExclusiveSemaphore(&lock);
kprintf("6: got lock 1\n");
MyDelay(60);
ObtainExclusiveSemaphore(&lock);
kprintf("6: got lock 2\n");
MyDelay(60);
ReleaseExclusiveSemaphore(&lock);
kprintf("6: Freed lock 2\n");
MyDelay(60);
ReleaseExclusiveSemaphore(&lock);
kprintf("6: Freed lock 1\n");
Wait(0);
}

/*************************************************/

void test7()
{
ObtainExclusiveSemaphore(&lock);
kprintf("7: got lock 1\n");
MyDelay(60);
ObtainExclusiveSemaphore(&lock);
kprintf("7: got lock 2\n");
MyDelay(60);
ReleaseExclusiveSemaphore(&lock);
kprintf("7: Freed lock 2\n");
MyDelay(60);
ReleaseExclusiveSemaphore(&lock);
kprintf("7: Freed lock 1\n");
Wait(0);
}

/*************************************************/

void MyDelay(x)
{
while (x--)
	WaitTOF();
}