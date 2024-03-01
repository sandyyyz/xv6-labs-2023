
#include "lru.h"
#include "types.h"
#include "list.h"
#include "page.h"

struct lruvec *lruvec;


static void __lru_add(struct page *page) {

    int file = page->page_type;

    int active = page->active;

    int lru = (file << 1) + active;

    if(!page->evictable) lru = 5;

    /* add page to lru_list */
    struct list_head *head = &lruvec->lists[lru];

    struct list_head *entry = &page->lru;

    struct list_head *prev = head;

    struct list_head *next = head->next;

    next->prev = entry;

    entry->next = next;

    entry->prev = prev;

    prev->next = entry;

    /* update page reclaim_stat */
    struct zone_reclaim_stat *reclaim_stat = &lruvec->reclaim_stat;

	reclaim_stat->recent_scanned[file]++;
	
    if (active)
		reclaim_stat->recent_rotated[file]++;
}


void rotate_reclaimable_page(struct page *page) {

    if (!page->active) {

        int file = page->page_type;

        int active = page->active;

        int lru = (file << 1) + active;

        if(!page->evictable) lru = 5;

        /* list move tail */
        struct list_head *list = &page->lru;

        struct list_head *head = &lruvec->lists[lru];

        struct list_head *prev = list->prev;

        struct list_head *next = list->next;

        next->prev = prev;

        prev->next = next;

        struct list_head *entry = list;

        prev = head->prev;

        next = head;

        next->prev = entry;

        entry->next = next;

        entry->prev = prev;

        prev->next = entry;
    }
}


void deactivate_page(struct page *page) {

    if (!page->evictable || !page->active)
        return;

    int file = page->page_type;

    int active = page->active;

    int lru = (file << 1) + active;

    /* delete page from lru_list */
    struct list_head *entry = &page->lru;

    struct list_head *prev = entry->prev;

    struct list_head *next = entry->next;

    next->prev = prev;

    prev->next = next;

    /* ClearPageActive */
    page->active = 0;

    /* add page to lru_list */
    __lru_add(page);

    if (page->dirty) {
        page->reclaim = 1;
    } else {
        rotate_reclaimable_page(page);
    }

    /* update page reclaim_stat */
	struct zone_reclaim_stat *reclaim_stat = &lruvec->reclaim_stat;

	reclaim_stat->recent_scanned[file]++;
}


void activate_page(struct page *page) {

    if (!page->evictable || page->active)
        return;

    int file = page->page_type;

    int active = page->active;

    int lru = (file << 1) + active;

    /* delete page from lru_list */
    struct list_head *entry = &page->lru;

    struct list_head *prev = entry->prev;

    struct list_head *next = entry->next;

    next->prev = prev;

    prev->next = next;

    /* ClearPageActive */
    page->active = 1;

    /* add page to lru_list */
    __lru_add(page);

    if (page->dirty) {
        page->reclaim = 1;
    }

    /* update page reclaim_stat */
	struct zone_reclaim_stat *reclaim_stat = &lruvec->reclaim_stat;

	reclaim_stat->recent_scanned[file]++;

    reclaim_stat->recent_rotated[file]++;
}

struct page *lru_to_page(struct list_head *page_list)
{
    /* get the element at the end of the LRU linked list 
     * Because pages at the end of the linked list are 
     * the least frequently accessed in the LRU management strategy, 
     * they are good candidates for recycling.
     */
    return page_list->prev;
}

static inline void _list_del(struct list_head *entry)
{
    struct list_head *prev = entry->prev;

    struct list_head *next = entry->next;

    next->prev = prev;

    prev->next = next;
}