/*
this file declears the main data structure buddy system needed
and exposes the interface to the kernel buddy system provide
*/

#include "list.h"
#include "types.h"
#include "page.h"
#include "spinlock.h"
#include "riscv.h"

#ifdef CONFIG_MIGRATE
enum migratetype {
  MIGRATE_UNMOVABLE,
  MIGRATE_MOVABLE,
  MIGRATE_RECLAIMABLE,

  MIGRATE_TYPES
};
#endif

#define MAX_PAGES (PGROUNDUP(PHYSTOP) / PGSIZE)
#define MAX_ORDER (12)
#define MAX_ZONE_ORDER (12)

#ifdef CONFIG_MIGRATE
// define the migratype
#define MIGRATE_USUAL 1

#endif

struct free_area {
  struct spinlock splock;
  struct list_head list;
};
struct active_area {
  struct spinlock splock;
  struct list_head list;
};
struct zone {
  struct spinlock lock; // protect the list maybe
  unsigned long actual_max_order; // the actural order we manage 
  struct free_area free_area[MAX_ORDER];
  struct active_area active_area[MAX_ORDER];
  unsigned long nr_free;
  unsigned long nr_active;
};
struct pglist_data {
  struct free_area free_area[MAX_ORDER];
  struct zone zones[MAX_ZONE_ORDER];
  struct zone *zone_array;
};
#ifdef CONFIG_MIGRATE
struct sorder_region {
  struct spinlock lk;
  struct page *pg[MIGRATE_TYPES];
  struct list_head list_head;
};
#endif

// then we declear the interface buddy provided

void init_buddy();
struct page *alloc_pages(uint64 gfp, unsigned int order);
#define alloc_page(gfp_mask) alloc_pages(gfp_mask, 0);
int free_pages(uint64 start_address, uint64 end_address);
int free_one_page(struct page *page);
int add_to_freelist(struct page *page, struct zone *zone, unsigned int order,
                    int migratetype);
