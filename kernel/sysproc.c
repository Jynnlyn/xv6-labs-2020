#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h" 

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
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

uint64
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

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int mask;
  if(argint(0, &mask) < 0)
    return -1;
  myproc()-> trace_mask = mask;
  return 0;
}

uint64
sys_sysinfo(void)
{
  uint64 user_addr; // 用来存放用户传进来的指针地址
  struct sysinfo info; // 在内核里创建一个临时的结构体
  struct proc *p = myproc();

  // 1. 从用户空间获取那个指针的地址 (使用 argaddr 而不是 argint)
  if(argaddr(0, &user_addr) < 0)
    return -1;

  // 2. 收集数据 (调用你刚才在步骤2和3写的两个函数)
  // 注意：要想在这里调用那两个函数，你得先在 kernel/defs.h 里声明它们！
  info.freemem = count_free_mem(); 
  info.nproc = count_live_process();

  // 3. 将装满数据的 info 结构体，安全地复制回用户的地址空间
  // copyout(当前进程的页表, 用户目标地址, 内核源数据地址, 复制的字节数)
  if(copyout(p->pagetable, user_addr, (char *)&info, sizeof(info)) < 0)
    return -1;

  return 0;
}