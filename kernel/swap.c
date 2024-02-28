#include "swap.h"


/* 
 * 下述为lru链表的四种缓存 
 */
//加入lru链表的缓存
struct pagevec *lru_add_pvec;
//将移至非活动lru链表的末尾
struct pagevec *lru_rotate_pvecs;
//从活动lru链表移至非活动lru链表，强制清楚PG_activate和PG_referenced
struct pagevec *lru_deactivepage_pvecs;
//从非活动lru链表移至活动lru链表
struct pagevec *page_active_pvecs;


/*
 * 将新页面加入lru链表时，需要先将该页面加入到当前CPU的lru_add_pvec缓存中
 */
static void __lru_cache_add(struct page *page)
{
    /* 
     * 用pvec存放当前CPU的缓存 
     */
    struct pagevec *pvec = lru_add_pvec;

    /* 如果LRU缓存已满,将LRU缓存中的页放到LRU链表中 */
    if (!(PAGEVEC_SIZE - pvec->nr)))
        __pagevec_lru_add(pvec);

    /* 将page加入到此cpu的lru缓存中，注意，加入pagevec实际上只是将pagevec中的pages数组中的某个指针指向此页，如果此页原本属于lru链表，那么现在实际还是在原来的lru链表中 */
    pagevec_add(pvec, page);
    put_cpu_var(lru_add_pvec);
}


/* 将pagevec中的页加入到lru链表中，并且会将pvec->nr设置为0 */
void __pagevec_lru_add(struct pagevec *pvec)
{
    /* __pagevec_lru_add_fn为回调函数 */
    pagevec_lru_move_fn(pvec, __pagevec_lru_add_fn, NULL);

    for(int i = 0; i < pvec->nr; i++) {
        struct page *page = pvec->pages[i];
    }
}


/* 将缓存中的页做move_fn处理，然后对页进行page->_count--
 * 当所有页加入到lru缓存中时，都要page->_count++
 */
static void pagevec_lru_move_fn(struct pagevec *pvec,
    void (*move_fn)(struct page *page, struct lruvec *lruvec, void *arg),
    void *arg)
{
    int i;
    struct zone *zone = NULL;
    struct lruvec *lruvec;
    unsigned long flags = 0;

    /* 遍历pagevec中的所有页
     * pagevec_count()返回lru缓存pvec中已经加入的页的数量
     */
    for (i = 0; i < pvec->nr; i++) {
        struct page *page = pvec->pages[i];
    
        struct zone *pagezone = page_zone(page);

        /* 由于不同页可能加入到的zone不同，这样就是判断是否是同一个zone，是的话就不需要上锁了
         * 不是的话要先把之前上锁的zone解锁，再对此zone的lru_lock上锁
         */
        if (pagezone != zone) {
            /* 对之前的zone进行解锁，如果是第一次循环则不需要 */
            if (zone)
                spin_unlock_irqrestore(&zone->lru_lock, flags);
            /* 设置上次访问的zone */
            zone = pagezone;
            /* 这里会上锁，因为当前zone没有上锁，后面加入lru的时候就不需要上锁 */
            spin_lock_irqsave(&zone->lru_lock, flags);
        }

        /* 获取zone的lru链表 */
        lruvec = mem_cgroup_page_lruvec(page, zone);
        /* 将page加入到zone的lru链表中 */
        (*move_fn)(page, lruvec, arg);
    }
    /* 遍历结束，对zone解锁 */
    if (zone)
        spin_unlock_irqrestore(&zone->lru_lock, flags);
    /* 对pagevec中所有页的page->_count-- */
    release_pages(pvec->pages, pvec->nr, pvec->cold);
    /* pvec->nr = 0 */
    pagevec_reinit(pvec);
}