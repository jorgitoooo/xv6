#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer)
{
	int can_insert = 0,
			insert_idx = 0,
			seg_exists = 0,
			seg_idx    = 0;

  acquire(&(shm_table.lock));
	int i;
	for(i = 0; i < 64; i++) {
		if(!can_insert && shm_table.shm_pages[i].id == 0) {
			can_insert = 1;
			insert_idx = i;
		}

		if(shm_table.shm_pages[i].id == id) {
			seg_exists = 1;
			seg_idx = i;
			break;
		}
	}

	if(!seg_exists) {
		if(!can_insert)
		{
  		release(&(shm_table.lock));
			return -1; // All shm_pages are beign used and segment doesn't exist
		}

		seg_idx = insert_idx;
		shm_table.shm_pages[seg_idx].id = id;

		if((shm_table.shm_pages[seg_idx].frame = kalloc()) == 0) {
  		release(&(shm_table.lock));
			return -1; // Could not allocate physical memory
		}

    memset(shm_table.shm_pages[seg_idx].frame, 0, PGSIZE);
		shm_table.shm_pages[seg_idx].refcnt = 0;
	}

	struct shm_page *seg = &shm_table.shm_pages[seg_idx];
	struct proc *p = myproc();

	uint pa = V2P(seg->frame);

	*pointer = (char *)PGROUNDUP(p->sz);

	if(mappages(p->pgdir, *pointer, PGSIZE, pa, PTE_W | PTE_U) != 0)
	{
  	release(&(shm_table.lock));
		return -1;
	}

	p->sz = (uint)(*pointer + PGSIZE);
	seg->refcnt++;
	
  release(&(shm_table.lock));

	return 0;
}


int shm_close(int id)
{
  acquire(&(shm_table.lock));
	int i;
	for(i = 0; i < 64; i++) {
		if(shm_table.shm_pages[i].id == id) {
			if(--shm_table.shm_pages[i].refcnt == 0) {
				struct proc *p = myproc();
				p->sz = deallocuvm(p->pgdir, p->sz, p->sz - PGSIZE);
				shm_table.shm_pages[i].id = 0;
				shm_table.shm_pages[i].frame = 0;
			}
		}
	}
  release(&(shm_table.lock));

	return 0;
}
