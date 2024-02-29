#include "swap.h"
#include "types.h"
#include "list.h"
#include "page.h"

struct lruvec *lruvec;

static void __lru_add(struct page *page) {

    int file = page->page_type;

    int active = page->active;

    int lru = (file << 1) + active;

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


/* 不会喵TT*/
// void rotate_reclaimable_page(struct page *page) {

//     if (!page->active && page->lru) {

//         unsigned long flags;

//         pagevec_move_tail(pvec);
//     }
// }


/* 将页移动到非活动lru链表中
 * 此页应该属于活动lru链表中的页
 */
void deactivate_page(struct page *page)
{
    /* 如果页被锁在内存中禁止换出，则跳出 */
    if (!page->evictable)
        return;

    /* page->_count == 1才会进入if语句 
     * 说明此页已经没有进程进行映射了
     */
    if (likely(get_page_unless_zero(page))) {
        struct pagevec *pvec = &get_cpu_var(lru_deactivate_pvecs);

        if (!pagevec_add(pvec, page))
            pagevec_lru_move_fn(pvec, lru_deactivate_fn, NULL);
        put_cpu_var(lru_deactivate_pvecs);
    }
}