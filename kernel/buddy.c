/*
this file is teh main part of the buddy-system-like allocator we made for xv6
*/
#include "buddy.h"
#include "types.h"
#include "defs.h"
#include "memlayout.h"
#include "tool.h"
#include "assert.h"
void *manage_start; // the start of the managed memory
void *manage_end;
struct zone zone;        // maybe just 1 zone in xv6
uint8 bitmap[MAX_PAGES]; // the bit map to track all the pa we can manage in the
                         // system
struct page manage_pages[MAX_MANAGE_PAGES]; // pages being managed in the system

static inline void __increase_pgref(struct page *page) { page->ref++; }
static inline void __decrease_pgref(struct page *page) { page->ref--; }
/// @brief get one locked invalid page, set the page valid
/// @return
static inline struct page *__get_one_invalid_pp() {
  struct page *pp = -1;
  for (int i = 0; i < MAX_MANAGE_PAGES; i++) {
    if ((manage_pages[i].flags & PAGE_VALID) == 0) {
      acquire(&manage_pages[i].splock);
      manage_pages[i].flags |= PAGE_VALID;
      manage_pages[i].indexp = i;
      __increase_pgref(pp);
      break;
    }
    return pp;
  }
}
/// @brief update the bitmap
/// @param page the reason we need to change the bitmap
/// @param set_one the action we take to the bitmap
static void __update_bitmap(struct page *page, int set_one) {
  // the page should contain the info about
  // the start of the pa and the order
  // if page == null, just make start be the manage_start
  // and the order is the MAX_PAGES
  unsigned int order = MAX_PAGES;
  void *pa_satrt = manage_start;
  if (page) {
    pa_satrt = PFN2PA(page->pfn);
    order = page->order;
  }
  int bmindex = 0;
  int bitindex = 0;
  int m = 0;
  for (char *start = pa_satrt - manage_start;
       start >= manage_start && start < PHYSTOP; start += PGSIZE) {

    bmindex = (unsigned long)start / PGSIZE / 8;
    bitindex = ((unsigned long)start / PGSIZE) % 8;
    m = 1 << bitindex;
    if (set_one)
      if (bitmap[bmindex] & m == 0) // the block is free
        bitmap[bmindex] |= m;
      else
        bitmap[bmindex] &= ~m;
  }
  return;
}

/// @brief // free the memory from start to end in an determined order
// if order == -1, as high as possible
// usually used in the initializing procedure
/// @param start pa_start
/// @param end pa_end
/// @param order the order we prefer
static void __buddy_free_range(void *start, void *end, int order) {
  if (order == -1)
    order = MAX_ORDER;
try_free:
}
void init_buddy() {

  memset(manage_pages, 0, sizeof(struct page) * MAX_MANAGE_PAGES);
  manage_end = PHYSTOP;
  manage_start = get_end();
  if (!is_power2(manage_end - manage_start)) {
    // tuncate the memory range to fit the 2^n restriction
    unsigned long target = ROUNDDOWN2(manage_end - manage_start);
    manage_start = target;
  }

  initlock(&zone.lock, "zone_lock");
  initlock(&zone.free_area->splock, "free_area_lock");
  initlock(&zone.active_area->splock, "active_area_lock");
  for (int i = 0; i < MAX_MANAGE_PAGES; i++) {
    initlock(&manage_pages->splock, "page_lock");
  }
  // now we truncate the memory end to the power of 2
  unsigned int order = get_order_2((manage_end - manage_start) / PGSIZE);
  if (order >= MAX_ORDER)
    panic("order out of memory range buddy init!");

  struct page *pp = __get_one_invalid_pp();
  pp->order = order;
  pp->pfn = PA2PFN((uint64)manage_start);
  __add_to_freelist(pp);

  zone.nr_free = (uint64)manage_end - (uint64)manage_start / PGSIZE;
  zone.nr_active = 0;
  zone.actual_max_order = order;
  release(&pp->splock);

  // __update_bitmap(0,0);
  __buddy_free_range(manage_start, manage_end, -1);
};

