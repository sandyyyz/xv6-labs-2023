
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
         * 加入到swap_cache中，同时修改page的某个标志位
         * 标志该page已在swap_cache中
         * 而page已在swap_cache中直接说明page已写入交换空间
         * 当再次调用shrink_page_list函数时，先检查该标志位
         * 若已标志，从物理内存和swap_cache中回收即可
         */
    }
}