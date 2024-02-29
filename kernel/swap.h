
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


struct list_head {//双向链表
    struct list_head *next, *prev;
};


struct zone_reclaim_stat {
	/* The anon LRU stats live in [0], file LRU stats in [1]*/
	unsigned long recent_rotated[2];
	unsigned long recent_scanned[2];
};


struct lruvec {//链表描述符
    //5个双向链表头
	struct list_head lists[NR_LRU_LISTS];
    
	struct zone_reclaim_stat reclaim_stat;
};


struct zone {
    /* lru链表使用的自旋锁 
     * 当需要修改lru链表描述符中任何一个链表时，都需要持有此锁，也就是说，不会有两个不同的lru链表同时进行修改
     */
    //spinlock_t lru_lock;
    /* lru链表描述符 */
    struct lruvec lruvec;
};