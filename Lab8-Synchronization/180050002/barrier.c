/*----------xv6 sync lab----------*/
#include "types.h"
#include "x86.h"
#include "spinlock.h"
#include "defs.h"
#include "barrier.h"

//define any variables needed here
struct spinlock barrier_lock;
int count;

int
barrier_init(int n)
{
  count = n;
  initlock(&barrier_lock, "barrier_lock");
  //to be done
  return 0;
}

int
barrier_check(void)
{
  acquire(&barrier_lock);
  //cprintf("%d\n",barrier_lock);
  count--;
  if(count>0){
  	sleep((void *)10,&barrier_lock);
  }
  else{
  	wakeup((void *)10);
  }
  release(&barrier_lock);
  //to be done
  return 0;
}

/*----------xv6 sync lock end----------*/
