/*
in this file, we define the struct page for
some memory-related operations
*/
#ifndef PAGE_H
#define PAGE_H
#include "sleeplock.h"
#include "types.h"
struct page {
    struct sleeplock sllock; // protect the page 
    uint64 flags;// may needed for control
    uint64 pfn;
    int ref; // reference
    struct zone* belg_zone;
};

#define PAGE_ZONE(pg) ((struct *page)pg->belg_zone);

#endif