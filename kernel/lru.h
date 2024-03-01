
#include "types.h"
#include "buddy.h"

#define LRU_BASE 0
#define LRU_ACTIVE 1
#define LRU_FILE 2

#define PAGEVEC_SIZE 14 //struct pagevec


enum lru_list {
	LRU_INACTIVE_ANON = LRU_BASE,//0
	LRU_ACTIVE_ANON = LRU_BASE + LRU_ACTIVE,//1
	LRU_INACTIVE_FILE = LRU_BASE + LRU_FILE,//2
	LRU_ACTIVE_FILE = LRU_BASE + LRU_FILE + LRU_ACTIVE,//3
	LRU_UNEVICTABLE,//4
	NR_LRU_LISTS//5
};


struct list_head {//Ë«ÏòÁ´±í
    struct list_head *next, *prev;
};


struct zone_reclaim_stat {
	/* The anon LRU stats live in [0], file LRU stats in [1]*/
	unsigned long recent_rotated[2];
	unsigned long recent_scanned[2];
};


struct lruvec {//Á´±íÃèÊö·û
    /*
	 * LRU_INACTIVE_ANON
	 * LRU_ACTIVE_ANON
	 * LRU_INACTIVE_FILE
	 * LRU_ACTIVE_FILE
	 * LRU_UNEVICTABLE
	 */
	struct list_head lists[NR_LRU_LISTS];
    
	struct zone_reclaim_stat reclaim_stat;
};


struct zone {
    struct lruvec lruvec;
};

static void __lru_add(struct page *page);

void rotate_reclaimable_page(struct page *page);

void deactivate_page(struct page *page);

void activate_page(struct page *page);

struct page *lru_to_page(struct list_head *page_list);

static inline void _list_del(struct list_head *entry);