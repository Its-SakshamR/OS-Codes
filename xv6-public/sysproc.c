#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "processInfo.h"

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

int
sys_hello(void)
{
  cprintf("Hello\n");
  return 0;
}

int
sys_helloYou(void)
{
  char *name;
  argstr(0,&name);
  cprintf("%s",name);
  return 0;
}

int
sys_getNumProc(void)
{
  return getNum();
}

int
sys_getMaxPid(void)
{
  return MaxPid();
}

int
sys_getProcInfo(void)
{
  int pid;
  if(argint(0, &pid) < 0){
    return -1;
  }

  int ptr;
  if(argint(1, &ptr) < 0){
    return -1;
  }

  struct processInfo* procinfo = (struct processInfo*)ptr;

  return ProcInfo(pid, procinfo);
}

int
sys_setprio(void)
{
  int i;
  if(argint(0, &i) < 0){
    return -1;
  }
  myproc()->prio = i;
  return 0;
}

int
sys_getprio(void)
{
  return myproc()->prio;
}

int
sys_welcomeFunction(void)
{
  int ptr;
  if(argint(0, &ptr) < 0){
    return -1;
  }
  char* xyz = (char*)ptr;
  myproc()->welcome_addr = xyz;
  return 0;
}

int
sys_welcomeDone(void)
{
  if(myproc()->return_addr){
    myproc()->tf->eip = (uint)(myproc()->return_addr);
    return 0;
  }

  return -1;
}

int
sys_numvp(void)
{
  return PGROUNDUP(myproc()->sz)/PGSIZE;
}

int
sys_numpp(void)
{
  return num_phy_page();
}

int
sys_mmap(void)
{
  int size;
  if(argint(0, &size) < 0){
    return -1;
  }

  if(size <= 0 || size % PGSIZE != 0){
    return 0;
  }

  int addr = myproc()->sz;
  int sz = addr + size;
  myproc()->sz = sz;

  return addr;
}