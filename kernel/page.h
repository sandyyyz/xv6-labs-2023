/*
in this file, we define the struct page for
some memory-related operations
*/
#ifndef PAGE_H
#define PAGE_H
#include "spinlock.h"
#include "types.h"
struct page {
  struct spinlock splock; // protect the page
  uint64 flags;            // may needed for control
  uint64 pfn;
  uint64 indexp;            //index of the pages array
  unsigned int ref;   // reference
  unsigned int order; // order buddy made
  struct list_head *list;
  struct zone *belg_zone;
};

#define PAGE_ZONE(pg) ((struct * page) pg->belg_zone);

#define PFN2PA(pfn) (pfn << PGSHIFT)
#define PA2PFN(pa) (pa >> PGSHIFT)
#define PAGE_VALID 1 << 0
#define MAX_MANAGE_PAGES (1 << 10)
#endif