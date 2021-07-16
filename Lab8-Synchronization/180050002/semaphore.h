#include "spinlock.h"

/*----------xv6 sync lab----------*/
#define NSEM 10

int sem_init(int, int);
int sem_up(int);
int sem_down(int);
struct sem_t{
	int value;
	struct spinlock lock;
	int cv;
};
struct sem_t kernel_semaphores[NSEM];
/*----------xv6 sync lab end----------*/