static struct page *__alloc_pages(uint64 gfp_mask, unsigned int order) {
  struct page *pp;
find:
  if (!list_empty(&(zone.free_area[order].list))) {
    pp = get_page_from_freelist(gfp_mask, order);
  }

  return -1;
}
struct page *alloc_pages(uint64 gfp_mask, unsigned int order) {
  if (order < 0 || order > zone.actual_max_order ||
      power2(order) > zone.nr_free)
    return -1;
  return __alloc_pages(gfp_mask, order);
}

static inline unsigned long __find_buddy_pfn(unsigned long page_pfn,
                                             unsigned int order) {
  return page_pfn ^ (1 << order);
}
static struct page *get_page_from_freelist(unsigned long gfp_mask,
                                           unsigned int order) {
  if (order < 0 || order > zone.actual_max_order)
    return -1;
  struct page *pp;
find:
  if (!list_empty(&(zone.free_area[order].list))) {
    pp = __get_page_from_freelist(order);
    __increase_pgref(pp);
    pp->pfn |= gfp_mask;
    release(&pp->splock);
    goto done;
  } else {
    for (int i = order + 1; i < zone.actual_max_order + 1; i++) {
      __buddy_split(i);
      goto find;
    }
  }

done:
  acquire(&zone.lock);
  zone.nr_free -= pow2(pp->order);
  release(&zone.lock);
#ifdef CONFIG_DEBUG
  printf("[!DEBUG]: nr_free: %ld\n", zone.nr_free)
#endif
      return pp;
}

// the check should be token before calling this function

/// @brief return a locked order-indicated from freelist
static struct page *__get_page_from_freelist(unsigned int order) {

  acquire(&zone.free_area[order].splock);
  struct list_head *lh = &zone.free_area[order].list;
  struct list_head *next = lh->next;
  __list_del(lh, next->next);
  release(&zone.free_area[order].splock);

  struct page *pg = (struct page *)list_entry(lh, struct page, list);
  acquire(&pg->splock);
  pg->list = 0;
  pg->order = order;
  return pg;
}
static struct page *__get_page_from_activelist(unsigned int order) {

  acquire(&zone.active_area[order].splock);
  struct list_head *lh = &(zone.active_area[order].list);
  struct list_head *next = lh->next;
  __list_del(lh, next->next);
  release(&zone.active_area[order].splock);
  struct page *pg = (struct page *)list_entry(lh, struct page, list);
  acquire(&pg->splock);
  pg->list = 0;
  pg->order = order;
  return pg;
}
static struct page *__get_page_from_pm(void *pa_start, unsigned int order,
                                       uint64 gfp_mask) {
  char *p;
  void *pa_end = order * PGSIZE + 1; // one page for the struct page
  p = (char *)PGROUNDUP((uint64)pa_start);
  for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE) {
    if ((uint64)p % PGSIZE != 0 || pa_start < manage_start ||
        (uint64)p >= manage_end)
      panic("__alloc_pm");
    memset(p, 0, PGSIZE);
  }
  struct page *pg = pa_start;
  initlock(&pg->splock, "page_lock");
  pg->order = order;
  pg->flags |= gfp_mask;
  __increase_pgref(pg);
  pg->pfn = PA2PFN((uint64)pa_start + PGSIZE);
  pg->belg_zone = &zone;
  return pg;
}
static inline void __add_to_freelist(struct page *page) {
  list_add_tail(page->list, &zone.free_area[page->order].list);
}
static inline void __add_to_activelist(struct page *page) {
  list_add_tail(page->list, &zone.active_area[page->order].list);
}

static void __buddy_split(unsigned long order) {
  assert(order > 0 && order <= zone.actual_max_order);
  assert(!list_empty(&(zone.free_area[order].list)));

  struct page *page_a;
  struct page *page_b = __get_one_invalid_pp();
  assert(page_b != -1);
  page_a = get_page_from_freelist(0, order);

  page_a->order -= order - 1;
  page_b->order = order - 1;
  page_b->flags |= page_a->flags;
  page_b->pfn = SPLIT_CHILD_PFN(page_a->pfn, order);
  __add_to_activelist(page_a);
  __add_to_activelist(page_b);
  release(&page_a->splock);
  release(&page_b->splock);
}