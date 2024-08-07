// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

struct {
  struct spinlock lock;
  int use_lock;
  int count[PHYSTOP / PGSIZE];
} refcount;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  initlock(&refcount.lock, "refcount");
  refcount.use_lock = 0;
  for(int i = 0 ; i < PHYSTOP/PGSIZE ; i++){
    refcount.count[i] = 0;
  }
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
  refcount.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");


  if(kmem.use_lock)
    acquire(&kmem.lock);
  // decrement refcount
  if(refcount.use_lock){
	  acquire(&refcount.lock);
	  if(V2P(v) / PGSIZE >= PHYSTOP / PGSIZE)
		  panic("kfree: refcount");
	  refcount.count[V2P(v) / PGSIZE]--;
	  if(refcount.count[V2P(v) / PGSIZE] < 0)
		  panic("kfree: refcount < 0");
	  if(refcount.count[V2P(v) / PGSIZE] != 0)
	  {
		  release(&refcount.lock);
		  if(kmem.use_lock)
			  release(&kmem.lock);
		  return;
	  }
	  release(&refcount.lock);
  }
  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
	// set refcount to 1 if page newly allocated
    if(refcount.use_lock){
      acquire(&refcount.lock);
      if(V2P((char*)r) / PGSIZE >= PHYSTOP / PGSIZE)
        panic("kalloc: refcount");
      refcount.count[V2P((char*)r) / PGSIZE] = 1;
      release(&refcount.lock);
    }
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

int
NumFreePages(void)
{
  struct run *r;
  int num = 0;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  while(r){
    num++;
    r = r->next;
    if((int)r >= PHYSTOP) break;
  }
  if(kmem.use_lock)
    release(&kmem.lock);

  return num;
}

void
incrementRefCount(char* v)
{
  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
		panic("incrementRefCount");
	if(refcount.use_lock){
		acquire(&refcount.lock);
		if(refcount.count[V2P(v) / PGSIZE] == 0)
		{
			panic("incrementRefCount: refcount == 0");
		}
		refcount.count[V2P(v) / PGSIZE]++;
		release(&refcount.lock);
	}
	return;
}
