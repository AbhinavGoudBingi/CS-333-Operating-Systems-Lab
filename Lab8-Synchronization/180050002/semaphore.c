/*----------xv6 sync lab----------*/
#include "types.h"
#include "x86.h"
#include "defs.h"
#include "semaphore.h"

int
sem_init(int index, int val)
{
  kernel_semaphores[index].value = val;
  initlock(&kernel_semaphores[index].lock, (char*) index);
  //to be done
  return 0;
}

int
sem_up(int index)
{
  acquire(&kernel_semaphores[index].lock);
  kernel_semaphores[index].value++;
  wake1((void*)index);
  release(&kernel_semaphores[index].lock);
  //to be done
  return 0;
}

int
sem_down(int index)
{
  acquire(&kernel_semaphores[index].lock);
  while(kernel_semaphores[index].value<=0){
  	sleep((void*) index, &kernel_semaphores[index].lock);
  }
  kernel_semaphores[index].value--;
  release(&kernel_semaphores[index].lock);
  //to be done
  return 0;
}

/*----------xv6 sync lab end----------*/
