
#include "lru.h"
#include "types.h"
#include "list.h"
#include "page.h"
#include "swap.h"

static struct swap_info_struct swap_info[MAX_SWAPFILES];


static unsigned long shrink_page_list(struct list_head *page_list) {

    while(!list_empty(page_list)) {
        struct page *page;



        /* add the page to slot */
        __add_to_swap(page_list);
        /* change the PTE of page */
        __change_page_PTE(page_list);
        /* add the page to swap_cache */
        __add_to_swap_cache(page_list);
        /*
         * ���뵽swap_cache�У�ͬʱ�޸�page��ĳ����־λ
         * ��־��page����swap_cache��
         * ��page����swap_cache��ֱ��˵��page��д�뽻���ռ�
         * ���ٴε���shrink_page_list����ʱ���ȼ��ñ�־λ
         * ���ѱ�־���������ڴ��swap_cache�л��ռ���
         */
    }
}