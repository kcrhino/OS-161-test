#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>
static volatile int corruptCounter;
static struct semaphore *tsem = NULL;
static struct lock *lock = NULL;
static struct spinlock sl;
static
void
init_sem(void)
{
	if (tsem==NULL) {
		tsem = sem_create("tsem", 0);
		if (tsem == NULL) {
			panic("threadtest: sem_create failed\n");
		}
	}
	if(lock == NULL) {
		lock = lock_create("lock");
		if(lock == NULL){ kprintf("Lock creation failed!"); }
	}
	spinlock_init(&sl);
}
static
void
unsafethreads(void *junk, unsigned long num){
	(void)junk;
	for(int i = 0; i < (int)num; i++){
		corruptCounter++;
	}
	V(tsem);
}

static
void
lockthreads(void *junk, unsigned long num){
	(void)junk;
	for(int i = 0; i < (int)num; i++){
		lock_acquire(lock);
		corruptCounter++;
		lock_release(lock);
}
V(tsem);
}
static
void
spinlockthreads(void *junk, unsigned long num){
	(void)junk;
	for(int i = 0; i < (int)num; i++){
		spinlock_acquire(&sl);
		corruptCounter++;
		spinlock_release(&sl);
	}
	V(tsem);
}

static
void
printThread(void *junk, int num)
{
	(void)junk;
	kprintf("%d ", num);
	V(tsem);
}


static
void
runthreads(int threadCount, int increment, int testType)
{
	char name[16];
	int result;
	void (*tType)();
	corruptCounter = 0;

	switch(testType){
	case 0:
		tType = printThread;
		break;
	case 1:
		tType = unsafethreads;
		break;
	case 2:
		tType = lockthreads;
		break;
	case 3:
		tType = spinlockthreads;
	default:
		break;
	}


	init_sem();
	for(int i = 0; i < threadCount; i++){
	result = thread_fork(name, NULL, tType, NULL, (testType !=0) ? increment : i); 
	if(result)( panic("thread_fork failed %s\n", strerror(result))); }
			if(testType == 0) {
		for(int o = 0; o < 2000; o++){}
			}
	for (int i=0; i<threadCount; i++) {
		P(tsem);
	}
}


int
digitThreadTest(int nargs, char **args)
{
	(void)nargs;
	int argument = atoi(args[1]);

	init_sem();
	kprintf("Starting thread test...\n");
	runthreads(argument, 0, 0);
	kprintf("\nThread test done.\n");

	return 0;
}

int
unsafeThreadTest(int nargs, char **args)
{
 (void)nargs;
 int count = atoi(args[1]);
 int increment = atoi(args[2]);

 if(atoi(args[1]) && atoi(args[2])){
	kprintf("Expected result: %d\n", (count * increment));
	}

 runthreads(count, increment, 1);
 kprintf("Actual result: %d\n", corruptCounter);
return 0;}
 int
lockedThreadTest(int nargs, char **args) {
	(void)nargs;
	int count = atoi(args[1]);
	int increment = atoi(args[2]);
	
	if(atoi(args[1]) && atoi(args[2])){
		kprintf("Expected result: %d\n", (count * increment));
}
runthreads(count, increment, 2);
kprintf("Actual result: %d\n", corruptCounter);
return 0;
}
int
spinLockThreadTest(int nargs, char **args) {
	(void)nargs;
	int count = atoi(args[1]);
	int increment = atoi(args[2]);
	if(atoi(args[1]) && atoi(args[2])){
		kprintf("Expected result: %d\n", (count * increment));
		}
	runthreads(count, increment, 3);
	kprintf("Actual result: %d\n", corruptCounter);
	return 0;

}
