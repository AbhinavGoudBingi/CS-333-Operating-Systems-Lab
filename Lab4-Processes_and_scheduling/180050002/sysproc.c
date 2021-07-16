#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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

//prints hello to screen
int
sys_hello(void)
{
cprintf("Hello\n");
return 0;
}

//prints name to screen
int
sys_helloYou(void)
{
  char* s;
  if(argstr(0,&s)<0){
    return -1;
  }
  else{
  cprintf("%s\n",s);
}
return 0;
}

//returns number of active processes
int
sys_getNumProc(void)
{
  return getNumofProc();
}

//returns max pid of running processes
int
sys_getMaxPid(void)
{
  return getMaximumPid();
}

//prints process info
int
sys_getProcInfo(void)
{
  int pid;
  if(argint(0,&pid)<0){
    return -1;
  }
  struct processInfo* pi;
  if(argptr(1,(void*)&pi,sizeof(*pi))<0){
    return -1;
  }
  int x = getProcessInfo(pid,pi);
  if(x<0){
    return -1;
  }
  return 0;
}

//sets process priority
int
sys_setprio(void)
{
  int n;
  if(argint(0,&n)<0){
    return -1;
  }
  int pid = sys_getpid();
  return setpriority(n,pid);
}

//returns process priority
int
sys_getprio(void)
{
  int pid = sys_getpid();
  return getpriority(pid);
}