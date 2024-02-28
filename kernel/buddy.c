/*
this file is teh main part of the buddy-system-like allocator we made for xv6
*/
#include "types.h"
#include "defs.h"
#include "memlayout.h"
struct zone zone; // maybe just 1 zone in xv6

void init_buddy(){
  void* end  = get_end();
  __buddy_free_range(end, (void*)PHYSTOP);
};

struct page *alloc_pages(uint64 gfp_mask, int order) {}

static inline unsigned long __find_buddy_pfn(unsigned long page_pfn,
                                             unsigned int order) {
  return page_pfn ^ (1 << order);
}
static struct page* get_page_from_freelist(unsigned long gfp_mask, unsigned int order) {
    
} 