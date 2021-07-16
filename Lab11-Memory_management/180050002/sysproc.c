#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

pte_t* walkpgdir(pde_t *pgdir, const void *va, int alloc);

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_getNumFreePages(void)
{
  struct run *r;
  int count = 0;
  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  while(r){
    r = r->next;
    count++;
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return count;
  //return getNumFreePages();
}

int sys_getNumVirtPages(void)
{
  int count = 0;
  int size = myproc()->sz;
  for(int p=0; p + PGSIZE <= size; p += PGSIZE){
    count++;
  }
  return count;
}

int sys_getNumPhysPages(void)
{
  pde_t *pgdir = myproc()->pgdir;
  pte_t *pte;
  int count = 0;
  int size = myproc()->sz;
  for(int i = 0; i < size; i += PGSIZE){
    if((pte = walkpgdir(pgdir, (void *) i, 0)) == 0)
      panic("copyuvm: pte should exist");
    if(!(*pte & PTE_P))
      panic("copyuvm: page not present");
    count++;
  }
  return count;
}

int sys_getNumPTPages(void)
{
  int count = 1;
  pde_t* pgdir = myproc()->pgdir;
  for(int i = 0; i < NPDENTRIES; i++){
    if(pgdir[i] & PTE_P){
      count++;
    }
  }
  return count;
}
