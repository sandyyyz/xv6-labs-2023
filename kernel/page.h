/*
in this file, we define the struct page for
some memory-related operations
*/
#ifndef PAGE_H
#define PAGE_H
#include "sleeplock.h"
#include "types.h"
#include "lru.h"

struct page {
    struct sleeplock sllock; // protect the page 
    uint64 flags;// may needed for control
    uint64 pfn;
    int ref; // reference
    struct zone* belg_zone;

    /* lru */
	uint page_type;
	uint active;
    uint evictable;
    uint dirty;
    uint reclaim;
    struct list_head lru;

    /* swap */
    uint in_swap_cache;

    struct page *next;
};

#define PAGE_ZONE(pg) ((struct *page)pg->belg_zone);

#endif