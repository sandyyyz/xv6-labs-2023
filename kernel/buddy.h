/*
this file declears the main data structure buddy system needed
and exposes the interface to the kernel buddy system provide
*/


#include "list.h"
#include "types.h"

#define MAX_ORDER (12)
#define MAX_ZONE_ORDER (12)

// define the migrate type
#define MIGRATE_USUAL 1
struct free_area {
    struct list_head;
    unsigned long nr_free;
};
struct zone {
    struct free_area free_area[MAX_ORDER];
    struct list_head free_list[MAX_ORDER];
    struct list_head active_list[MAX_ORDER]; 
};
struct pglist_data {
    struct free_area free_area[MAX_ORDER];
    struct zone zones[MAX_ZONE_ORDER];
    struct zone *zone_array;
};

// then we declear the interface buddy provided

void init_buddy();
struct page* alloc_pages(uint64 gfp, unsigned int order);
int free_pages(uint64 start_address, uint64 end_address);
int free_one_page(struct page* page);
int add_to_freelist(struct page* page, struct zone* zone, unsigned int order, int migratetype);
