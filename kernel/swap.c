#include "swap.h"


/* 
 * ����Ϊlru��������ֻ��� 
 */
//����lru����Ļ���
struct pagevec *lru_add_pvec;
//�������ǻlru�����ĩβ
struct pagevec *lru_rotate_pvecs;
//�ӻlru���������ǻlru����ǿ�����PG_activate��PG_referenced
struct pagevec *lru_deactivepage_pvecs;
//�ӷǻlru���������lru����
struct pagevec *page_active_pvecs;


/*
 * ����ҳ�����lru����ʱ����Ҫ�Ƚ���ҳ����뵽��ǰCPU��lru_add_pvec������
 */
static void __lru_cache_add(struct page *page)
{
    /* 
     * ��pvec��ŵ�ǰCPU�Ļ��� 
     */
    struct pagevec *pvec = lru_add_pvec;

    /* ���LRU��������,��LRU�����е�ҳ�ŵ�LRU������ */
    if (!(PAGEVEC_SIZE - pvec->nr)))
        __pagevec_lru_add(pvec);

    /* ��page���뵽��cpu��lru�����У�ע�⣬����pagevecʵ����ֻ�ǽ�pagevec�е�pages�����е�ĳ��ָ��ָ���ҳ�������ҳԭ������lru������ô����ʵ�ʻ�����ԭ����lru������ */
    pagevec_add(pvec, page);
    put_cpu_var(lru_add_pvec);
}


/* ��pagevec�е�ҳ���뵽lru�����У����һὫpvec->nr����Ϊ0 */
void __pagevec_lru_add(struct pagevec *pvec)
{
    /* __pagevec_lru_add_fnΪ�ص����� */
    pagevec_lru_move_fn(pvec, __pagevec_lru_add_fn, NULL);

    for(int i = 0; i < pvec->nr; i++) {
        struct page *page = pvec->pages[i];
    }
}


/* �������е�ҳ��move_fn����Ȼ���ҳ����page->_count--
 * ������ҳ���뵽lru������ʱ����Ҫpage->_count++
 */
static void pagevec_lru_move_fn(struct pagevec *pvec,
    void (*move_fn)(struct page *page, struct lruvec *lruvec, void *arg),
    void *arg)
{
    int i;
    struct zone *zone = NULL;
    struct lruvec *lruvec;
    unsigned long flags = 0;

    /* ����pagevec�е�����ҳ
     * pagevec_count()����lru����pvec���Ѿ������ҳ������
     */
    for (i = 0; i < pvec->nr; i++) {
        struct page *page = pvec->pages[i];
    
        struct zone *pagezone = page_zone(page);

        /* ���ڲ�ͬҳ���ܼ��뵽��zone��ͬ�����������ж��Ƿ���ͬһ��zone���ǵĻ��Ͳ���Ҫ������
         * ���ǵĻ�Ҫ�Ȱ�֮ǰ������zone�������ٶԴ�zone��lru_lock����
         */
        if (pagezone != zone) {
            /* ��֮ǰ��zone���н���������ǵ�һ��ѭ������Ҫ */
            if (zone)
                spin_unlock_irqrestore(&zone->lru_lock, flags);
            /* �����ϴη��ʵ�zone */
            zone = pagezone;
            /* �������������Ϊ��ǰzoneû���������������lru��ʱ��Ͳ���Ҫ���� */
            spin_lock_irqsave(&zone->lru_lock, flags);
        }

        /* ��ȡzone��lru���� */
        lruvec = mem_cgroup_page_lruvec(page, zone);
        /* ��page���뵽zone��lru������ */
        (*move_fn)(page, lruvec, arg);
    }
    /* ������������zone���� */
    if (zone)
        spin_unlock_irqrestore(&zone->lru_lock, flags);
    /* ��pagevec������ҳ��page->_count-- */
    release_pages(pvec->pages, pvec->nr, pvec->cold);
    /* pvec->nr = 0 */
    pagevec_reinit(pvec);
}